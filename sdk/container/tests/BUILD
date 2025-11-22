load("@rules_cc//cc:cc_test.bzl", "cc_test")

package(default_visibility = ["//visibility:public"])

cc_test(
    name = "segment_tree_test",
    srcs = ["segment_tree_test.cpp"],
    deps = [
        "//sdk/container",
        "@glog",
        "@googletest//:gtest_main",
    ],
)

cc_test(
    name = "ringbuffer_test",
    srcs = ["ringbuffer_test.cpp"],
    deps = [
        "//sdk/container",
        "@glog",
        "@googletest//:gtest_main",
    ],
)
