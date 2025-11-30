#pragma once

#include <spdlog/spdlog.h>
#include <sstream>
#include <string>

namespace sdk {
namespace log {

enum class Severity {
  INFO,
  WARNING,
  ERROR,
  FATAL,
};

inline constexpr spdlog::level::level_enum ToSpdLevel(Severity s) {
  switch (s) {
  case Severity::INFO:
    return spdlog::level::info;
  case Severity::WARNING:
    return spdlog::level::warn;
  case Severity::ERROR:
    return spdlog::level::err;
  case Severity::FATAL:
    return spdlog::level::critical;
  }
  return spdlog::level::info;
}

struct InitOptions {
  bool to_stdout = true;
  bool to_stderr = false;
  bool to_file = false;
  std::string file_name = "server.log";
  size_t max_file_size = 10 * 1024 * 1024;
  size_t max_files = 5;
  size_t queue_size = 8192;
  size_t worker_threads = 1;
  spdlog::level::level_enum level = spdlog::level::info;
};

void InitLogging(const InitOptions &opts = {});
void ShutdownLogging();

// 真正承接日志的类，支持 << 语法
class LogMessage {
 public:
  LogMessage(Severity severity, const char *file, int line, bool fatal = false);
  ~LogMessage();

  std::ostringstream &stream() {
    return stream_;
  }

 private:
  spdlog::level::level_enum level_;
  bool fatal_;
  std::ostringstream stream_;
};

} // namespace log
} // namespace sdk

#define LOG_INTERNAL(sev, fatal)                                               \
  ::sdk::log::LogMessage(::sdk::log::Severity::sev, __FILE__, __LINE__, fatal) \
      .stream()

#define LOG(severity) LOG_INTERNAL(severity, false)

#define LOG_IF(severity, cond) !(cond) ? (void)0 : LOG(severity)

#define CHECK(cond) \
  (cond) ? (void)0 : LOG_INTERNAL(FATAL, true) << "Check failed: " #cond " "

#ifndef NDEBUG
#define DLOG(severity) LOG(severity)
#else
#define DLOG(severity) true ? (void)0 : LOG(severity)
#endif
