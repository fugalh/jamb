#include <gtest/gtest.h>
#include "ApprovalTests.hpp"

#include "../Command.hh"

#include <vector>

TEST(Command, toString) {
  std::vector<Command> cmds;
  Command cmd;

  cmd = {Command::Type::GeneralCancel};
  cmds.push_back(cmd);

  cmd = {Command::Type::RecallPreset};
  cmd.u.preset = 7;
  cmds.push_back(cmd);

  cmd = {Command::Type::StopToggle};
  cmd.u.stop = {1, 3};
  cmds.push_back(cmd);

  cmd = {Command::Type::MidiPanic};
  cmds.push_back(cmd);

  ApprovalTests::Approvals::verifyAll(cmds);
}