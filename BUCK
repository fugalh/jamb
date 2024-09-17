COMPILER_FLAGS = ['-g', '-O2', '-std=c++20', '-I/opt/homebrew/include']

cxx_library(
    name = "lib",
    srcs = glob(
        ["*.cc"],
        exclude = ["main.cc"]
    ),
    headers = glob(["*.hh"]),

    compiler_flags = COMPILER_FLAGS,
)

cxx_test(
    name = 'test',
    srcs = glob(["test/*.cc"]),
    headers = glob(["test/*.hh"]),
    deps = [":lib"],

    linker_flags = ['-lgtest'],
    compiler_flags = COMPILER_FLAGS,
)

cxx_binary(
    name = "jamb",
    srcs = ["main.cc"],
    deps = [":lib"],

    linker_flags = ['-L/opt/homebrew/lib', '-lspdlog', '-lfmt'],
    compiler_flags = COMPILER_FLAGS,
)
