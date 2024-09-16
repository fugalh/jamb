#include <gtest/gtest.h>
#include "../Aeolus.hh"
#include "ApprovalTests.hpp"
#include "FakeMidi.hh"

#include <string>
#include <vector>

TEST(midi, generalCancel) {
  FakeMidi midi;
  auto aeolus = Aeolus{midi};
  aeolus.generalCancel();
  ApprovalTests::Approvals::verify(midi);
}
