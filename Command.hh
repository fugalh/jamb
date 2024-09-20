#pragma once
#include <cstdint>
#include <string>

struct Command {
  enum class Type {
    GeneralCancel,
    RecallPreset,
    StopToggle,
    MidiPanic,
  };
  Type type;
  union {
    struct {
      uint8_t group;
      uint8_t button;
    } stop;
    uint8_t preset;
  } u;

  std::string toString() const;

  template <class STREAM>
  friend STREAM& operator<<(STREAM& os, const Command& x) {
    os << x.toString();
    return os;
  }
};
