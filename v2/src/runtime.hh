#include <functional>
#include <tuple>

#include "launchpad.hh"

namespace launchpad {
using Message = int;
using Dispatch = std::function<void(Message const&)>;
using Effect = std::function<void(Dispatch)>;

struct Program {
  State initialState() { return State{}; }
  std::tuple<State, Effect> update(State const&, Message const&);
  void view(State const&);
};

struct Runtime {
  Program program;
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
}  // namespace launchpad