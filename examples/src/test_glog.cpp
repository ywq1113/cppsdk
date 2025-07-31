#include "glog_example/test_glog.h"
#include <glog/logging.h>
#include <gflags/gflags.h>

DEFINE_string(name, "World", "Name to greet");

namespace my_sdk {
    void log_message(const std::string& msg) {
        LOG(INFO) << "Hello from my_sdk: " << msg;
    }
}