#include <gtest/gtest.h>
#include "ApprovalTests.hpp"
#include "FakeMidi.hh"

#include "../Jamb.hh"

TEST(Jamb, generalCancel) {
  FakeMidi lpMidi, aeolusMidi;
  auto launchpad = Launchpad{lpMidi};
  launchpad.init();
  auto aeolus = Aeolus{aeolusMidi};
  Jamb jamb{launchpad, aeolus};
  jamb.init();

  lpMidi.emit({0x90, {0x78, 1}});
  ApprovalTests::Approvals::verify(aeolusMidi);
}

TEST(Jamb, recallPreset) {
  FakeMidi lpMidi, aeolusMidi;
  auto launchpad = Launchpad{lpMidi};
  launchpad.init();
  auto aeolus = Aeolus{aeolusMidi};
  Jamb jamb{launchpad, aeolus};
  jamb.init();

  lpMidi.emit({0xB0, {0x69, 0x7f}});
  ApprovalTests::Approvals::verifyAll({lpMidi, aeolusMidi});
}