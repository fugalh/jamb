cxx_library(
    name = "lib",
    srcs = glob(
        ["*.cc"],
        exclude = ["main.cc"]
    ),
    headers = glob(["*.hh"]),

    compiler_flags = ['-g', '-O2', '-std=c++20'],
)

cxx_test(
    name = 'test',
    srcs = glob(["test/*.cc"]),
    headers = glob(["test/*.hh"]),
    deps = [":lib"],

    linker_flags = ['-lgtest'],
    compiler_flags = ['-g', '-O2', '-std=c++20'],
)

cxx_binary(
    name = "jamb",
    srcs = ["main.cc"],
    deps = [":lib"],

    compiler_flags = ['-g', '-O2', '-std=c++20'],
)
