#include "Jamb.hh"
#include "common.hh"

void Jamb::init() {
  launchpad_.observer_ = [this](Command cmd) { dispatch(cmd); };
}

void Jamb::dispatch(Command cmd) {
  switch (cmd.type) {
    case Command::Type::GeneralCancel: {
      state_.activePreset = {};
      for (auto& g : state_.groups) {
        g.reset();
      }
      aeolus_.generalCancel();
      emitState();
      break;
    }
    case Command::Type::RecallPreset:
      aeolus_.programChange(cmd.u.preset);
      break;
    case Command::Type::StopToggle: {
      auto const g = cmd.u.stop.group;
      auto const b = cmd.u.stop.button;
      state_.groups[g][b].flip();
      if (state_.groups[g][b]) {
        aeolus_.stopOn(g, b);
      } else {
        aeolus_.stopOff(g, b);
      }
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
}