#include "launchpad.hh"
#include <gtest/gtest.h>

using namespace launchpad;

TEST(Color, syntax) {
  Color off{};
  EXPECT_EQ(off.red, 0);
  EXPECT_EQ(off.green, 0);
}

TEST(Button, syntax) {
  Button b{{1, 2}};
  EXPECT_EQ(b.color.red, 1);
  EXPECT_EQ(b.color.green, 2);
  EXPECT_TRUE(b.clear);
  EXPECT_TRUE(b.copy);
}

TEST(Midi, button) {
  EXPECT_EQ(Midi::velocity(Button{}), 0x0c);
  EXPECT_EQ(Midi::velocity(Button{{1, 2}}), 0x10 * 2 + 1 + 0b1100);
  EXPECT_EQ(Midi::velocity(Button{{2, 0}, true, false}), 0x10 * 0 + 2 + 0b1000);
}

TEST(Midi, fix) {
  EXPECT_EQ(Midi::velocity(Button{{1, 17}}), 0x10 * 3 + 1 + 0x0c);
}
TEST(Midi, note) {
  EXPECT_EQ(Midi::note_on(Position{2, 5}, Button{{1, 0}}), 0x90420d);
  EXPECT_EQ(Midi::note_off(Position{2, 5}, Button{{1, 0}}) >> 8, 0x8042);
}

TEST(Midi, note_first_row) {
  EXPECT_EQ(Midi::note_on(Position{5, 0}, Button{}), 0xb06d0c);
  EXPECT_EQ(Midi::note_off(Position{5, 0}, Button{}), 0xb06d00);
}

TEST(Midi, note_dead) {
  EXPECT_EQ(Midi::note_on(Position{8, 0}, Button{}), 0x90080c);
  EXPECT_EQ(Midi::note_off(Position{8, 0}, Button{}) >> 8, 0x8008);
}

TEST(State, dump) {
  State s;
  s.at({3, 5}).color = {2, 3};
  s.at({7, 7}).color = {1, 3};
  EXPECT_EQ(s.dump(),
            ".. .. .. .. .. .. .. .. \n"
            ".. .. .. .. .. .. .. .. .. \n"
            ".. .. .. .. .. .. .. .. .. \n"
            ".. .. .. .. .. .. .. .. .. \n"
            ".. .. .. .. .. .. .. .. .. \n"
            ".. .. .. O0 .. .. .. .. .. \n"
            ".. .. .. .. .. .. .. .. .. \n"
            ".. .. .. .. .. .. .. o0 .. \n"
            ".. .. .. .. .. .. .. .. .. \n");
}

#include "runtime.hh"
TEST(Runtime, pullStop) {
  JambProgram p;
  Runtime r{p};
  r.dispatch({3, 5});
  r.dispatch({7, 7});
}