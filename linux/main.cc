
#include "ASeq.hh"

#include "../Jamb.hh"
#include "../common.hh"
#include "../test/FakeMidi.hh"

#include <thread>

int main(void) {
  midi::aseq::Sequencer seq("jamb");
  FakeMidi aeolusMidi;
  Launchpad launchpad{*seq.launchpad_};
  Aeolus aeolus{*seq.aeolus_};
  Jamb jamb{launchpad, aeolus};
  launchpad.init();
  jamb.init();

  aeolus.generalCancel();
  aeolus.stopOn(1, 0);  // I prin8 (channel 0)
  aeolus.stopOn(2, 3);  // III suab8 (channel 1)
  aeolus.stopOn(2, 4);  // III rofl4 (channel 1)
  // Less annoying: `watch pmidi -p 128:0 ~/bwv651.mid`
  // aeolus.noteOn(0, 69);

  seq.launchpad_->readLoop();
  return 0;
}
