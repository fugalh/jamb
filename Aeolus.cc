#include "Aeolus.hh"
#include "Jamb.hh"
#include "Midi.hh"

void Aeolus::jambStateUpdate(jamb::State const& s2) {
  for (auto g = 0; g < state_.groups.size(); g++) {
    auto const& group = state_.groups[g];
    for (auto s = 0; s < group.size(); s++) {
      bool stop = group[s];
      bool stop2 = s2.groups[g][s];
      if (stop != stop2) {
        if (stop2) {
          stopOn(g, s);
        } else {
          stopOff(g, s);
        }
      }
    }
  }
  state_.groups = s2.groups;
}

void Aeolus::generalCancel() {
  for (unsigned group = 0; group < 4; group++) {
    unsigned const param = StopController;  // MIDICTL_IFELM in Aeolus
    unsigned const status = 0xB0;           // Control Change

    // 01mm0ggg with mm=00 indicating cancel
    uint8_t value = 0x40 | group;
    aeolus_.send(midi::Message{status, {param, value}});
  }
}

void Aeolus::programChange(uint8_t program) {
  aeolus_.send({0xC0, {program}});
}

void Aeolus::noteOn(uint8_t channel, uint8_t key, uint8_t velocity) {
  uint8_t status = 0x90 | (channel & 0xf);
  aeolus_.send({status, {key, velocity}});
}

void Aeolus::setStop(uint8_t ggg, uint8_t element, uint8_t mm) {
  uint8_t val = 0b0100'0000 | ((mm & 0b0011) << 4) | (ggg & 0b0111);
  aeolus_.send({0xB0, {0x62, val}});
  val = element & 0b0001'1111;
  aeolus_.send({0xB0, {0x62, val}});
}

void Aeolus::stopOn(uint8_t group, uint8_t element) {
  setStop(group, element, 0b10);
}

void Aeolus::stopOff(uint8_t group, uint8_t element) {
  setStop(group, element, 0b01);
}

void Aeolus::stopToggle(uint8_t group, uint8_t element) {
  setStop(group, element, 0b11);
}

void Aeolus::allSoundOff() {
  aeolus_.send({0xb0, {0x78, 1}});
}

/*
Make a seq, and two ports (one duplex for launchpad and one simplex output for
aeolus)

snd_seq_open
snd_seq_set_client_name
snd_seq_create_simple_port

*/