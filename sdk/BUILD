load("@rules_pkg//:pkg.bzl", "pkg_tar")

cc_library(
    name = "algorithm_v1",
    srcs = glob(["src/*.cpp"]),
    hdrs = glob(["include/*/*.h"]),
    includes = ["include"],
    strip_include_prefix = "include",
    visibility = ["//visibility:public"],
)

