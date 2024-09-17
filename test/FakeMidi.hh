#pragma once
#include <format>
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

  friend std::ostream& operator<<(std::ostream& os, const FakeMidi& x) {
    for (auto const& msg : x.messages_) {
      os << std::format("0x{:X} {:X} {:X}", msg.status, msg.data[0],
                        msg.data[1]);
      switch ((msg.status & 0xf0) >> 4) {
        case 0xB:
          if (msg.data[0] == StopController) {
            if (msg.data[1] & 0b0100'0000) {
              std::string const modes[4] = {"cancel", "off", "on", "toggle"};
              auto mm = (msg.data[1] & 0b110000) >> 4;
              auto ggg = msg.data[1] & 0b111;
              os << std::format(": group {} {}", ggg, modes[mm]);
            } else {
              os << std::format(": stop {}", msg.data[1] & 0b00011111);
            }
          }
          break;

        case 0xC:
          if (msg.data[1] == 0) {
            os << std::format(": program change {}", msg.data[0]);
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