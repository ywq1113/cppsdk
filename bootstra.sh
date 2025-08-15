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

curl -L https://github.com/bazelbuild/bazel/releases/download/8.3.1/bazel-8.3.1-installer-linux-x86_64.sh -o bazel-installer.sh
chmod +x bazel-installer.sh
sudo ./bazel-installer.sh
rm -f bazel-installer.sh
