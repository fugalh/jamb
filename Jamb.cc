#include "Jamb.hh"

void Jamb::init() {
  launchpad_.observer_ = [this](Launchpad::Event ev) { dispatch(ev); };
}

void Jamb::dispatch(Launchpad::Event ev) {
  switch (ev.type) {
    case Launchpad::Event::Type::ButtonPress:
      if (ev.button == 0x78) {
        aeolus_.generalCancel();
      } else if (ev.button >= 0x80) {
        aeolus_.programChange(ev.button - 0x80);
      }
  }
}
