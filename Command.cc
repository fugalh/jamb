#include "Command.hh"

std::string Command::toString() const {
  switch (type) {
    case Type::GeneralCancel:
      return "general cancel";
  }
  return "invalid";
}