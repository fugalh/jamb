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

/// This class interfaces with the Launchpad over MIDI. jamb::Model sets itself
/// as observer to recieve commands in response to button pressses, and it calls
/// jambUpdateState() with any state updates, and then we send MIDI messages to
/// update the Launchpad display.
///
/// cf `doc/launchpad programmer reference.pdf`
struct Launchpad {
  enum class Color { Off, Red, Green, Amber };
  enum class Intensity { Off = 0, Low = 1, Mid = 2, High = 3 };
  static uint8_t constexpr kSetButton = 0x58;

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

  Launchpad(midi::Transport& midi, Observer o = {})
      : observer_{o}, midi_(midi) {
    init();
  }

  void configureStopmap(std::string config);
  void jambStateUpdate(jamb::State const&);

 public:  // for testing only
  Observer observer_;

  // Make this grid location look like that. 0 indexed.
  void grid(uint8_t row, uint8_t col, Button button);
  // Make this top row button ("piston") look like that.
  void topRow(uint8_t loc, Button);
  // Clear all top row buttons
  void resetTopRow();

 protected:
  midi::Transport& midi_;
  State state_{};
  launchpad::Stopmap stopmap_;
  std::map<Command::Stop, std::pair<int, int>> gridmap_;

  void init();
  void dispatch(midi::Message const);
  void emit(Command cmd) {
    if (observer_) {
      observer_(cmd);
    }
  }
  // Translate a grid location to a Stop, honoring the stopmap
  std::optional<Command::Stop> gridToStop(uint8_t button);
  // Translate a stop back to a grid address, honoring the stopmap
  std::optional<std::pair<int, int>> stopToGrid(Command::Stop);
  // Send required MIDI updates so that the Launchpad looks like the new state,
  // and update the state.
  void render(State const& newState);

  // send the MIDI to set this grid button to that. 9th row "wraps around" as
  // the top row
  void setGridButton(uint8_t loc, Button button);
};
