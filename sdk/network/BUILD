load("@rules_cc//cc:cc_library.bzl", "cc_library")

package(default_visibility = ["//visibility:public"])

cc_library(
    name = "network",
    srcs = [
        "src/tcp_client.cc",
        "src/tcp_client_impl.cc",
        "src/tcp_server.cc",
        "src/tcp_server_impl.cc",
    ],
    hdrs = glob(["include/**/*.h"]),
    copts = [
        "-std=c++20",
    ],
    include_prefix = "network",
    strip_include_prefix = "include",
    visibility = ["//sdk:__subpackages__"],
    deps = [
        "//sdk/log",
        "//sdk/macro",
        "@boost.asio//:boost.asio",
    ],
)
