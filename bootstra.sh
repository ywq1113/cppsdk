#!/bin/bash
set -x
set -e

sudo apt-get -y install curl \
    unzip \
    build-essential \
    clang \
    clang-format \
    libc++-dev \
    libc++abi-dev \
    lld

bazel_version="8.3.1"
current_version=$(command -v bazel >/dev/null 2>&1 && bazel --version | awk '{print $2}' || echo "")
if [[ "$current_version" == "$bazel_version" ]]; then
    echo "Bazel $bazel_version is already installed."
    exit 0
fi

arch=$(uname -m)
if [[ "$arch" == "x86_64" ]]; then
    curl -L https://github.com/bazelbuild/bazel/releases/download/8.3.1/bazel-8.3.1-installer-linux-x86_64.sh -o bazel-installer.sh
    chmod +x bazel-installer.sh
    sudo ./bazel-installer.sh
    rm -f bazel-installer.sh
elif [[ "$arch" == "aarch64" || "$arch" == "arm64" ]]; then
    wget https://github.com/bazelbuild/bazel/releases/download/8.3.1/bazel-8.3.1-linux-arm64
    chmod +x bazel-8.3.1-linux-arm64
    sudo mv bazel-8.3.1-linux-arm64 /usr/local/bin/bazel
else
    echo "Not support arch: $arch"
    exit 1
fi
