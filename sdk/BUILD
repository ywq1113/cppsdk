load("@rules_pkg//:pkg.bzl", "pkg_tar")

# === 1. 收集 third_party 库头文件 ===
filegroup(
    name = "third_party_includes",
    srcs = [
        "@com_github_google_glog//:all_srcs",
        "@com_github_gflags_gflags//:all_srcs",
    ],
    visibility = ["//visibility:public"],
)

# === 2. 收集第三方动态库 (.so) 文件 ===
filegroup(
    name = "third_party_libs",
    srcs = [
        "@com_github_google_glog//:glog",
        "@com_github_gflags_gflags//:gflags",
    ],
    visibility = ["//visibility:public"],
)

# === 3. 打包 SDK，包含头文件和库文件 ===
pkg_tar(
    name = "third_party_sdk",
    srcs = [
        ":third_party_includes",
        ":third_party_libs",
    ],
    package_dir = "third_party_sdk",
    strip_prefix = ".",
    include_runfiles = True,
    stamp = 0,
    visibility = ["//visibility:public"],
)
