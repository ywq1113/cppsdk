#include <iostream>
#include <vector>
#include <thread>
#include <atomic>
#include <mutex> // temp
#include <chrono>

// Note:
// 1. 线程 id 可以自定义
// 2. 不得冲突可以使用锁
// 3. 输出格式没有要求，可以打到标准输出
// 4. 如果写失败，没有要求

const int kFailed = 1000;
const int kSuccess = 0;
const int kThreadNum = 5;

static std::atomic<int> lock_(0); // TODO: Opt
static std::mutex g_mutex_;

struct context {
    int thread_id;
    int w_cnt;
};

bool terminate_condition(std::vector<int> &nums) {
    if (nums.size() == 100) {
        std::cout << "Current size: " << nums.size() << "\n";
        return true;
    }
    return false;
}

void single_thread_write_arr(std::vector<int> &nums, int thread_id, std::mutex& mutex, context& ctx) {
    while (true) {
        std::lock_guard<std::mutex> lg(mutex);
	{
	    if (terminate_condition(nums)) {
	        break;
	    }
	    nums.emplace_back(thread_id);
	    ctx.w_cnt++;
	    std::cout << "Thread id: " << thread_id << ", w_cnt = " << ctx.w_cnt << "\n";
	}
        std::this_thread::sleep_for(std::chrono::milliseconds(2000));
    }
}

int64_t paralle_write_arr(std::vector<int> &nums, int thread_num) {
    if (thread_num < 1) {
        std::cout << "Invalid thread number\n";
        return kFailed;
    }
    std::vector<std::thread> thread_list;
    std::vector<context> ctx_list(5, {0, 0});
    for (int i = 0; i < kThreadNum; i++) {
        thread_list.emplace_back(std::thread([i, &nums, &ctx_list]() {
            ctx_list[i].thread_id = i;
            single_thread_write_arr(nums, i, g_mutex_, ctx_list[i]);
        }));
    }
    // Join
    for (auto &th : thread_list) {
        th.join();
    }
    for (const auto &ctx : ctx_list) {
        std::cout << "Thread id: " << ctx.thread_id << ", write cnt: " << ctx.w_cnt << "\n";
    }
    std::cout << "Parallel write sucess\n";
    return kSuccess;
}


int main() {
    std::cout << "Start!\n";
    std::vector<int> nums;
    int64_t ret = paralle_write_arr(nums, 5);
    std::cout << "End!\n";
    return ret;
}

