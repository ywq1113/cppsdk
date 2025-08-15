cc_binary(
    name = "segment_tree_benchmark_no_simd",
    srcs = ["segment_tree_benchmark.cpp"],
    deps = [
        "//sdk:algorithm_v1",
        "@glog//:glog",
        "@google_benchmark//:benchmark",
    ],
    copts = [
        "-O1",
    ],
)

cc_binary(
    name = "segment_tree_benchmark_simd",
    srcs = ["segment_tree_benchmark.cpp"],
    deps = [
        "//sdk:algorithm_v1",
        "@glog//:glog",
        "@google_benchmark//:benchmark",
    ],
    copts = [
        "-O3",
        "-march=native",
        "-Rpass=loop-vectorize",
        "-Rpass=slp-vectorize",
        "-Rpass-missed=loop-vectorize",
        "-Rpass-analysis=loop-vectorize",
    ],
)
