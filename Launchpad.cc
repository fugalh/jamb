#include "Launchpad.hh"

Launchpad::Launchpad(midi::Transport& midi) : midi_{midi} {
  midi_.send({0, 0xB, {0, 0}});
}