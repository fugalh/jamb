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

TEST(Jamb, stopToggle) {
  FakeMidi lpMidi, aeolusMidi;
  auto launchpad = Launchpad{lpMidi};
  launchpad.init();
  auto aeolus = Aeolus{aeolusMidi};
  Jamb jamb{launchpad, aeolus};
  jamb.init();
  lpMidi.messages_.clear();
  aeolusMidi.messages_.clear();

  lpMidi.emit({0x90, {0x17, midi::kFullVelocity}});  // group 1 button 15
  lpMidi.emit({0x90, {0x42, midi::kFullVelocity}});  // group 2 button 2
  lpMidi.emit({0x90, {0x42, midi::kFullVelocity}});  // group 2 button 2
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

  lpMidi.emit({0x90, {0x68, midi::kFullVelocity}});
  ApprovalTests::Approvals::verifyAll({lpMidi, aeolusMidi});
}

auto setPreset(FakeMidi& m, uint8_t piston) {
  piston += 0x68;
  m.emit({0x90, {Launchpad::kSetButton, midi::kFullVelocity}});
  m.emit({0xB0, {piston, midi::kFullVelocity}});
  m.emit({0xB0, {piston, 0x00}});
  m.emit({0x90, {Launchpad::kSetButton, 0x00}});
}

TEST(Jamb, setCombo) {
  FakeMidi lpMidi, aeolusMidi;
  auto launchpad = Launchpad{lpMidi};
  launchpad.init();
  auto aeolus = Aeolus{aeolusMidi};
  Jamb jamb{launchpad, aeolus};
  jamb.init();
  lpMidi.messages_.clear();
  aeolusMidi.messages_.clear();

  lpMidi.emit({0x90, {0x01, midi::kFullVelocity}});
  setPreset(lpMidi, 0);
  lpMidi.emit({0x90, {0x12, midi::kFullVelocity}});
  setPreset(lpMidi, 1);
  lpMidi.emit({0x90, {0x15, midi::kFullVelocity}});
  setPreset(lpMidi, 2);

  lpMidi.emit({0x90, {0x78, 1}});                    // general cancel
  lpMidi.emit({0xB0, {0x69, midi::kFullVelocity}});  // recall combo

  ApprovalTests::Approvals::verifyAll({lpMidi, aeolusMidi});
}

TEST(Jamb, serializeMemory) {
  FakeMidi lpMidi, aeolusMidi;
  auto launchpad = Launchpad{lpMidi};
  launchpad.init();
  auto aeolus = Aeolus{aeolusMidi};
  Jamb jamb{launchpad, aeolus};
  jamb.init();

  lpMidi.emit({0x90, {0x01, midi::kFullVelocity}});
  setPreset(lpMidi, 0);
  lpMidi.emit({0x90, {0x78, 1}});  // general cancel
  lpMidi.emit({0x90, {0x73, midi::kFullVelocity}});
  setPreset(lpMidi, 3);

  ApprovalTests::Approvals::verify(jamb.memoryString());
}

TEST(Jamb, unserializeMemory) {
  FakeMidi lpMidi, aeolusMidi;
  auto launchpad = Launchpad{lpMidi};
  launchpad.init();
  lpMidi.clear();
  auto aeolus = Aeolus{aeolusMidi};
  Jamb jamb{launchpad, aeolus};
  jamb.init();

  std::string memory = R"(
memory:
  0:
    0:
      - ..............o.
      - ......oooo......
      - ................
      - ...o............
    3:
      - ................
      - ........ooo.....
      - ...........oo...
      - ....o...........

  )";
  jamb.memoryFromString(memory);
  lpMidi.emit({0xb0, {0x68, midi::kFullVelocity}});

  ApprovalTests::Approvals::verifyAll({lpMidi, aeolusMidi});
}