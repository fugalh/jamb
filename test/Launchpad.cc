
#include <gtest/gtest.h>
#include "ApprovalTests.hpp"

#include "../Launchpad.hh"
#include "FakeMidi.hh"

TEST(Launchpad, init) {
  FakeMidi midi;
  auto lp = Launchpad(midi);
  ApprovalTests::Approvals::verify(midi);
}