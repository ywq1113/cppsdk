load("@rules_cc//cc:cc_library.bzl", "cc_library")

package(default_visibility = ["//visibility:public"])

cc_library(
    name = "time",
    srcs = [
        "src/time.cc",
    ],
    hdrs = glob(["include/**/*.h"]),
    copts = [
        "-std=c++20",
    ],
    include_prefix = "time",
    strip_include_prefix = "include",
    visibility = ["//sdk:__subpackages__"],
    deps = [
        "//sdk/log",
        "//sdk/macro",
    ],
)
