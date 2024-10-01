#pragma once
#include <cstdint>
#include <functional>

namespace midi {
uint8_t constexpr kNoteOn = 0x90;
uint8_t constexpr kController = 0xb0;
uint8_t constexpr kFullVelocity = 0x7f;

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