#include <functional>
#include <iostream>
#include <tuple>

#include "launchpad.hh"

namespace launchpad {
using Message = Position;
using Dispatch = std::function<void(Message const&)>;
using Effect = std::function<void(Dispatch)>;

struct Program {
  State initialState() { return State{}; }
  virtual ~Program() = default;
  virtual std::tuple<State, Effect> update(State const&, Message const&) = 0;
  virtual void view(State const&) = 0;
};

struct Runtime {
  Program& program;
  State state{program.initialState()};
  void dispatch(Message const& msg) {
    auto [s, effect] = program.update(state, msg);
    state = std::move(s);
    if (effect) {
      effect([this](Message const& msg) { dispatch(msg); });
    }
    program.view(state);
  }
};

struct JambProgram : public Program {
  std::tuple<State, Effect> update(State const& s1, Message const& msg) {
    State s2 = s1;
    s2.at(msg).color = {3};
    return {s2, {}};
  }
  void view(State const& s) { std::cout << s.dump() << std::endl; }
};

}  // namespace launchpad