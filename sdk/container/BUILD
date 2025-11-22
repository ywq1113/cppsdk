load("@rules_cc//cc:cc_library.bzl", "cc_library")

package(default_visibility = ["//visibility:public"])

cc_library(
    name = "container",
    srcs = [
        "src/segment_tree.cpp",
    ],
    # hdrs = [
    #     "container/include/segment_tree.h",
    #     "container/include/spsc_ringbuffer.hpp",
    # ],
    hdrs = glob(["include/**/*.h", "include/**/*.hpp"]),
    include_prefix = "container",
    strip_include_prefix = "include",
    visibility = ["//visibility:public"],
    deps = [
        "//sdk/macro",
        "@glog",
    ],
)
