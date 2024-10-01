#pragma once
#include <memory>
#include "Midi.hh"

#include <bitset>
#include <map>

namespace jamb {
struct State;
}

struct Aeolus {
  // cf MIDICTL_IFELM in aeolus/source/global.h
  static uint8_t constexpr kStopController = 98;

  struct State {
    using Groups = std::array<std::bitset<16>, 4>;
    Groups groups;
  };

  Aeolus(midi::Transport& t) : aeolus_{t} {}

  void jambStateUpdate(jamb::State const&);

  void generalCancel();
  void programChange(uint8_t program);
  void noteOn(uint8_t channel, uint8_t key, uint8_t velocity = 0x7f);
  void stopOn(uint8_t group, uint8_t element);
  void stopOff(uint8_t group, uint8_t element);
  void stopToggle(uint8_t group, uint8_t element);
  void allSoundOff();

 protected:
  midi::Transport& aeolus_;
  State state_{};
  void setStop(uint8_t group, uint8_t element, uint8_t mode);
};