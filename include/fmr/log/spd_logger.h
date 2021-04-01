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

#ifndef H_FMR_LOG_SPDLOG_H
#define H_FMR_LOG_SPDLOG_H

#include <fmr/log/logger.h>
#include <spdlog/common.h>
#include <spdlog/logger.h>

namespace fmr {

namespace log {

class SpdLogger : public Logger {
  std::shared_ptr<spdlog::logger> logger_;

 public:
  SpdLogger(std::shared_ptr<spdlog::logger> logger) : logger_(logger) {}

  virtual Logger::LogLevel GetLogLevel() const override {
    switch (logger_->level()) {
      case spdlog::level::off:
        return Logger::kNone;
      case spdlog::level::trace:
        return Logger::kTrace;
      case spdlog::level::debug:
        return Logger::kDebug;
      case spdlog::level::info:
        return Logger::kInfo;
      case spdlog::level::warn:
        return Logger::kWarn;
      case spdlog::level::critical:
        return Logger::kCritical;
      case spdlog::level::err:
        return Logger::kError;
      default:
        return Logger::kNone;
    }
  }

  std::string GetName() const override { return logger_->name(); }

  virtual void Flush() override { return logger_->flush(); }

 protected:
  void DoLog(LogLevel level, std::string_view str) override {
    switch (level) {
      case Logger::kDebug:
        logger_->debug(str);
        break;
      case Logger::kInfo:
        logger_->info(str);
        break;
      case Logger::kWarn:
        logger_->warn(str);
        break;
      case Logger::kError:
        logger_->error(str);
      default:
        break;
    }
  }

  virtual SpdLogger* DoClone(std::string_view name) const override {
    return new SpdLogger(logger_->clone(std::string(name)));
  }
};

}  // namespace log

}  // namespace fmr

#endif /* end of include guard: H_FMR_LOG_SPDLOG_H */
