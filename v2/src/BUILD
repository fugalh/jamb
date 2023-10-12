cc_library(
  name = "jamb",
  srcs = glob(["*.cc"]),
  hdrs = glob(["*.hh"]),
)

cc_test(
  name = "tests",
  size = "small",
  srcs = glob(["test/*.cc"]),
  deps = [
    "@com_google_googletest//:gtest_main",
    ":jamb",
  ],
  includes = ["."],
)