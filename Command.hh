#pragma once
#include <iostream>

struct Command {
  enum class Type {
    GeneralCancel,
    StopToggle,
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
};

std::ostream& operator<<(std::ostream& os, const Command& x) {
  os << x.toString();
  return os;
}