#include "Launchpad.hh"
#include "Jamb.hh"
#include "common.hh"

void Launchpad::init() {
  midi_.observer = [this](midi::Message msg) { dispatch(msg); };
  reset();
  grid(6, 8, {Color::Red, Intensity::Low});    // midi panic
  grid(7, 8, {Color::Amber, Intensity::Low});  // general cancel
}

void Launchpad::reset() {
  midi_.observer = [this](midi::Message msg) { dispatch(msg); };
  midi_.send({0xb0, {0, 0}});
}

void Launchpad::dispatch(midi::Message const msg) {
  // LOGf("%2x %2x %2x", msg.status, msg.data[0], msg.data[1]);
  if (msg.status == 0x90) {
    uint8_t button = msg.data[0];
    uint8_t val = msg.data[1];
    if (val != 0) {
      if (button < 0x80 && (button & 0x0f) < 8) {
        Command cmd{Command::Type::StopToggle};
        cmd.u.stop = gridToStop(button);
        emit(cmd);
      }
      if (button == 0x68) {
        emit({Command::Type::MidiPanic});
      }
      if (button == 0x78) {
        resetTopRow();
        emit({Command::Type::GeneralCancel});
      }
    }
    if (button == kSetButton) {
      auto const row = (button & 0xf0) >> 4;
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
  if (msg.status == 0xb0) {
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
      topRow(cmd.u.combo.piston, {Color::Green, Intensity::Mid});
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
void Launchpad::grid_(uint8_t loc, Button b) {
  auto const color = b.color;
  auto const intensity = b.intensity;
  auto const vel = velocity(color, intensity);
  if (loc < 0x80) {
    if ((loc & 0x0f) <= 8) {
      midi_.send({0x90, {loc, vel}});
    }
  } else if (loc < 0x88) {
    loc -= 0x80;
    loc += 0x68;
    midi_.send({0xb0, {loc, vel}});
  }
}

void Launchpad::resetTopRow() {
  auto s2 = state_;
  for (auto& x : s2.topRow) {
    x = {Launchpad::Color::Off, Launchpad::Intensity::Off};
  }
  render(s2);
}

Command::Stop Launchpad::gridToStop(uint8_t button) {
  Command::Stop stop;
  stop.group = (button & 0xf0) >> 5;
  stop.button = button & 0x0f;
  if (button & 0x10) {
    stop.button += 8;
  }
  return stop;
}

uint8_t Launchpad::velocity(Launchpad::Color color,
                            Launchpad::Intensity intensity) {
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

void Launchpad::topRow(uint8_t loc, Button button) {
  auto s2 = state_;
  s2.topRow.at(loc) = button;
  render(s2);
}

void Launchpad::jambStateUpdate(jamb::State const& j) {
  Button const off = {Launchpad::Color::Off, Launchpad::Intensity::Off};
  State s2 = state_;
  for (auto i = 0; i < 8; i++) {
    s2.topRow[i] = off;
  }

  if (j.activeCombination.has_value()) {
    s2.topRow[j.activeCombination->piston] = {Launchpad::Color::Green,
                                              Launchpad::Intensity::Mid};
  }

  for (auto i = 0; i < j.groups.size(); i++) {
    auto& g = j.groups[i];
    for (auto j = 0; j < g.size(); j++) {
      auto row = i * 2;
      auto col = j;
      if (j >= 8) {
        row++;
        col -= 8;
      }
      if (g[j]) {
        auto color = Launchpad::Color::Amber;
        if (i % 2) {
          color = Launchpad::Color::Red;
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
  // top row
  for (auto i = 0; i < 8; i++) {
    auto& b = s2.topRow[i];
    if (state_.topRow[i] != b) {
      grid_(0x80 + i, b);
    }
  }

  // grid (including rightmost column)
  for (auto row = 0; row < 8; row++) {
    for (auto col = 0; col < 9; col++) {
      auto& b = s2.grid[row][col];
      if (state_.grid[row][col] != b) {
        grid_((0x10 * row) | col, b);
      }
    }
  }

  state_ = s2;
}