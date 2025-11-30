load("@rules_cc//cc:cc_library.bzl", "cc_library")

package(default_visibility = ["//visibility:public"])

cc_library(
    name = "macro",
    hdrs = glob(["include/**/*"]),
    include_prefix = "macro",
    strip_include_prefix = "include",
    alwayslink = True,
    visibility = ["//sdk:__subpackages__"],
)
