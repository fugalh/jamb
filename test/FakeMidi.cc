#include "FakeMidi.hh"
#include "../Launchpad.hh"
#include "../common.hh"

#include <fmt/format.h>
#include <sstream>

std::string FakeMidi::toString() const {
  std::ostringstream os;
  for (auto const& msg : messages_) {
    auto const v = msg.data[1];
    os << fmt::format("0x{:02x} {:02x} {:02x}", msg.status, msg.data[0], v);
    auto const color = v & 0x33;
    switch (msg.status & 0xf0) {
      case 0x90: {
        auto const k = msg.data[0];
        auto const v = msg.data[1];
        if (k <= 0x80 && (k & 0x0f) < 8) {
          os << fmt::format(": grid {} {} {:02x}", topFour(k), k & 0x0f, color);
        }
        if (k == Launchpad::kSetButton) {
          os << fmt::format(": set {:02x}", color);
        }
        if (k == 0x68) {
          os << fmt::format(": midi panic {:02x}", color);
        }
        if (k == 0x78) {
          os << fmt::format(": general cancel {:02x}", color);
        }
        break;
      }
      case midi::kController: {
        auto const c = msg.data[0];
        if (c == StopController) {
          if (v & 0b0100'0000) {
            std::string const modes[4] = {"cancel", "off", "on", "toggle"};
            auto mm = (v & 0b110'000) >> 4;
            auto ggg = v & 0b111;
            os << fmt::format(": group {} {}", ggg, modes[mm]);
          } else {
            os << fmt::format(": stop {}", v & 0b00011111);
          }
        }
        if (c >= 0x68 && c <= 0x6f) {
          os << fmt::format(": piston {} {:02x}", c - 0x68, color);
        }
        break;
      }

      case 0xC0:
        if (v == 0) {
          os << fmt::format(": program change {}", msg.data[0]);
        }
        break;

      default:
        break;
    }
    os << "\n";
  }
  return os.str();
}