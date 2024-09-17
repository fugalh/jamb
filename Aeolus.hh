#pragma once
#include <memory>
#include "Midi.hh"

uint8_t constexpr StopController = 98;

struct Aeolus {
  midi::Transport& aeolus_;
  void generalCancel();
  void programChange(uint8_t program);
};