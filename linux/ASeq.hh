#pragma once

#include "../Midi.hh"
#include "../common.hh"

#include <alsa/asoundlib.h>

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
  snd_seq_t* seq_{};

  Transport(std::string name, std::string client);
  ~Transport() override;
  void send(Message) override;
};
}  // namespace midi::aseq