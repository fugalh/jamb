#include "Aeolus.hh"
#include "Midi.hh"

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
  aeolus_.send({0xC0, {program, 0}});
}