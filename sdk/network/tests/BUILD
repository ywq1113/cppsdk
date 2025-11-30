load("@rules_cc//cc:defs.bzl", "cc_test")

cc_test(
    name = "tcp_client_server_test",
    srcs = [
        "tcp_client_server_test.cc",
        "tcp_server_test_wrapper.h",
    ],
    copts = [
        "-std=c++20",
    ],
    deps = [
        "//sdk/network",
        "@glog",
        "@googletest//:gtest_main",
    ],
)
