cc_binary(
    name = "spsc_ringbuffer_benchmark",
    srcs = ["spsc_ringbuffer_benchmark.cpp"],
    deps = [
        "//sdk:algorithm_v1",
        "@glog//:glog",
        "@google_benchmark//:benchmark",
        "@boost.lockfree//:boost.lockfree",
    ],
    copts = [
        "-O2",
    ],
)
