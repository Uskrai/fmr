/*
 *  Copyright (c) 2020-2021 Uskrai
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

#ifndef FMR_THREAD_FIND_HANDLER
#define FMR_THREAD_FIND_HANDLER

#include <fmr/common/bitmask.h>
#include <fmr/file_handler/factory.h>
#include <fmr/log/logger.h>
#include <fmr/queue/find_handler_checker.h>
#include <fmr/queue/find_handler_flags.h>
#include <fmr/queue/find_handler_item.h>
#include <fmr/queue/task.h>

#include <queue>

#include "fmr/queue/base.h"

namespace fmr {

namespace queue {

class FindHandler : public Task<FindItem> {
  log::UniqueLogger log_{log::Logger::GetGlobal(), "Find"};

 private:
  FindHandlerFlags flags_ = kFindHandlerDefault;
  const file_handler::Factory *factory_ = file_handler::Factory::GetGlobal();

  FindHandlerChecker *checker_ = nullptr;

 public:
  FindHandler(const file_handler::Factory *factory) : factory_(factory) {}
  FindHandler(const file_handler::Factory *factory, receiver_type *receiver)
      : Task(receiver), factory_(factory){};

  void SetHandlerFactory(const file_handler::Factory *factory) {
    factory_ = factory;
  }

  log::Logger &GetLogger() { return log_; }
  void SetLogger(const log::Logger &log, std::string_view name) {
    log_.SetLogger(log, name);
  }

  FindStatus Find(FindItem &item);

  void SetChecker(FindHandlerChecker *checker) { checker_ = checker; }
  FindHandlerChecker *GetChecker() { return checker_; }

  void SendFoundEvent(FindItem &item);

  /**
   * @brief: flags for the thread
   * kFindHandlerOnlyFirstItem will only send the first item found with
   * CheckStream. kFindHandlerCheckHandler will check the handler stream, use
   * this for searching handler like archive or directory,.
   * kFindHandlerRecursive will check the handler recursively, if the first
   * level contain Non openable handler, it will be checked too.
   */
  void SetFlags(FindHandlerFlags flags) { flags_ = flags; }

  bool ProcessItem(value_type &item) override;

 private:
  bool Is(FindHandlerFlags flags) { return flags_ & flags; }

  FindStatus CheckAndSendIfFound(FindItem &item);
  FindStatus TraverseHandler(file_handler::Handler &handler, FindItem &item);
};

}  // namespace queue

}  // namespace fmr

#endif
