#pragma once
#include <cstdint>
#include <functional>

namespace midi {
struct Message {
  uint8_t channel;
  uint8_t status;
  uint8_t data[2];
};

struct Transport {
  virtual void send(Message) = 0;
  std::function<void(Message)> observer{};
};

}  // namespace midi