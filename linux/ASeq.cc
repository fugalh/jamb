#include "ASeq.hh"

#include <optional>

namespace {

std::optional<int> findClient(snd_seq_t* seq, std::string clientName) {
  snd_seq_client_info_t* cinfo;
  snd_seq_port_info_t* pinfo;

  snd_seq_client_info_alloca(&cinfo);
  snd_seq_port_info_alloca(&pinfo);

  while (snd_seq_query_next_client(seq, cinfo) >= 0) {
    int client = snd_seq_client_info_get_client(cinfo);
    if (clientName == snd_seq_client_info_get_name(cinfo)) {
      return client;
    }
  }
  return {};
}

}  // namespace

namespace midi::aseq {

Transport::Transport(snd_seq_t* const& seq, std::string client) : seq_(seq) {
  auto const otherClient = findClient(seq_, client);
  int const otherPort = 0;
  auto caps = SND_SEQ_PORT_CAP_READ | SND_SEQ_PORT_CAP_SUBS_READ |
              SND_SEQ_PORT_CAP_WRITE | SND_SEQ_PORT_CAP_SUBS_WRITE;
  enum { to, fro, tofro } direction = tofro;
  if (client == "aeolus") {
    caps = SND_SEQ_PORT_CAP_WRITE | SND_SEQ_PORT_CAP_SUBS_WRITE;
    direction = to;
  }
  ASEQ_CHECK(port_ = snd_seq_create_simple_port(seq_, client.c_str(), caps,
                                                SND_SEQ_PORT_TYPE_APPLICATION));
  if (otherClient) {
    try {
      if (direction == to || direction == tofro) {
        ASEQ_CHECK(snd_seq_connect_to(seq_, port_, *otherClient, otherPort));
      }
      if (direction == fro || direction == tofro) {
        ASEQ_CHECK(snd_seq_connect_from(seq_, port_, *otherClient, otherPort));
      }
      LOG << "Connected to " << *otherClient << ":0 (" << client << ")\n";
    } catch (std::runtime_error err) {
      LOG << err.what() << " when connecting to " << *otherClient << ":"
          << otherPort << " (" << client << ") " << direction << "\n";
    }
  } else {
    LOG << "ALSA Sequencer client '" << client << "' not found\n";
  }
}

Transport::~Transport() {
  if (port_ >= 0) {
    snd_seq_delete_simple_port(seq_, port_);
  }
}

Sequencer::Sequencer(std::string name) {
  ASEQ_CHECK(snd_seq_open(&seq_, "default", SND_SEQ_OPEN_DUPLEX, 0));
  ASEQ_CHECK(snd_seq_set_client_name(seq_, name.c_str()));
  launchpad_ = std::make_unique<Transport>(seq_, "Launchpad Mini");
  aeolus_ = std::make_unique<Transport>(seq_, "aeolus");
}

Sequencer::~Sequencer() {
  if (seq_) {
    snd_seq_close(seq_);
  }
}

}  // namespace midi::aseq
