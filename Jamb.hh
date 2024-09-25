#pragma once

#include "Aeolus.hh"
#include "Command.hh"
#include "Launchpad.hh"

namespace jamb {
using ComboAddr = Command::Combination;
using Groups = Aeolus::State::Groups;
struct State {
  std::optional<Command::Combination> activeCombination;
  // Aeolus uses a "group" and "element" scheme for referring to buttons. These
  // are the groups and elements as arranged in the UI for that instrument.
  // Aeolus doesn't restrict it to 4 groups and 16 elements I don't think, and
  // may be less, but we just treat them as a fixed array of 4x16 because that's
  // what we can reasonably do with a Launchpad Mini.
  Groups groups;
  std::map<ComboAddr, Groups> memory;
};

struct Model {
  Launchpad& launchpad_;
  Aeolus& aeolus_;

  Model(Launchpad& lp, Aeolus& a) : launchpad_{lp}, aeolus_{a} {}

  void init(bool persistMemory = false);
  std::string memoryString();
  void writeMemory();

 protected:
  State state_{};
  bool persistMemory_{false};
  void dispatch(Command);
  void emitState();
};

}  // namespace jamb

using Jamb = jamb::Model;  // deprecated name