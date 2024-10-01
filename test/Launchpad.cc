
#include <gtest/gtest.h>
#include "ApprovalTests.hpp"
#include "FakeMidi.hh"

#include "../Jamb.hh"
#include "../Launchpad.hh"
#include "Launchpad-defaultConfig.hh"

static std::string dumpStopmap(launchpad::Stopmap& stopmap) {
  std::stringstream ss;
  using std::hex;
  for (uint8_t row = 0; row < 8; row++) {
    for (uint8_t col = 0; col < 8; col++) {
      auto x = stopmap[{row, col}];
      if (x) {
        ss << hex << int(x->group) << hex << int(x->button);
      } else {
        ss << "  ";
      }
      ss << " ";
    }
    ss << "\n";
  }
  return ss.str();
}

TEST(Launchpad, init) {
  FakeMidi midi;
  auto lp = Launchpad{midi};
  ApprovalTests::Approvals::verify(midi);
}

TEST(Launchpad, observerCalledBack) {
  FakeMidi lpMidi;
  bool visited = false;
  Launchpad lp{lpMidi, [&](Command ev) {
                 EXPECT_EQ(ev.type, Command::Type::GeneralCancel);
                 visited = true;
               }};

  uint8_t const velocity = 1;
  lpMidi.emit({0x90, {0x78, velocity}});
  EXPECT_TRUE(visited);
}

TEST(Launchpad, buttons) {
  FakeMidi midi;
  auto lp = Launchpad{midi};
  lp.grid(4, 2, {Launchpad::Color::Amber, Launchpad::Intensity::Mid});
  lp.grid(7, 7, {Launchpad::Color::Green, Launchpad::Intensity::High});
  lp.topRow(5, {Launchpad::Color::Red, Launchpad::Intensity::Low});
  lp.grid(0, 0, {Launchpad::Color::Red, Launchpad::Intensity::Off});
  lp.grid(0, 1, {Launchpad::Color::Off, Launchpad::Intensity::High});
  ApprovalTests::Approvals::verify(midi);
}

TEST(Launchpad, resetTopRow) {
  FakeMidi midi;
  auto lp = Launchpad{midi};
  midi.clear();

  lp.resetTopRow();  // nothing

  lp.topRow(2, {Launchpad::Color::Amber, Launchpad::Intensity::Mid});
  lp.resetTopRow();  // turn off just 2

  ApprovalTests::Approvals::verify(midi);
}

TEST(Launchpad, jambStateUpdate) {
  FakeMidi midi;
  auto lp = Launchpad{midi};
  auto jState = jamb::State{};
  midi.clear();

  jState.groups[2][9] = true;
  jState.groups[1][1] = true;
  jState.activeCombination = {1, 3};
  lp.jambStateUpdate(jState);
  jState.groups[1][1] = false;
  jState.activeCombination = std::nullopt;
  lp.jambStateUpdate(jState);

  ApprovalTests::Approvals::verify(midi);
}

TEST(Launchpad, emitSetCombo) {
  FakeMidi midi;
  Command cmd{};
  auto lp = Launchpad{midi, [&](Command ev) { cmd = ev; }};
  auto jState = jamb::State{};
  midi.clear();

  midi.emit({0x90, {Launchpad::kSetButton, midi::kFullVelocity}});
  midi.emit({midi::kController, {0x69, midi::kFullVelocity}});
  midi.emit({0x90, {Launchpad::kSetButton, 0x00}});

  EXPECT_EQ(cmd.type, Command::Type::SetCombination);
  EXPECT_EQ(cmd.u.combo.memory, 0);
  EXPECT_EQ(cmd.u.combo.piston, 1);
}

TEST(Launchpad, setButtonLightedWhilePressed) {
  FakeMidi midi;
  auto lp = Launchpad{midi};
  midi.clear();

  midi.emit({0x90, {Launchpad::kSetButton, midi::kFullVelocity}});
  midi.emit({0x90, {Launchpad::kSetButton, 0}});

  ApprovalTests::Approvals::verify(midi);
}

TEST(Launchpad, parseStopmap) {
  auto stopmap = launchpad::parseStopmapFromConfig(launchpad::kDefaultConfig);

  ApprovalTests::Approvals::verify(dumpStopmap(stopmap));
}

TEST(Launchpad, badStopmap) {
  auto stopmap = launchpad::parseStopmapFromConfig("nonsense");
  ApprovalTests::Approvals::verify(dumpStopmap(stopmap));
}

TEST(Launchpad, invalidInstrumentForStopmap) {
  auto stopmap = launchpad::parseStopmapFromConfig(launchpad::kDefaultConfig,
                                                   "bad instrument");
  ApprovalTests::Approvals::verify(dumpStopmap(stopmap));
}