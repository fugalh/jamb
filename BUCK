cxx_library(
    name = "jamb",
    srcs = glob(["*.cc"]),
    headers = glob(["*.hh"]),
    compiler_flags = ['-g', '-O2', '-std=c++20'],
)

cxx_test(
    name = 'test',
    srcs = glob(["test/*.cc"]),
    headers = glob(["test/*.hh"]),
    compiler_flags = ['-g', '-O2', '-std=c++20'],
    linker_flags = ['-lgtest'],
    deps = [":jamb"],
)
