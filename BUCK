## For buliding on Mac

load('//:subdir_glob.bzl', 'subdir_glob')

COMPILER_FLAGS = ['-g', '-O2', '-Wall', '-Werror', '-std=c++20', '-I/opt/homebrew/include']
LINKER_FLAGS = ['-L/opt/homebrew/lib', '-lfmt', '-lgtest', '-lyaml-cpp']

genrule(
    name = "defaultConfig",
    srcs = ["codegen-defaultConfig.sh", "jamb.config.yaml"],
    bash = "bash codegen-defaultConfig.sh > $OUT",
    out = "Launchpad-defaultConfig.hh",
)

cxx_library(
    name = "lib",
    srcs = glob(["src/*.cc"]),
    exported_headers = subdir_glob([("", "src/*.hh")]) |
        {'Launchpad-defaultConfig.hh': ':defaultConfig'},

    linker_flags = LINKER_FLAGS,
    compiler_flags = COMPILER_FLAGS,
)

cxx_test(
    name = 'test',
    srcs = glob(["src/test/*.cc"]),
    headers = glob(["src/test/*.hh"]),
    deps = [":lib"],

    linker_flags = LINKER_FLAGS,
    compiler_flags = COMPILER_FLAGS,
)
