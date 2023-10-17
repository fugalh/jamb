#include <functional>
#include <iostream>
#include <tuple>

#include "launchpad.hh"

namespace launchpad {
using Message = Position;
using Dispatch = std::function<void(Message const&)>;
using Effect = std::function<void(Dispatch)>;

struct Program {
  State init;
  std::function<std::tuple<State, Effect>(State, Message)> update;
  std::function<void(State)> view;
};

struct Runtime {
  Program program;
  State state{program.init};
  void dispatch(Message const& msg) {
    auto [s, effect] = program.update(state, msg);
    state = std::move(s);
    if (effect) {
      effect([this](Message const& msg) { dispatch(msg); });
    }
    program.view(state);
  }
};

static Program const jambProgram = {
    .init = State{},
    .update =
        [](State s1, Message msg) {
          State s2 = s1;
          s2.at(msg).color = {3};
          return std::tuple{s2, Effect{}};
        },
    .view = [](State s) { std::cout << s.dump() << std::endl; }};

}  // namespace launchpad