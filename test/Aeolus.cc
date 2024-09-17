#include "../Aeolus.hh"
#include <gtest/gtest.h>
#include "ApprovalTests.hpp"
#include "FakeMidi.hh"

#include <string>
#include <vector>

TEST(Aeolus, generalCancel) {
  FakeMidi midi;
  auto aeolus = Aeolus{midi};
  aeolus.generalCancel();
  ApprovalTests::Approvals::verify(midi);
}
