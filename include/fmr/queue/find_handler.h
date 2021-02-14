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
#include <fmr/handler/abstract_handler.h>
#include <fmr/handler/handler_factory.h>
#include <fmr/handler/struct_stream.h>
#include <fmr/thread/thread.h>

#include <queue>

#include "fmr/queue/base.h"

namespace fmr {

namespace queue {

/// flags used by FindHandler
enum FindHandlerFlags {
  kFindHandlerDefault = 0x00,
  kFindHandlerRecursive = 0x01,
  kFindHandlerCheckHandler = 0x02,
  kFindHandlerOnlyFirstItem = 0x08,
  kFindHandlerDontRecursiveNonOpenable = 0x10,
  kFindHandlerCheckFilenameIfOpenable = 0x20
};
DEFINE_BITMASK_TYPE(FindHandlerFlags);

enum FindStatus {
  kFindItemFound = 0,
  kFindBeingStopped = 1,
  kFindNotFound = 2
};

[[deprecated("Changed to FindStatus")]] typedef FindStatus FindReturn;

class FindHandler;
class FindHandlerChecker {
 public:
  virtual FindStatus Check(FindHandler &parent,
                           AbstractOpenableHandler &handler, SStream &stream) {
    return Check(parent, static_cast<AbstractHandler &>(handler), stream);
  };
  virtual FindStatus Check(FindHandler &parent, AbstractHandler &handler,
                           SStream &stream) = 0;
};

class FindItem {
  SStream found_stream_;
  const SStream *source_stream_;
  FindStatus status_;

 public:
  FindItem(const SStream *source_stream, SStream found_stream) {
    SetFoundStream(std::move(found_stream));
    SetSourceStream(source_stream);
  };
  FindItem(const FindItem &item) = default;
  FindItem(FindItem &&item) = default;

  SStream &GetFoundStream() { return found_stream_; }
  const SStream *GetSourceStream() { return source_stream_; }
  FindStatus GetStatus() { return status_; }

  void SetFoundStream(SStream stream) { found_stream_ = std::move(stream); }
  void SetSourceStream(const SStream *stream) { source_stream_ = stream; }
  void SetStatus(FindStatus status) { status_ = status; }
};

class FindHandler : public Base<const SStream *, FindItem> {
 private:
  FindHandlerFlags flags_;

  FindHandlerChecker *checker_ = nullptr;

 public:
  FindHandler() {}
  FindHandler(receiver_type *receiver) : Base(receiver){};

  FindStatus Find(FindItem &item);
  FindStatus Find(AbstractOpenableHandler *handler, FindItem &item);
  FindStatus Find(AbstractHandler *handler, FindItem &item);

  void SetChecker(FindHandlerChecker *checker) { checker_ = checker; }
  FindHandlerChecker *GetChecker() { return checker_; }

  void SendFoundEvent(FindItem &item);

  // FindStatus CheckAndSendIfFound(AbstractHandler *handler, FoundEvent
  // *event);

  /**
   * @brief: flags for the thread
   * kFindHandlerOnlyFirstItem will only send the first item found with
   * CheckStream. kFindHandlerCheckHandler will check the handler stream, use
   * this for searching handler like archive or directory,.
   * kFindHandlerRecursive will check the handler recursively, if the first
   * level contain Non openable handler, it will be checked too.
   */
  void SetFlags(FindHandlerFlags flags) { flags_ = flags; }

  bool ProcessTask(value_type &item);

 private:
  bool Is(FindHandlerFlags flags) { return flags_ & flags; }

  // void StreamFound(FoundEvent *stream);
  template <typename T>
  FindStatus CheckAndSendIfFound(T *handler, FindItem &item);

  template <typename T>
  FindStatus TraverseHandler(T *handler, FindItem &item);
};

};  // namespace queue

};  // namespace fmr

#endif
