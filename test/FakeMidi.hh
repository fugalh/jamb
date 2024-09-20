#pragma once

#include "../Aeolus.hh"
#include "../Midi.hh"

struct FakeMidi : public midi::Transport {
  std::vector<midi::Message> messages_;

  void send(midi::Message msg) override { messages_.push_back(msg); }
  void emit(midi::Message msg) {
    if (observer) {
      observer(msg);
    }
  }

  std::string toString() const;

  template <class STREAM>
  friend STREAM& operator<<(STREAM& os, const FakeMidi& x) {
    os << x.toString();
    return os;
  }
};

static auto format_as(FakeMidi const& x) {
  return x.toString();
}
