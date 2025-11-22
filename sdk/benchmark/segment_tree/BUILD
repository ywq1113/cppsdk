load("@rules_cc//cc:cc_binary.bzl", "cc_binary")

package(default_visibility = ["//visibility:public"])

cc_binary(
    name = "segment_tree_benchmark_no_simd",
    srcs = ["segment_tree_benchmark.cpp"],
    copts = [
        "-O1",
    ],
    deps = [
        "//sdk/container",
        "@glog",
        "@google_benchmark//:benchmark",
    ],
)

cc_binary(
    name = "segment_tree_benchmark_simd",
    srcs = ["segment_tree_benchmark.cpp"],
    copts = [
        "-O3",
        "-march=native",
        "-Rpass=loop-vectorize",
        "-Rpass=slp-vectorize",
        "-Rpass-missed=loop-vectorize",
        "-Rpass-analysis=loop-vectorize",
    ],
    deps = [
        "//sdk/container",
        "@glog",
        "@google_benchmark//:benchmark",
    ],
)
