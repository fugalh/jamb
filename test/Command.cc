#include <gtest/gtest.h>
#include "ApprovalTests.hpp"

#include "../Command.hh"

#include <vector>

TEST(Command, toString) {
  std::vector<Command> cmds = {{Command::Type::GeneralCancel}};
  ApprovalTests::Approvals::verifyAll(cmds);
}