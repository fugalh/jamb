#include <gtest/gtest.h>
#include "ApprovalTests.hpp"
#include "FakeMidi.hh"

#include "../Aeolus.hh"

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
  aeolus.noteOn(69, 64);  // A440 at half velocity
  ApprovalTests::Approvals::verify(midi);
}