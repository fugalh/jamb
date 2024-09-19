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
  aeolus_.send({0xC0, {program}});
}

void Aeolus::noteOn(uint8_t key, uint8_t velocity) {
  aeolus_.send({0x90, {key, velocity}});
}

/*
Make a seq, and two ports (one duplex for launchpad and one simplex output for
aeolus)

snd_seq_open
snd_seq_set_client_name
snd_seq_create_simple_port

*/