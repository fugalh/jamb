#pragma once
#include <cassert>

namespace launchpad {
using byte = uint8_t;

template <class T>
void clamp(T& x, int min, int max) {
  assert(min <= max);
  if (x < min)
    x = min;
  if (x > max)
    x = max;
}

struct Color {
  byte red = 0;
  byte green = 0;
  void fix() {
    clamp(red, 0, 3);
    clamp(green, 0, 3);
  }
};

struct Button {
  Color color{};
  bool clear{true};
  bool copy{true};
  void fix() { color.fix(); }
};

struct Position {
  byte x;
  byte y;
  void fix() {
    clamp(x, 0, 8);
    clamp(y, 0, 8);
    if (y == 0 && x == 8) {
      y = 1;
    }
  }
};

struct Midi {
  static byte velocity(Button b) {
    b.fix();
    return 0x10 * b.color.green + b.color.red + (b.clear << 3) + (b.copy << 2);
  }
  static byte key(Position g) {
    g.fix();
    assert(g.y != 0);
    if (g.y == 0) {
      return 8;
    }
    return g.x + (g.y - 1) * 0x10;
  }
  static uint32_t note_off(byte channel, byte key, byte velocity) {
    clamp(channel, 0, 3);
    return ((0x80 + channel) << 16) + (key << 8) + velocity;
  }
  static uint32_t note_on(byte channel, byte key, byte velocity) {
    clamp(channel, 0, 3);
    return ((0x90 + channel) << 16) + (key << 8) + velocity;
  }
  static uint32_t note_on(Position g, Button b) {
    g.fix();
    if (g.y == 0) {
      return 0xb06800 + (g.x << 8) + velocity(b);
    }
    return note_on(0, key(g), velocity(b));
  }
  static uint32_t note_off(Position g, Button b) {
    g.fix();
    if (g.y == 0) {
      return note_on(g, b) & 0xffffc0;
    }
    return note_off(0, key(g), velocity(b));
  }
};

}  // namespace launchpad