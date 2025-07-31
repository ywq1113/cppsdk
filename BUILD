cc_library(
    name = "my_sdk",
    srcs = ["src/my_sdk.cpp"],
    hdrs = glob(["include/**/*.h"]),
    includes = ["include"],
    deps = [
        "@com_github_google_glog//:glog",
        "@com_github_gflags_gflags//:gflags",
    ],
    visibility = ["//visibility:public"],
)