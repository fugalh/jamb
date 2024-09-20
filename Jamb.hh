#pragma once

#include "Aeolus.hh"
#include "Command.hh"
#include "Launchpad.hh"

struct Jamb {
  Launchpad& launchpad_;
  Aeolus& aeolus_;

  void init();

 protected:
  void dispatch(Command);
};