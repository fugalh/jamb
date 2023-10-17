#pragma once
#include <cassert>
#include <string>

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

struct State {
  State() : buttons(9 * 9) {}

  std::string dump() const {
    std::string str;
    visit([&](Position p, Button b) {
      if (p.x == 8 && p.y == 0) {
        str += "\n";
        return;
      }

      char bright[] = {'.', 'o', 'O', '0'};
      b.fix();
      str += bright[b.color.red];
      str += bright[b.color.green];
      str += ' ';
      if (p.x == 8)
        str += "\n";
    });
    return str;
  }

  Button& at(Position p) { return buttons.at(p.x + p.y * 9); }
  Button at(Position p) const { return buttons.at(p.x + p.y * 9); }
  void visit(std::function<void(Position p, Button b)> func) const {
    for (byte y = 0; y <= 8; y++)
      for (byte x = 0; x <= 8; x++) {
        Position p{x, y};
        p.fix();
        func({x, y}, at({x, y}));
      }
  }

 protected:
  std::vector<Button> buttons;
};

struct Midi {
  static byte velocity(Button b) {
    b.fix();
    return 0x10 * b.color.green + b.color.red + (b.clear << 3) + (b.copy << 2);
  }
  static byte key(Position p) {
    p.fix();
    assert(p.y != 0);
    if (p.y == 0) {
      return 8;
    }
    return p.x + (p.y - 1) * 0x10;
  }
  static uint32_t note_off(byte channel, byte key, byte velocity) {
    clamp(channel, 0, 3);
    return ((0x80 + channel) << 16) + (key << 8) + velocity;
  }
  static uint32_t note_on(byte channel, byte key, byte velocity) {
    clamp(channel, 0, 3);
    return ((0x90 + channel) << 16) + (key << 8) + velocity;
  }
  static uint32_t note_on(Position p, Button b) {
    p.fix();
    if (p.y == 0) {
      return 0xb06800 + (p.x << 8) + velocity(b);
    }
    return note_on(0, key(p), velocity(b));
  }
  static uint32_t note_off(Position p, Button b) {
    p.fix();
    if (p.y == 0) {
      return note_on(p, b) & 0xffffc0;
    }
    return note_off(0, key(p), velocity(b));
  }
};

}  // namespace launchpad