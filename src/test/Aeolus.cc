#include <gtest/gtest.h>
#include "ApprovalTests.hpp"
#include "FakeMidi.hh"

#include "../Aeolus.hh"
#include "../Jamb.hh"

TEST(Aeolus, generalCancel) {
  FakeMidi midi;
  auto aeolus = Aeolus{midi};
  aeolus.generalCancel();
  ApprovalTests::Approvals::verify(midi);
}

TEST(Aeolus, programChange) {
  FakeMidi midi;
  auto aeolus = Aeolus{midi};
  aeolus.programChange(42);
  ApprovalTests::Approvals::verify(midi);
}

TEST(Aeolus, noteOn) {
  FakeMidi midi;
  auto aeolus = Aeolus{midi};
  aeolus.noteOn(1, 69, 64);  // A440 at half velocity on channel 1
  ApprovalTests::Approvals::verify(midi);
}

TEST(Aeolus, stops) {
  FakeMidi midi;
  auto aeolus = Aeolus{midi};
  aeolus.stopOn(1, 3);
  aeolus.stopOff(2, 2);
  aeolus.stopToggle(3, 1);
  ApprovalTests::Approvals::verify(midi);
}

TEST(Aeolus, jambStateUpdate) {
  FakeMidi midi;
  auto aeolus = Aeolus{midi};
  jamb::State mState{};
  mState.groups[1][2] = true;
  mState.groups[3][9] = true;
  aeolus.generalCancel();
  aeolus.jambStateUpdate(mState);
  ApprovalTests::Approvals::verify(midi);
}