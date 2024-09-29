#pragma once
#include "Command.hh"
#include "Launchpad-defaultConfig.hh"
#include "Midi.hh"

#include <map>

namespace jamb {
struct State;
}  // namespace jamb

namespace launchpad {

using Stopmap = std::map<std::pair<int, int>, std::optional<Command::Stop>>;
Stopmap parseStopmapFromConfig(std::string config,
                               std::string instrument = "Aeolus");
}  // namespace launchpad

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
    bool pressingSet{false};
  };
  using Observer = std::function<void(Command)>;
  static uint8_t constexpr kSetButton = 0x58;

  midi::Transport& midi_;
  Observer observer_;

  Launchpad(midi::Transport& midi) : midi_{midi} {
    configureStopmap(launchpad::kDefaultConfig);
  }
  Launchpad(midi::Transport& midi, Observer o) : midi_{midi}, observer_{o} {}

  void init();
  void reset();

  void configureStopmap(std::string config);
  void jambStateUpdate(jamb::State const&);

  // public for testing only
  void grid(uint8_t row, uint8_t col, Button button);
  void topRow(uint8_t loc, Button);
  void resetTopRow();

 protected:
  State state_{};
  launchpad::Stopmap stopmap_;

  void dispatch(midi::Message const);
  void emit(Command cmd) {
    if (observer_) {
      observer_(cmd);
    }
  }
  std::optional<Command::Stop> gridToStop(uint8_t button);
  uint8_t velocity(Color, Intensity);
  void render(State const&);
  void grid_(uint8_t loc, Button button);
};
