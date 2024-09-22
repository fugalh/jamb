#pragma once
#include "Command.hh"
#include "Midi.hh"

namespace jamb {
struct State;
}
struct Launchpad {
  enum class Color { Off, Red, Green, Amber };
  enum class Intensity { Off = 0, Low = 1, Mid = 2, High = 3 };
  struct Button {
    Color color;
    Intensity intensity;
  };
  struct State {
    std::array<std::array<Button, 9>, 8> grid;
    std::array<Button, 8> topRow;
  };
  using Observer = std::function<void(Command)>;

  midi::Transport& midi_;
  Observer observer_;

  void init();
  void reset();
  void grid(uint8_t loc, Color, Intensity);
  void resetTopRow();

  void jambStateUpdate(jamb::State const&);

 protected:
  void dispatch(midi::Message const);
  void emit(Command cmd) {
    if (observer_) {
      observer_(cmd);
    }
  }
  Command::Stop gridToStop(uint8_t button);
  uint8_t velocity(Color, Intensity);
  void render();
};