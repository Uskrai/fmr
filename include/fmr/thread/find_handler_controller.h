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

#ifndef FMR_THREAD_FIND_HANDLER_CONTROLLER
#define FMR_THREAD_FIND_HANDLER_CONTROLLER

#include <queue>
#include <unordered_map>

#include "fmr/handler/abstract_handler.h"
#include "fmr/thread/find_handler.h"
#include "fmr/thread/thread.h"

namespace fmr {

namespace thread {

constexpr int FindHandlerControllerIdDefault = wxID_HIGHEST + 3010;

class FindHandlerController : public ThreadController {
 protected:
  bool (*stream_checker_)(const SStream &stream);
  std::unique_ptr<AbstractHandler> handler_;
  std::queue<const SStream *> stream_queue_;
  std::vector<const SStream *> in_queue_vec_;
  FindHandler *thread_ = nullptr;
  FindHandlerFlags thread_flags_ = kFindHandlerDefault;

  std::unordered_map<const SStream *, const SStream *> found_source_map_;
  std::vector<std::unique_ptr<SStream>> loaded_stream_;

  wxEvtHandler *parent_ = nullptr;
  wxCriticalSection lock_;

  int thread_id_ = FindHandlerControllerIdDefault;

 public:
  FindHandlerController(wxEvtHandler *parent,
                        int id = FindHandlerControllerIdDefault);
  virtual ~FindHandlerController() { Clear(); }
  bool Open(const std::string &path);
  void Push(const SStream *stream) { stream_queue_.push(stream); };
  bool Run();

  wxThread *GetThread(int id);
  void DoSetNull(int id);
  wxEvtHandler *GetParent() { return parent_; }

  void SetChecker(bool (*checker)(const SStream &stream)) {
    stream_checker_ = checker;
  }
  void SetFlags(FindHandlerFlags flags) { thread_flags_ = flags; }
  void SetThreadId(int thread_id);
  int GetThreadId() const { return thread_id_; }

  const SStream *GetSourceStream(const SStream *source_stream);

  bool IsInQueue(const SStream *stream) const;

  void Clear();

 private:
  void OnStreamFound(FoundEvent &event);
};

}  // namespace thread

}  // namespace fmr

#endif /* end of include guard: FMR_THREAD_FIND_HANDLER_CONTROLLER */
