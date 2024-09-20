#pragma once
#include <iostream>

struct Command {
  enum class Type {
    GeneralCancel,
  };
  Type type;

  std::string toString() const;
};

std::ostream& operator<<(std::ostream& os, const Command& x) {
  os << x.toString();
  return os;
}