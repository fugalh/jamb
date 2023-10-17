#include <functional>
#include <iostream>
#include <tuple>

#include "launchpad.hh"

namespace launchpad {
using std::function;
using std::tuple;

struct ButtonPress {
  Position pos;
};
using Message = std::variant<ButtonPress>;

using Dispatch = function<void(Message const&)>;
using Effect = function<void(Dispatch)>;

struct Program {
  State init;
  function<tuple<State, Effect>(State, Message const&)> update;
  function<void(State)> view;
};

struct Runtime {
  Program program;
  State state{program.init};
  void dispatch(Message const& msg) {
    if (!program.update)
      return;
    auto [s, effect] = program.update(state, msg);
    state = std::move(s);
    if (effect)
      effect([this](Message const& msg) { dispatch(msg); });
    if (program.view)
      program.view(state);
  }
};

static Program const jambProgram = {
    .init = State{},
    .update =
        [](State s1, Message const& msg) {
          State s2 = s1;
          Position pos = std::get<ButtonPress>(msg).pos;
          s2.at(pos).color = {3};
          return tuple{s2, Effect{}};
        },
    .view = [](State s) { std::cout << s.dump() << std::endl; }};

}  // namespace launchpad