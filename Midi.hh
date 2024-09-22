#pragma once
#include <cstdint>
#include <functional>

namespace midi {
struct Message {
  uint8_t status;
  uint8_t data[2];
};

struct Transport {
  std::function<void(Message)> observer{};

  virtual ~Transport() = default;
  virtual void send(Message) = 0;
};

}  // namespace midi