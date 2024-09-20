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

void Launchpad::grid(uint8_t loc,
                     Launchpad::Color color,
                     Launchpad::Intensity intensity) {
  auto const vel = velocity(color, intensity);
  if (loc < 0x80 && (loc & 0xf <= 8)) {
    midi_.send({0x90, {loc, vel}});
  } else if (loc < 0x88) {
    loc -= 0x80;
    loc += 0x68;
    midi_.send({0xb0, {loc, vel}});
  }
}

uint8_t Launchpad::velocity(Launchpad::Color color,
                            Launchpad::Intensity intensity) {
  uint8_t green = 0;
  uint8_t red = 0;
  if (color == Color::Green || color == Color::Amber) {
    green = 1;
  }
  if (color == Color::Red || color == Color::Amber) {
    red = 1;
  }
  uint8_t flags = 0;
  return 0x10 * green + red;
}