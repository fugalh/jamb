
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

  seq.launchpad_->readLoop();
  return 0;
}
