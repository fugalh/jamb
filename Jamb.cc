#include "Jamb.hh"
#include "common.hh"

void Jamb::init() {
  launchpad_.observer_ = [this](Command cmd) { dispatch(cmd); };
}

void Jamb::dispatch(Command cmd) {
  switch (cmd.type) {
    case Command::Type::GeneralCancel:
      aeolus_.generalCancel();
      break;
    case Command::Type::RecallPreset:
      aeolus_.programChange(cmd.u.preset);
      break;
    case Command::Type::StopToggle:
      aeolus_.stopToggle(cmd.u.stop.group, cmd.u.stop.button);
      break;
    default:
      LOGf("event type %d not yet handled", cmd.type);
  }
}
