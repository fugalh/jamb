#include <gtest/gtest.h>
#include "ApprovalTests.hpp"

#include "../Command.hh"

#include <vector>

TEST(Command, toString) {
  std::vector<Command> cmds = {{Command::Type::GeneralCancel},
                               {Command::Type::StopToggle, .u.stop = {1, 3}}};
  ApprovalTests::Approvals::verifyAll(cmds);
}