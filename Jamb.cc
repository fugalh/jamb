#include "Jamb.hh"
#include "common.hh"

void Jamb::init() {
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

#include <yaml-cpp/yaml.h>

std::string Jamb::serializeState() {
  using std::map;
  using std::string;
  using std::vector;
  map<string, map<int, map<int, vector<string>>>> x;
  auto& mem = x["memory"];

  for (uint8_t m = 0; m < 1 << 4; m++) {  // memory number
    for (uint8_t p = 0; p < 8; p++) {     // piston number
      auto const addr = ComboAddr{m, p};
      if (state_.memory.contains(addr)) {
        for (auto const& g : state_.memory[addr]) {  // group
          mem[m][p].push_back(g.to_string('.', 'o'));
        }
      }
    }
  }

  YAML::Emitter out;
  out << x;
  return out.c_str();
}
