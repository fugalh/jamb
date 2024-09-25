COMPILER_FLAGS = ['-g', '-O2', '-std=c++20', '-I/opt/homebrew/include']
LINKER_FLAGS = ['-L/opt/homebrew/lib', '-lfmt', '-lgtest', '-lyaml-cpp']

cxx_library(
    name = "lib",
    srcs = glob(["*.cc"]),
    headers = glob(["*.hh"]),

    linker_flags = LINKER_FLAGS,
    compiler_flags = COMPILER_FLAGS,
)

cxx_test(
    name = 'test',
    srcs = glob(["test/*.cc"]),
    headers = glob(["test/*.hh", "*.hh"]),
    deps = [":lib"],

    linker_flags = LINKER_FLAGS,
    compiler_flags = COMPILER_FLAGS,
)
