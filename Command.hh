#pragma once
#include <cstdint>
#include <string>

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
  };
  struct Combination {
    uint8_t memory, piston;
    bool operator<(Combination other) const {
      return memory < other.memory || piston < other.piston;
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
