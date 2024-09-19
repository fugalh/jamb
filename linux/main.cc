
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
  aeolus.stopOn(1, 0);  // listening on channel 0
  aeolus.stopOn(2, 3);  // listening on channel 1
  aeolus.stopOn(2, 4);  // listening on channel 1
  // Less annoying: `watch pmidi -p 128:0 ~/bwv651.mid`
  // aeolus.noteOn(0, 69);

  LOG << "Waiting 300s\n";
  std::this_thread::sleep_for(std::chrono::seconds(300));
  LOG << "\n" << aeolusMidi << "\n";

  return 0;
}
