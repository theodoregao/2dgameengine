#include "Logger.h"

#include <chrono>
#include <ctime>
#include <iostream>
#include <string>

std::vector<LogEntry> Logger::messages;

std::string CurrentDateTimeToString() {
  std::time_t now =
      std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
  std::string output(30, '\0');
  std::strftime(&output[0], output.size(), "%Y/%m/%d %H:%M:%S",
                std::localtime(&now));
  return output;
}

void Logger::Log(const std::string& tag, const std::string& message) {
  LogEntry logEntry{LOG_INFO, "LOG: [" + CurrentDateTimeToString() + "] " +
                                  "<" + tag + "> : " + message};
  std::cout << "\x1B[32m" << logEntry.message << "\033[0m" << std::endl;
  messages.push_back(logEntry);
}

void Logger::Err(const std::string& tag, const std::string& message) {
  LogEntry logEntry{LOG_ERROR, "ERR: [" + CurrentDateTimeToString() + "] " +
                                   "<" + tag + "> : " + message};
  std::cout << "\x1B[91m" << logEntry.message << "\033[0m" << std::endl;
  messages.push_back(logEntry);
}
