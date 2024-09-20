#include "Command.hh"
#include <fmt/format.h>

std::string Command::toString() const {
  switch (type) {
    case Type::GeneralCancel:
      return "general cancel";
    case Type::RecallPreset:
      return fmt::format("recall preset {}", u.preset);
    case Type::StopToggle:
      return fmt::format("stop: group {} button {}", u.stop.group,
                         u.stop.button);
    case Type::MidiPanic:
      return "midi panic";
  }
  return "invalid";
}