#!/bin/bash
set -e

sudo apt-get -y install curl \
    unzip \
    build-essential \
    clang \
    clang-format \
    libc++-dev \
    libc++abi-dev \
    lld

BAZEL_VERSION=$(bazel --version | awk '{print $2}')

if [[ "$BAZEL_VERSION" == "8.3.1" ]]; then
    echo "Bazel 8.3.1 is already installed."
else
    echo "Current Bazel version is: $BAZEL_VERSION"
    echo "Installing Bazel 8.3.1..."
    curl -L https://github.com/bazelbuild/bazel/releases/download/8.3.1/bazel-8.3.1-installer-linux-x86_64.sh -o bazel-installer.sh
    chmod +x bazel-installer.sh
    sudo ./bazel-installer.sh
    rm -f bazel-installer.sh
fi
