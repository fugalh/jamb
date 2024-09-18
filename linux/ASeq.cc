#include "ASeq.hh"

namespace midi::aseq {

Transport::Transport(std::string name, std::string client) {
  ASEQ_CHECK(snd_seq_open(&seq_, "default", SND_SEQ_OPEN_DUPLEX, 0));
  ASEQ_CHECK(snd_seq_set_client_name(seq_, name.c_str()));
}

Transport::~Transport() {
  if (seq_) {
    snd_seq_close(seq_);
  }
}

void Transport::send(Message msg) {}

}  // namespace midi::aseq