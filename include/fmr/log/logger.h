/*
 *  Copyright (c) 2021 Uskrai
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#ifndef H_FMR_LOG_LOGGER_H
#define H_FMR_LOG_LOGGER_H

#include <fmt/core.h>

#include <string>

namespace fmr {

namespace log {

class Logger {
  inline static Logger *global_ = nullptr;

 public:
  enum LogLevel { kTrace, kDebug, kInfo, kWarn, kCritical, kError, kNone };
  virtual ~Logger() {}

  template <typename... U>
  void Log(LogLevel level, std::string_view format, U &&...args) {
    if (ShouldLog(level))
      DoLog(level, fmt::format(format, std::forward<U>(args)...));
  }

  void Log(LogLevel level, std::string_view str) {
    if (ShouldLog(level)) DoLog(level, str);
  }

  template <typename... U>
  void Trace(std::string_view format, U &&...args) {
    return Log(kTrace, format, std::forward<U>(args)...);
  }

  template <typename... U>
  void Debug(std::string_view format, U &&...args) {
    return Log(kDebug, format, std::forward<U>(args)...);
  }

  template <typename... U>
  void Info(std::string_view format, U &&...args) {
    return Log(kInfo, format, std::forward<U>(args)...);
  }

  template <typename... U>
  void Warn(std::string_view format, U &&...args) {
    return Log(kWarn, format, std::forward<U>(args)...);
  }

  template <typename... U>
  void Critical(std::string_view format, U &&...args) {
    return Log(kCritical, format, std::forward<U>(args)...);
  }

  template <typename... U>
  void Error(std::string_view format, U &&...args) {
    return Log(kError, format, std::forward<U>(args)...);
  }

  virtual void Flush() = 0;

  bool ShouldLog(LogLevel level) const { return level >= GetLogLevel(); }

  virtual LogLevel GetLogLevel() const { return kInfo; }

  virtual std::string GetName() const = 0;

  static void SetGlobal(Logger &logger) { global_ = &logger; }
  static Logger &GetGlobal();

  std::unique_ptr<Logger> Clone(std::string_view name) const {
    return std::unique_ptr<Logger>(DoClone(name));
  }

 protected:
  virtual Logger *DoClone(std::string_view name) const = 0;
  virtual void DoLog(LogLevel level, std::string_view str) = 0;
};

class UniqueLogger : public Logger {
  std::unique_ptr<Logger> logger_ = nullptr;
  std::string name_;

 public:
  UniqueLogger() = default;
  UniqueLogger(const Logger &log, std::string_view name) {
    SetLogger(log, name);
  };

  LogLevel GetLogLevel() const override {
    if (logger_) return logger_->GetLogLevel();
    return kNone;
  }

  void SetLogger(const Logger &log, std::string_view name) {
    logger_ = log.Clone(name);
    name_ = name;
  }

  Logger *GetLogger() const { return logger_.get(); }

  std::string GetName() const override { return name_; }

  virtual void Flush() override {
    if (logger_) logger_->Flush();
  }

 protected:
  virtual UniqueLogger *DoClone(std::string_view name) const override {
    if (logger_) {
      return new UniqueLogger(*logger_, name);
    }
    return new UniqueLogger();
  }

  virtual void DoLog(LogLevel level, std::string_view str) override {
    if (logger_) logger_->Log(level, str);
  }
};

inline Logger &Logger::GetGlobal() {
  if (global_) {
    return *global_;
  }

  // default logger that does nothing
  static auto default_ = std::make_unique<UniqueLogger>();
  return *default_;
}

}  // namespace log

}  // namespace fmr

#endif /* end of include guard: H_FMR_LOG_LOGGER_H */
