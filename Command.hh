#pragma once
#include <cstdint>
#include <string>

struct Command {
  enum class Type {
    GeneralCancel,
    StopToggle,
    MidiPanic,
  };
  struct Stop {
    uint8_t group;
    uint8_t button;
  };
  Type type;
  union {
    Stop stop;
  } u;

  std::string toString() const;

  template <class STREAM>
  friend STREAM& operator<<(STREAM& os, const Command& x) {
    os << x.toString();
    return os;
  }
};
