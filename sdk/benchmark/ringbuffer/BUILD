load("@rules_cc//cc:cc_binary.bzl", "cc_binary")

package(default_visibility = ["//visibility:public"])

cc_binary(
    name = "spsc_ringbuffer_benchmark",
    srcs = ["spsc_ringbuffer_benchmark.cc"],
    copts = [
        "-O2",
    ],
    deps = [
        "//sdk/container",
        "@boost.lockfree//:boost.lockfree",
        "@glog",
        "@google_benchmark//:benchmark",
    ],
)
