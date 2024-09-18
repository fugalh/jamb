#pragma once
#include <fmt/format.h>
#include <sstream>

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

  template <class STREAM>
  friend STREAM& operator<<(STREAM& os, const FakeMidi& x) {
    for (auto const& msg : x.messages_) {
      os << fmt::format("0x{:X} {:X} {:X}", msg.status, msg.data[0],
                        msg.data[1]);
      switch ((msg.status & 0xf0) >> 4) {
        case 0xB:
          if (msg.data[0] == StopController) {
            if (msg.data[1] & 0b0100'0000) {
              std::string const modes[4] = {"cancel", "off", "on", "toggle"};
              auto mm = (msg.data[1] & 0b110000) >> 4;
              auto ggg = msg.data[1] & 0b111;
              os << fmt::format(": group {} {}", ggg, modes[mm]);
            } else {
              os << fmt::format(": stop {}", msg.data[1] & 0b00011111);
            }
          }
          break;

        case 0xC:
          if (msg.data[1] == 0) {
            os << fmt::format(": program change {}", msg.data[0]);
          }
          break;

        default:
          break;
      }
      os << "\n";
    }
    return os;
  }
};

static auto format_as(FakeMidi const& x) {
  std::ostringstream os;
  os << x;
  return os.str();
}
