#pragma once

#include <string>
#include <typeinfo>
#include <vector>

#define LOG_CLASS_TAG typeid(this).name()

enum LogType { LOG_INFO, LOG_WARNING, LOG_ERROR };

struct LogEntry {
  LogType type;
  std::string message;
};

class Logger {
 public:
  static std::vector<LogEntry> messages;
  static void Log(const std::string& tag, const std::string& message);
  static void Err(const std::string& tag, const std::string& message);
};
