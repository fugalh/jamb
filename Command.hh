#pragma once
#include <cstdint>
#include <string>

// Commands that can be sent from Launchpad (or later OSC) to jamb::Model
struct Command {
  enum class Type {
    GeneralCancel,
    SetCombination,
    RecallCombination,
    StopToggle,
    MidiPanic,
  };

  struct Stop {
    uint8_t group;
    uint8_t button;
    bool operator==(Stop const& other) const {
      return group == other.group && button == other.button;
    }
    bool operator<(Stop const& other) const {
      return std::tie(group, button) < std::tie(other.group, other.button);
    }
  };

  struct Combination {
    uint8_t memory, piston;
    bool operator<(Combination other) const {
      return std::tie(memory, piston) < std::tie(other.memory, other.piston);
    }
    std::string toString() {
      return std::to_string(memory) + "." + std::to_string(piston);
    }
  };

  Type type;
  union {
    Stop stop;
    Combination combo;
  } u;

  std::string toString() const;

  template <class STREAM>
  friend STREAM& operator<<(STREAM& os, const Command& x) {
    os << x.toString();
    return os;
  }
};
