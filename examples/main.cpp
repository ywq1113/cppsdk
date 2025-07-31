#include "glog_example/test_glog.h"

int main(int argc, char* argv[]) {
    google::InitGoogleLogging(argv[0]);
    gflags::ParseCommandLineFlags(&argc, &argv, true);

    my_sdk::log_message("SDK is working!");
    return 0;
}