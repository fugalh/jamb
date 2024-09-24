#include "Command.hh"
#include <fmt/format.h>

std::string Command::toString() const {
  switch (type) {
    case Type::GeneralCancel:
      return "general cancel";
    case Type::RecallCombination:
      return fmt::format("recall combination {}.{}", u.combo.memory,
                         u.combo.piston);
    case Type::StopToggle:
      return fmt::format("stop: group {} button {}", u.stop.group,
                         u.stop.button);
    case Type::MidiPanic:
      return "midi panic";
    case Type::SetCombination:
      return fmt::format("set combination {}.{}", u.combo.memory,
                         u.combo.piston);
  }
  return "invalid";
}