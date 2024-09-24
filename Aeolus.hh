#pragma once
#include <memory>
#include "Midi.hh"

#include <bitset>

uint8_t constexpr StopController = 98;
namespace jamb {
struct State;
}

struct Aeolus {
  struct State {
    using Groups = std::array<std::bitset<16>, 4>;
    Groups groups;
  };
  midi::Transport& aeolus_;
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
  State state_{};
  void setStop(uint8_t group, uint8_t element, uint8_t mode);
};