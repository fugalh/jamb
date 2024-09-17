#include "Aeolus.hh"
#include "Midi.hh"

void Aeolus::generalCancel() {
  unsigned const channel = 0;
  for (unsigned group = 0; group < 4; group++) {
    unsigned const param = StopController;  // MIDICTL_IFELM in Aeolus
    unsigned const event = 0xB;             // Control Change

    // 01mm0ggg with mm=00 indicating cancel
    uint8_t value = 0x40 | group;
    aeolus_.send(midi::Message{channel, event, {param, value}});
  }
}

void Aeolus::programChange(uint8_t program) {
  aeolus_.send({0, 0xC, {program, 0}});
}