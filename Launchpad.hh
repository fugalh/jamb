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
  using Observer = std::function<void(Event)>;

  midi::Transport& midi_;
  Observer observer_;

  Launchpad(midi::Transport&, Observer = nullptr);

 protected:
  void dispatch(midi::Message const);
  void emit(Event ev) {
    if (observer_) {
      observer_(ev);
    }
  }
};