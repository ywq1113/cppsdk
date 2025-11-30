load("@rules_cc//cc:cc_library.bzl", "cc_library")

package(default_visibility = ["//visibility:public"])

cc_library(
    name = "log",
    srcs = [
        "src/log.cc",
    ],
    hdrs = glob(["include/**/*.h"]),
    copts = [
        "-std=c++20",
    ],
    include_prefix = "log",
    strip_include_prefix = "include",
    visibility = ["//sdk:__subpackages__"],
    deps = [
        "//sdk/macro",
        "@spdlog//:spdlog",
    ],
)
