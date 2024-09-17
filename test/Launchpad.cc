
#include <gtest/gtest.h>
#include "ApprovalTests.hpp"

#include "../Launchpad.hh"
#include "FakeMidi.hh"

TEST(Launchpad, init) {
  FakeMidi midi;
  auto lp = Launchpad(midi);
  ApprovalTests::Approvals::verify(midi);
}

TEST(Launchpad, observerCalledBack) {
  FakeMidi lpMidi;
  bool visited = false;
  Launchpad lp(lpMidi, [&](Launchpad::Event ev) {
    EXPECT_EQ(ev.button, 0x78);
    visited = true;
  });

  uint8_t const velocity = 1;
  lpMidi.emit({0x90, {0x78, velocity}});
  EXPECT_TRUE(visited);
}