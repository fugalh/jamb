#include "Launchpad.hh"

void Launchpad::init() {
  midi_.observer = [this](midi::Message msg) { dispatch(msg); };
  midi_.send({0xB0, {0, 0}});
  midi_.send({0x90, {0x78, 0x0D}});  // light the general cancel button
}

void Launchpad::dispatch(midi::Message const msg) {
  if (msg.status == 0x90 && msg.data[1] != 0) {
    emit({Event::Type::ButtonPress, msg.data[0]});
  }
}