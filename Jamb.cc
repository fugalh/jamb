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