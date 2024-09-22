
#include <gtest/gtest.h>
#include "ApprovalTests.hpp"
#include "FakeMidi.hh"

#include "../Jamb.hh"
#include "../Launchpad.hh"

TEST(Launchpad, init) {
  FakeMidi midi;
  auto lp = Launchpad{midi};
  lp.init();
  ApprovalTests::Approvals::verify(midi);
}

TEST(Launchpad, observerCalledBack) {
  FakeMidi lpMidi;
  bool visited = false;
  Launchpad lp{lpMidi, [&](Command ev) {
                 EXPECT_EQ(ev.type, Command::Type::GeneralCancel);
                 visited = true;
               }};
  lp.init();

  uint8_t const velocity = 1;
  lpMidi.emit({0x90, {0x78, velocity}});
  EXPECT_TRUE(visited);
}

TEST(Launchpad, buttons) {
  FakeMidi midi;
  auto lp = Launchpad{midi};
  lp.init();
  lp.grid(0x42, Launchpad::Color::Amber, Launchpad::Intensity::Mid);
  lp.grid(0x77, Launchpad::Color::Green, Launchpad::Intensity::High);
  lp.topRow(0x05, {Launchpad::Color::Red, Launchpad::Intensity::Low});
  lp.grid(0x00, Launchpad::Color::Red, Launchpad::Intensity::Off);
  lp.grid(0x01, Launchpad::Color::Off, Launchpad::Intensity::High);
  ApprovalTests::Approvals::verify(midi);
}

TEST(Launchpad, jambStateUpdate) {
  FakeMidi midi;
  auto lp = Launchpad{midi};
  auto jState = jamb::State{};
  lp.init();
  midi.clear();

  jState.groups[1][0] = true;
  jState.groups[1][1] = true;
  jState.activePreset = 3;
  lp.jambStateUpdate(jState);
  jState.groups[1][1] = false;
  jState.activePreset = std::nullopt;
  lp.jambStateUpdate(jState);

  ApprovalTests::Approvals::verify(midi);
}
