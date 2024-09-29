#pragma once
#include <string>

namespace launchpad {

// If you change this don't forget to update stopyaml.yaml
// (maybe we can codegen from stopmap.yaml?)
std::string const kDefaultConfig = R"(
# A mapping from Aeolus (group, element) to Launchpad buttons.
# The Aeolus UI is laid out horizontally, but in the more traditional knob
# style stops would be laid out in columns on the jamb, with Pedal and Swell
# on the left and Great and Choir on the right. Groups are mapped to columns
# with `groupmap`. In `stopmap` the elements in each group are numbered 0-f
# by the order they appear in the Aeolus UI for that group, and placed in
# their position in the Launchpad grid with spaces between columns for
# legibility. Dots are for blank spaces. All indices are 0-based.
---
# Instrument directory name inside the `stops` directory,
# so that the helper script can find the stop names
- instrument: Aeolus
  launchpad:
    # P     III   I     II
    # Pedal Swell Great Choir
    groupmap: [3, 0, 2, 1]
    stopmap: |
      ce b. de bc
      9f .. cf ..
      ad a. 3b 9.
      b8 89 5a 6a
      45 76 29 78
      37 45 48 45
      26 23 17 23
      10 01 06 01
      )";

}  // namespace launchpad