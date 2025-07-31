#!/bin/bash

sudo apt-get -y install curl \
    build-essential

curl -L https://github.com/bazelbuild/bazel/releases/download/8.3.1/bazel-8.3.1-installer-linux-x86_64.sh -o bazel-installer.sh
chmod +x bazel-installer.sh
sudo ./bazel-installer.sh
sudo rm bazel-installer.sh
