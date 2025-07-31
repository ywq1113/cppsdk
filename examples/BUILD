cc_library(
    name = "glog_example",
    srcs = ["src/test_glog.cpp"],
    hdrs = glob(["include/**/*.h"]),
    includes = ["include"],
    deps = [
        "@com_github_google_glog//:glog",
        "@com_github_gflags_gflags//:gflags",
    ],
    visibility = ["//visibility:public"],
)

cc_binary(
    name = "demo",
    srcs = ["main.cpp"],
    deps = [
        ":glog_example",
        "@com_github_google_glog//:glog",
        "@com_github_gflags_gflags//:gflags",
    ],
)
