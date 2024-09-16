#include <gtest/gtest.h>
#include "ApprovalTests.hpp"

#include <vector>
#include <string>

TEST(midi, generalCancel)
{
  // generate midi messages
  // channel 0
  // value 01mm0ggg with mm=00 and ggg=[000, 001, 002, 003]
  auto messages = std::string(
    "ch=0 v=0b01000000\n"
    "ch=0 v=0b01000001\n"
    "ch=0 v=0b01000002\n"
    "ch=0 v=0b01000003\n"
  );
  ApprovalTests::Approvals::verify(messages);
}
