#include "log/log.h"

#include <spdlog/async.h>
#include <spdlog/sinks/rotating_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <vector>

namespace sdk {
namespace log {

namespace {
std::once_flag g_init_flag;
} // namespace

void InitLogging(const InitOptions &opt) {
  std::call_once(g_init_flag, [&]() {
    spdlog::init_thread_pool(opt.queue_size, opt.worker_threads);

    std::vector<spdlog::sink_ptr> sinks;

    if (opt.to_stdout) {
      sinks.push_back(std::make_shared<spdlog::sinks::stdout_color_sink_mt>());
    }

    if (opt.to_stderr) {
      sinks.push_back(std::make_shared<spdlog::sinks::stderr_color_sink_mt>());
    }

    if (opt.to_file) {
      sinks.push_back(std::make_shared<spdlog::sinks::rotating_file_sink_mt>(
          opt.file_name, opt.max_file_size, opt.max_files));
    }

    if (sinks.empty()) {
      sinks.push_back(std::make_shared<spdlog::sinks::stdout_color_sink_mt>());
    }

    auto logger = std::make_shared<spdlog::async_logger>(
        "sdk", sinks.begin(), sinks.end(), spdlog::thread_pool(),
        spdlog::async_overflow_policy::block);

    spdlog::set_default_logger(logger);
    spdlog::set_level(opt.level);
    spdlog::flush_on(spdlog::level::warn);
    spdlog::set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%^%l%$] %v");
  });
}

void ShutdownLogging() {
  spdlog::shutdown();
}

LogMessage::LogMessage(Severity severity, const char *file, int line,
                       bool fatal)
    : level_(ToSpdLevel(severity)), fatal_(fatal) {
  stream_ << file << ":" << line << " ";
}

LogMessage::~LogMessage() {
  try {
    auto logger = spdlog::default_logger();
    if (logger) {
      logger->log(level_, stream_.str());
      if (fatal_) {
        logger->flush();
        std::abort();
      }
    }
  } catch (...) {
    if (fatal_) std::abort();
  }
}

} // namespace log
} // namespace sdk
