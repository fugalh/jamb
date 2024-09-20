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
    uint8_t button = msg.data[0];
    if (button < 0x80 && (button & 0x0f) < 8) {
      Command cmd{Command::Type::StopToggle};
      cmd.u.stop = gridToStop(button);
      emit(cmd);
    } else if (button == 0x78) {
      emit({Command::Type::GeneralCancel});
    }
  }
  if (msg.status == 0xb0 && msg.data[1] != 0) {
    Command cmd{Command::Type::RecallPreset};
    cmd.u.preset = msg.data[0] - 0x68;
    emit(cmd);
  }
}

void Launchpad::grid(uint8_t loc,
                     Launchpad::Color color,
                     Launchpad::Intensity intensity) {
  auto const vel = velocity(color, intensity);
  if (loc < 0x80) {
    if ((loc & 0x0f) < 8) {
      midi_.send({0x90, {loc, vel}});
    }
  } else if (loc < 0x88) {
    loc -= 0x80;
    loc += 0x68;
    midi_.send({0xb0, {loc, vel}});
  }
}
void Launchpad::resetTopRow() {
  for (auto i = 0; i < 8; i++) {
    grid(0x80 + i, Launchpad::Color::Off, Launchpad::Intensity::Off);
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
  green *= int(intensity);
  red *= int(intensity);
  uint8_t flags = 0x0c;
  return 0x10 * green + red + flags;
}

Command::Stop Launchpad::gridToStop(uint8_t button) {
  Command::Stop stop;
  stop.group = (button & 0xf0) >> 5;
  stop.button = button & 0x0f;
  if (button & 0x10) {
    stop.button += 8;
  }
  return stop;
}
