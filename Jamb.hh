#pragma once

#include "Aeolus.hh"
#include "Command.hh"
#include "Launchpad.hh"

#include <bitset>

namespace jamb {
struct State {
  std::array<std::bitset<16>, 4> groups;
};

struct Model {
  Launchpad& launchpad_;
  Aeolus& aeolus_;

  void init();

 protected:
  State state_;
  void dispatch(Command);
  void emitState();
};

}  // namespace jamb
using Jamb = jamb::Model;