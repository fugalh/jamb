#include "Launchpad.hh"

Launchpad::Launchpad(midi::Transport& midi, Launchpad::Observer observer)
    : midi_{midi}, observer_{observer} {
  midi_.observer = [this](midi::Message msg) { dispatch(msg); };
  midi_.send({0, 0xB, {0, 0}});
}

void Launchpad::dispatch(midi::Message const msg) {
  if (msg.channel == 0 && msg.status == 0x9 && msg.data[1] != 0) {
    emit({Event::Type::ButtonPress, msg.data[0]});
  }
}