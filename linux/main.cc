
#include "ASeq.hh"

#include "../Jamb.hh"
#include "../common.hh"
#include "../test/FakeMidi.hh"
#include "Launchpad-defaultConfig.hh"

#include <filesystem>
#include <iostream>
#include <optional>
#include <stringstream>
#include <thread>

static std::optional<std::string> readConfig() {
  auto path = std::filesystem::path(getenv("HOME")) / ".jamb.config";
  if (std::filesystem::exists(path)) {
    std::ifstream in(path.string());
    std::stringstream buf;
    buf << in.rdbuf();
    return buf.str();
  }
  return std::nullopt;
}

int main(void) {
  midi::aseq::Sequencer seq("jamb");

  Launchpad launchpad{*seq.launchpad_};
  auto configStr = readConfig();
  if (configStr) {
    launchpad.configureStopmap(configStr);
  }
  Aeolus aeolus{*seq.aeolus_};

  jamb::Model jamb{launchpad, aeolus, true /* persistMemory */};
  jamb.readMemory();

  seq.launchpad_->readLoop();
  return 0;
}
