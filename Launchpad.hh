#pragma once
#include "Midi.hh"

struct Launchpad {
  struct Event {
    enum class Type {
      ButtonPress,
    };
    Type type;
    uint8_t button;
  };
  enum class Color { Off, Red, Green, Amber };
  enum class Intensity { Off = 0, Low = 1, Mid = 2, High = 3 };
  using Observer = std::function<void(Event)>;

  midi::Transport& midi_;
  Observer observer_;

  void init();
  void reset();
  void grid(uint8_t loc, Color, Intensity);

 protected:
  void dispatch(midi::Message const);
  void emit(Event ev) {
    if (observer_) {
      observer_(ev);
    }
  }
  uint8_t velocity(Color, Intensity);
};