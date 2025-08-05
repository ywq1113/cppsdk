cc_binary(
    name = "segment_tree_benchmark_no_simd",
    srcs = ["segment_tree_benchmark.cpp"],
    deps = [
        "//sdk:algorithm_v1",
        "@glog//:glog",
        "@google_benchmark//:benchmark",
    ],
    copts = [
        "-O3",
        "-march=native",
        "-ftree-vectorize",
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
        "-ftree-vectorize",
        "-ftree-vectorizer-verbose=2",
        "-mavx2",
        "-msse4.2",
    ],
)
