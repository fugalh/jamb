#pragma once
#include "Midi.hh"

struct Launchpad {
  midi::Transport& midi_;
  Launchpad(midi::Transport&);
};