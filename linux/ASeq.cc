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

void sendEvent(snd_seq_t* seq, snd_seq_event_t& ev) {
  snd_seq_event_output(seq, &ev);
  snd_seq_drain_output(seq);
}

}  // namespace

namespace midi::aseq {

Transport::Transport(snd_seq_t* const& seq, std::string client)
    : seq_(seq), name_{client} {
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
  snd_seq_delete_simple_port(seq_, port_);
}

snd_seq_event_t Transport::eventPrototype() {
  snd_seq_event_t ev;
  snd_seq_ev_clear(&ev);
  snd_seq_ev_set_source(&ev, port_);
  snd_seq_ev_set_subs(&ev);
  snd_seq_ev_set_direct(&ev);
  return ev;
}

void Transport::send(Message msg) {
  LOGf("[%14s] %02x %02x %02x", name_.c_str(), msg.status, msg.data[0],
       msg.data[1]);
  auto ch = msg.status & 0x0f;
  switch (msg.status & 0xf0) {
    case 0x90: {
      auto ev = eventPrototype();
      auto const key = msg.data[0];
      auto const vel = msg.data[1];
      snd_seq_ev_set_noteon(&ev, ch, key, vel);
      sendEvent(seq_, ev);
      break;
    }

    case 0xb0: {
      auto ev = eventPrototype();
      auto const cc = msg.data[0];
      auto const val = msg.data[1];
      snd_seq_ev_set_controller(&ev, ch, cc, val);
      sendEvent(seq_, ev);
      break;
    }

    case 0xc0: {
      auto ev = eventPrototype();
      auto const val = msg.data[0];
      snd_seq_ev_set_pgmchange(&ev, ch, val);
      sendEvent(seq_, ev);
      break;
    }

    default:
      LOGf("unknown status %2x", msg.status);
  }
}

void Transport::readLoop() {
  while (true) {
    snd_seq_event_t* ev{};
    auto err = snd_seq_event_input(seq_, &ev);
    if (err < 0) {
      LOG << snd_strerror(err) << "\n";
      break;
    }
    if (!ev || !observer) {
      continue;
    }
    uint8_t status;
    switch (ev->type) {
      case SND_SEQ_EVENT_NOTEON: {
        auto const& note = ev->data.note;
        if (note.velocity > 0) {
          LOGf("noteon %2x %2x", note.note, note.velocity);
          status = note.channel | 0x90;
          observer({status, {note.note, note.velocity}});
        }
        break;
      }
      case SND_SEQ_EVENT_CONTROLLER: {
        auto const& control = ev->data.control;
        LOGf("controller %2x %2x", control.param, control.value);
        status = control.channel | 0xb0;
        observer({status, {uint8_t(control.param), uint8_t(control.value)}});
        break;
      }
      default:
        LOG << "unknown event type " << int(ev->type) << "\n";
        break;
    }
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
