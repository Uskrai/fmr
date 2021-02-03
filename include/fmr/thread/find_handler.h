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

#include "fmr/thread/queue.h"

namespace fmr {

namespace thread {

/// flags used by FindHandler
enum FindHandlerFlags {
  kFindHandlerDefault = 0x00,
  kFindHandlerRecursive = 0x01,
  kFindHandlerCheckHandler = 0x02,
  kFindHandlerOnlyFirstItem = 0x08,
  kFindHandlerDontRecursiveNonOpenable = 0x10
};
DEFINE_BITMASK_TYPE(FindHandlerFlags);

class FoundEvent : public wxCommandEvent {
 protected:
  const SStream *source_stream_ = nullptr;
  std::unique_ptr<SStream> found_stream_;

 public:
  FoundEvent(wxEventType type, int id) : wxCommandEvent(type, id) {}

  // Copy Construct will copy the whole found stream so be careful using this
  // event with wxPostEvent
  FoundEvent(const FoundEvent &event);
  wxEvent *Clone() const override { return new FoundEvent(*this); }

  std::unique_ptr<SStream> GetFoundStreamOwnerShip() {
    return std::move(found_stream_);
  }

  SStream *GetFoundStream() { return found_stream_.get(); }
  const SStream *GetSourceStream() { return source_stream_; }

  void SetSourceStream(const SStream *stream) { source_stream_ = stream; }
  void SetFoundStream(std::unique_ptr<SStream> &&stream) {
    found_stream_ = std::move(stream);
  }
};

wxDECLARE_EVENT(kEventStreamFound, FoundEvent);
wxDECLARE_EVENT(kEventStreamNotFound, FoundEvent);
typedef void (wxEvtHandler::*FoundEventFunction)(FoundEvent &);
#define FoundEventHandler(func) wxEVENT_HANDLER_CAST(FoundEventFunction, func);

class FindHandler : public Queue<std::pair<const SStream *, SStream>> {
 private:
  bool (*check_func_)(const SStream &stream);  // function to check if the
                                               // thread should send FoundEvent
  FindHandlerFlags flags_;

 public:
  FindHandler(ThreadController *parent, wxThreadKind type, int id)
      : Queue(parent, type, id){};

  bool Find(FoundEvent *event);
  bool Find(AbstractOpenableHandler *handler, FoundEvent *event);
  bool Find(AbstractHandler *handler, FoundEvent *event);

  void SetChecker(bool (*check_func)(const SStream &stream)) {
    check_func_ = check_func;
  }

  bool SendIfFound(FoundEvent *event);
  bool CheckStream(const SStream &stream) { return check_func_(stream); }

  /**
   * @brief: flags for the thread
   * kFindHandlerOnlyFirstItem will only send the first item found with
   * CheckStream. kFindHandlerCheckHandler will check the handler stream, use
   * this for searching handler like archive or directory,.
   * kFindHandlerRecursive will check the handler recursively, if the first
   * level contain Non openable handler, it will be checked too.
   */
  void SetFlags(FindHandlerFlags flags) { flags_ = flags; }

  using Queue::Push;
  bool Push(const SStream *stream);

 private:
  std::unique_ptr<FoundEvent> MakeEvent(
      wxEventType type, int id, const SStream *source,
      std::unique_ptr<SStream> &&found_stream) {
    auto ret = std::make_unique<FoundEvent>(type, id);
    ret->SetSourceStream(source);
    ret->SetFoundStream(std::move(found_stream));
    return ret;
  }
  bool Is(FindHandlerFlags flags) { return flags_ & flags; }

  void StreamFound(FoundEvent *stream);
  template <typename T>
  bool TraverseHandler(T *handler, FoundEvent *stream);

  ExitCode Entry();
};

};  // namespace thread

};  // namespace fmr

#endif
