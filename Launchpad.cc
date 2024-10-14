#include "Launchpad.hh"
#include "Jamb.hh"
#include "common.hh"

#include <yaml-cpp/yaml.h>

static uint8_t velocity(Launchpad::Color color,
                        Launchpad::Intensity intensity) {
  using Color = Launchpad::Color;
  uint8_t green = 0;
  uint8_t red = 0;
  if (color == Color::Green || color == Color::Amber) {
    green = 1;
  }
  if (color == Color::Red || color == Color::Amber) {
    red = 1;
  }
  green *= int(intensity);
  red *= int(intensity);
  uint8_t flags = 0x0c;
  return 0x10 * green + red + flags;
}

namespace launchpad {

Stopmap parseStopmapFromConfig(std::string config, std::string instrument) {
  Stopmap result;
  YAML::Node y = YAML::Load(config);

  using std::string;
  using std::vector;
  for (auto it : y) {
    if (it["instrument"]) {
      auto yInstr = it["instrument"];
      if (it["launchpad"] && it["instrument"].as<string>() == instrument) {
        std::stringstream ss;
        auto yLp = it["launchpad"];
        vector<int> groupmap = {0, 1, 2, 3};
        if (yLp["groupmap"]) {
          groupmap = yLp["groupmap"].as<vector<int>>();
        }
        auto sStopmap = yLp["stopmap"].as<string>();
        // strip leading whitespace
        auto beg = sStopmap.find_first_not_of(" \t\n");
        sStopmap.erase(0, beg);
        uint8_t row = 0, col = 0;
        bool success = false;
        for (char x : sStopmap) {
          if (x == '\n') {
            if (col < 8) {
              break;
            }
            row++;
            if (row >= 8) {
              success = true;
              break;
            }
            col = 0;
            continue;
          }
          if (col >= 8) {
            // if we go off the end, just keep going until we find a newline
            continue;
          }
          if (x == ' ') {
            continue;
          }
          if (x == '.') {
            col++;
            continue;
          }
          uint8_t const group = groupmap.at(col / 2);
          int elem;
          char buf[] = {x, '\0'};
          sscanf(buf, "%x", &elem);
          if (group >= 4 || elem >= 16) {
            LOG << "Bad data in config\n";
          } else {
            result[{row, col}] = {group, uint8_t(elem)};
          }
          col++;
        }
        if (success) {
          return result;
        } else {
          LOG << "Unable to interpret stopmap data";
        }
      }  // if we found an entry
    }
  }

  // fallback to direct columnar map
  Stopmap defaultResult;
  for (uint8_t row = 0; row < 8; row++) {
    for (uint8_t col = 0; col < 8; col++) {
      uint8_t const group = col / 2;
      uint8_t element = (7 - row) * 2;
      if (col & 1) {
        element++;
      }
      defaultResult[{row, col}] = {group, element};
    }
  }
  return defaultResult;
}

}  // namespace launchpad

void Launchpad::init() {
  midi_.send({midi::kController, {0, 0}});
  grid(6, 8, {Color::Red, Intensity::Low});    // midi panic
  grid(7, 8, {Color::Amber, Intensity::Low});  // general cancel
  grid(5, 8, {Color::Green, Intensity::Low});  // set

  midi_.observer = [this](midi::Message msg) { dispatch(msg); };

  configureStopmap(launchpad::kDefaultConfig);
}

void Launchpad::configureStopmap(std::string config) {
  stopmap_ = launchpad::parseStopmapFromConfig(config);
  gridmap_.clear();
  for (auto [button, stop] : stopmap_) {
    if (stop) {
      gridmap_[*stop] = button;
    } else {
      LOGf("No stop for %x%x", button.first, button.second);
    }
  }
}

void Launchpad::dispatch(midi::Message const msg) {
  if (msg.status == midi::kNoteOn) {
    uint8_t button = msg.data[0];
    uint8_t val = msg.data[1];
    if (val != 0) {
      if (button < 0x80 && (button & 0x0f) < 8) {
        Command cmd{Command::Type::StopToggle};
        auto stop = gridToStop(button);
        if (stop) {
          cmd.u.stop = *stop;
          emit(cmd);
        }
      }
      if (button == 0x68) {
        emit({Command::Type::MidiPanic});
      }
      if (button == 0x78) {
        emit({Command::Type::GeneralCancel});
      }
    }
    if (button == kSetButton) {
      auto const row = topFour(button);
      auto const col = button & 0x0f;
      if (val == 0) {
        grid(row, col, {Color::Green, Intensity::Low});
        state_.pressingSet = false;
      } else {
        grid(row, col, {Color::Green, Intensity::High});
        state_.pressingSet = true;
      }
    }
  }
  if (msg.status == midi::kController) {
    auto const piston = msg.data[0];
    if (piston >= 0x68 && piston <= 0x6f && msg.data[1] != 0) {
      Command cmd;
      if (state_.pressingSet) {
        cmd.type = Command::Type::SetCombination;
      } else {
        cmd.type = Command::Type::RecallCombination;
      }
      cmd.u.combo.memory = 0;
      cmd.u.combo.piston = msg.data[0] - 0x68;
      emit(cmd);
    }
  }
}

void Launchpad::grid(uint8_t row, uint8_t col, Button b) {
  auto s2 = state_;
  s2.grid.at(row).at(col) = b;
  render(s2);
}

// send the MIDI message to update the grid. the top row is mapped to 0x80+i
void Launchpad::setGridButton(uint8_t loc, Button b) {
  auto const color = b.color;
  auto const intensity = b.intensity;
  auto const vel = velocity(color, intensity);
  if (loc < 0x80) {
    if ((loc & 0x0f) <= 8) {
      midi_.send({midi::kNoteOn, {loc, vel}});
    }
  } else if (loc < 0x88) {
    loc -= 0x80;
    loc += 0x68;
    midi_.send({midi::kController, {loc, vel}});
  }
}

void Launchpad::resetTopRow() {
  auto s2 = state_;
  for (auto& x : s2.topRow) {
    x = {Launchpad::Color::Off, Launchpad::Intensity::Off};
  }
  render(s2);
}

std::optional<Command::Stop> Launchpad::gridToStop(uint8_t button) {
  Command::Stop stop;
  uint8_t row = topFour(button);
  uint8_t col = button & 0x0f;
  return stopmap_[{row, col}];
}

std::optional<std::pair<int, int>> Launchpad::stopToGrid(Command::Stop stop) {
  if (gridmap_.contains(stop)) {
    return gridmap_[stop];
  }
  return {};
}

void Launchpad::topRow(uint8_t loc, Button button) {
  auto s2 = state_;
  s2.topRow.at(loc) = button;
  render(s2);
}

void Launchpad::jambStateUpdate(jamb::State const& jState) {
  Button const off = {Launchpad::Color::Off, Launchpad::Intensity::Off};
  State s2 = state_;
  for (auto i = 0; i < 8; i++) {
    s2.topRow[i] = off;
  }

  if (jState.activeCombination.has_value()) {
    s2.topRow[jState.activeCombination->piston] = {Launchpad::Color::Green,
                                                   Launchpad::Intensity::Mid};
  }

  for (auto i = 0; i < jState.groups.size(); i++) {
    auto& g = jState.groups[i];
    for (auto j = 0; j < g.size(); j++) {
      Command::Stop stop{uint8_t(i), uint8_t(j)};
      auto oGrid = stopToGrid(stop);
      if (!oGrid.has_value()) {
        continue;
      }
      auto [row, col] = *oGrid;
      if (g[j]) {
        auto color = Launchpad::Color::Red;
        if (col & 0b10) {
          color = Launchpad::Color::Amber;
        }
        s2.grid[row][col] = {color, Launchpad::Intensity::Mid};
      } else {
        s2.grid[row][col] = off;
      }
    }
  }
  render(s2);
}

void Launchpad::render(State const& s2) {
  // pistons (top row)
  for (auto i = 0; i < 8; i++) {
    auto& b = s2.topRow[i];
    if (state_.topRow[i] != b) {
      setGridButton(0x80 + i, b);
    }
  }

  // grid (including rightmost column)
  for (auto row = 0; row < 8; row++) {
    for (auto col = 0; col < 9; col++) {
      auto& b = s2.grid[row][col];
      if (state_.grid[row][col] != b) {
        setGridButton((0x10 * row) | col, b);
      }
    }
  }

  state_ = s2;
}
