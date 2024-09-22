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

    bool operator==(Button const& other) const {
      return color == other.color && intensity == other.intensity;
    }
  };
  struct State {
    std::array<std::array<Button, 9>, 8> grid;
    std::array<Button, 8> topRow;
  };
  using Observer = std::function<void(Command)>;

  midi::Transport& midi_;
  Observer observer_;

  Launchpad(midi::Transport& midi) : midi_{midi} {}
  Launchpad(midi::Transport& midi, Observer o) : midi_{midi}, observer_{o} {}

  void init();
  void reset();

  void jambStateUpdate(jamb::State const&);

  // testing only
  void grid(uint8_t row, uint8_t col, Button button);
  void topRow(uint8_t loc, Button);
  void resetTopRow();

 protected:
  State state_{};

  void dispatch(midi::Message const);
  void emit(Command cmd) {
    if (observer_) {
      observer_(cmd);
    }
  }
  Command::Stop gridToStop(uint8_t button);
  uint8_t velocity(Color, Intensity);
  void render(State const&);
  void grid_(uint8_t loc, Button button);
};
