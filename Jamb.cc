#include "Jamb.hh"

void Jamb::init() {
  launchpad_.observer_ = [this](Launchpad::Event ev) { dispatch(ev); };
}

void Jamb::dispatch(Launchpad::Event ev) {
  switch (ev.type) {
    case Launchpad::Event::Type::ButtonPress:
      switch (ev.button) {
        case 0x78:
          aeolus_.generalCancel();
          break;
      }
  }
}