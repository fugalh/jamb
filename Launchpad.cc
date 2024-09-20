#include "Launchpad.hh"
#include "common.hh"

void Launchpad::init() {
  midi_.observer = [this](midi::Message msg) { dispatch(msg); };
  midi_.send({0xB0, {0, 0}});
  midi_.send({0x90, {0x78, 0x0D}});  // light the general cancel button
}

void Launchpad::dispatch(midi::Message const msg) {
  // LOGf("%2x %2x %2x", msg.status, msg.data[0], msg.data[1]);
  if (msg.status == 0x90 && msg.data[1] != 0) {
    emit({Event::Type::ButtonPress, msg.data[0]});
  }
  if (msg.status == 0xb0 && msg.data[1] != 0) {
    // map the top row to row 8. That's a little unintuitive but saner than
    // shifting everything or having a negative row index.
    uint8_t button = msg.data[0] - 0x68 + 0x80;
    emit({Event::Type::ButtonPress, button});
  }
}