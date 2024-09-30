
cat > "$OUT" <<EOF
#pragma once
#include <string>

namespace launchpad {

std::string const kDefaultConfig = R"(
$(cat jamb.config.yaml)
)";

}  // namespace launchpad
EOF