#include "Jamb.hh"
#include "test/FakeMidi.hh"
#include "common.hh"

int main(void) {
  FakeMidi lpMidi, aeolusMidi;
  Launchpad launchpad{lpMidi};
  Aeolus aeolus{aeolusMidi};
  Jamb jamb{launchpad, aeolus};
  launchpad.init();
  jamb.init();

  lpMidi.emit({0x90, {0x78, 1}}); // temporary: general cancel at startup
  std::cout << aeolusMidi << "\n";

  return 0;
}
