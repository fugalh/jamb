#pragma once

#include "../Midi.hh"
#include "../common.hh"

#include <alsa/asoundlib.h>

#include <memory>
#include <string>

#define ASEQ_CHECK(cmd)          \
  {                              \
    auto _err = (cmd);           \
    if (_err < 0) {              \
      FATAL(snd_strerror(_err)); \
    }                            \
  }

namespace midi::aseq {
struct Transport : public midi::Transport {
  snd_seq_t* const& seq_;
  short port_{-1};  // port is unsigned char, short is big enough to hold
  // unsigned char and still be negative.
  std::string name_;

  Transport(snd_seq_t* const&, std::string client);
  ~Transport() override;
  void send(Message) override;
  void readLoop();

 protected:
  snd_seq_event_t eventPrototype();
};

struct Sequencer {
  snd_seq_t* seq_{};
  std::unique_ptr<Transport> launchpad_;
  std::unique_ptr<Transport> aeolus_;

  Sequencer(std::string name);
  ~Sequencer();
};

}  // namespace midi::aseq