
#include "ASeq.hh"

#include "../Jamb.hh"
#include "../common.hh"
#include "../test/FakeMidi.hh"

#include <thread>

int main(void) {
  midi::aseq::Sequencer seq("jamb");
  FakeMidi aeolusMidi;
  Launchpad launchpad{*seq.launchpad_};
  Aeolus aeolus{aeolusMidi};
  Jamb jamb{launchpad, aeolus};
  launchpad.init();
  jamb.init();

  LOG << "Waiting 30s\n";
  std::this_thread::sleep_for(std::chrono::seconds(10));
  LOG << "\n" << aeolusMidi << "\n";

  return 0;
}
