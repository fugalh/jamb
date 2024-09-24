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

TEST(Jamb, recallCombo) {
  FakeMidi lpMidi, aeolusMidi;
  auto launchpad = Launchpad{lpMidi};
  launchpad.init();
  auto aeolus = Aeolus{aeolusMidi};
  Jamb jamb{launchpad, aeolus};
  jamb.init();
  lpMidi.messages_.clear();
  aeolusMidi.messages_.clear();

  lpMidi.emit({0xB0, {0x69, 0x7f}});
  ApprovalTests::Approvals::verifyAll({lpMidi, aeolusMidi});
}

TEST(Jamb, stopToggle) {
  FakeMidi lpMidi, aeolusMidi;
  auto launchpad = Launchpad{lpMidi};
  launchpad.init();
  auto aeolus = Aeolus{aeolusMidi};
  Jamb jamb{launchpad, aeolus};
  jamb.init();
  lpMidi.messages_.clear();
  aeolusMidi.messages_.clear();

  lpMidi.emit({0x90, {0x17, 0x7f}});  // group 1 button 15
  lpMidi.emit({0x90, {0x42, 0x7f}});  // group 2 button 2
  lpMidi.emit({0x90, {0x42, 0x7f}});  // group 2 button 2
  ApprovalTests::Approvals::verifyAll({lpMidi, aeolusMidi});
}

TEST(Jamb, midiPanic) {
  FakeMidi lpMidi, aeolusMidi;
  auto launchpad = Launchpad{lpMidi};
  launchpad.init();
  auto aeolus = Aeolus{aeolusMidi};
  Jamb jamb{launchpad, aeolus};
  jamb.init();
  lpMidi.messages_.clear();
  aeolusMidi.messages_.clear();

  lpMidi.emit({0x90, {0x68, 0x7f}});
  ApprovalTests::Approvals::verifyAll({lpMidi, aeolusMidi});
}