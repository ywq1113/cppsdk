load("@bazel_tools//tools/build_defs/repo:git.bzl", "git_repository")
load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")

# ==============================
# 配置开关
# ==============================
# True  = 使用 git_repository 方式，方便测试指定 commit
# False = 使用 http_archive 固定版本 + sha256（推荐生产环境）
USE_GIT_DEPS = False

def _load_gflags():
    if USE_GIT_DEPS:
        git_repository(
            name = "com_github_gflags_gflags",
            remote = "https://github.com/gflags/gflags.git",
            commit = "52e94563eba1968783864942fedf6e87e3c611f4", # 2025.04.01
        )
    else:
        http_archive(
            name = "com_github_gflags_gflags",
            urls = ["https://github.com/gflags/gflags/archive/refs/tags/v2.2.2.tar.gz"],
            sha256 = "34af2f15cf7367513b352bdcd2493ab14ce43692d2dcd9dfc499492966c64dcf",
            strip_prefix = "gflags-2.2.2",
        )

def _load_glog():
    if USE_GIT_DEPS:
        git_repository(
            name = "com_github_google_glog",
            remote = "https://github.com/google/glog.git",
            commit = "53d58e4531c7c90f71ddab503d915e027432447a", # 2025.05.31
        )
    else:
        http_archive(
            name = "com_github_google_glog",
            urls = ["https://github.com/google/glog/archive/refs/tags/v0.6.0.tar.gz"],
            sha256 = "8a83bf982f37bb70825df71a9709fa90ea9f4447fb3c099e1d720a439d88bad6",
            strip_prefix = "glog-0.6.0",
        )

def load_third_party_deps():
    """统一加载第三方依赖"""
    _load_gflags()
    _load_glog()