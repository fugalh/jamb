#include "Jamb.hh"
#include "common.hh"

#include <fmt/os.h>
#include <yaml-cpp/yaml.h>
#include <algorithm>
#include <fstream>

void Jamb::init(bool persistMemory) {
  persistMemory_ = persistMemory;
  launchpad_.observer_ = [this](Command cmd) { dispatch(cmd); };
}

void Jamb::dispatch(Command cmd) {
  switch (cmd.type) {
    case Command::Type::GeneralCancel: {
      state_.activeCombination = {};
      state_.groups = {};
      aeolus_.generalCancel();
      emitState();
      break;
    }
    case Command::Type::SetCombination:
      state_.memory[cmd.u.combo] = state_.groups;
      state_.activeCombination = cmd.u.combo;
      emitState();
      if (persistMemory_) {
        writeMemory();
      }
      break;
    case Command::Type::RecallCombination:
      if (state_.memory.contains(cmd.u.combo)) {
        state_.groups = state_.memory[cmd.u.combo];
        state_.activeCombination = cmd.u.combo;
        emitState();
      }
      break;
    case Command::Type::StopToggle: {
      auto const g = cmd.u.stop.group;
      auto const b = cmd.u.stop.button;
      state_.groups[g][b].flip();
      emitState();
      break;
    }
    case Command::Type::MidiPanic:
      aeolus_.allSoundOff();
      break;
    default:
      LOGf("event type %d not yet handled", cmd.type);
  }
}

void Jamb::emitState() {
  launchpad_.jambStateUpdate(state_);
  aeolus_.jambStateUpdate(state_);
}

using std::map;
using std::string;
using std::vector;

std::string Jamb::memoryString() {
  map<string, map<int, map<int, vector<string>>>> x;
  auto& mem = x["memory"];

  for (uint8_t m = 0; m < (1 << 4); m++) {  // memory number
    for (uint8_t p = 0; p < 8; p++) {       // piston number
      auto const addr = ComboAddr{m, p};
      if (state_.memory.contains(addr)) {
        for (auto const& g : state_.memory[addr]) {  // group
          auto bs = g.to_string('.', 'o');
          std::reverse(bs.begin(), bs.end());
          mem[m][p].push_back(bs);
        }
      }
    }
  }

  YAML::Emitter out;
  out << x;
  return out.c_str();
}

void Jamb::memoryFromString(std::string str) {
  auto x = YAML::Load(str);
  x = x["memory"];
  for (uint8_t m = 0; m < (1 << 4); m++) {  // memory number
    if (!x[m]) {
      continue;
    }
    for (uint8_t p = 0; p < 8; p++) {  // piston number
      if (x[m][p]) {
        auto const& v = x[m][p].as<std::vector<std::string>>();
        for (auto g = 0; g < v.size(); g++) {
          auto bs = v[g];
          std::reverse(bs.begin(), bs.end());
          state_.memory[{m, p}][g] = std::bitset<16>(bs, 0, 16, '.', 'o');
        }
      }
    }
  }
}

static std::string getConfigPath() {
  return fmt::format("{}/.jamb.memory", getenv("HOME"));
}

void Jamb::writeMemory() {
  std::ofstream out(getConfigPath());
  out << memoryString() << "\n";
}

void Jamb::readMemory() {
  std::ifstream input(getConfigPath());
  std::stringstream buf;
  buf << input.rdbuf();
  memoryFromString(buf.str());
}