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
#include "fmr/queue/factory.h"
#include "fmr/queue/find_handler.h"
#include "fmr/thread/queue.h"
#include "fmr/thread/queue_ctrl.h"

namespace fmr {

namespace thread {

constexpr int FindHandlerControllerIdDefault = wxID_HIGHEST + 3010;

class FindHandlerController : public QueueThreadCtrl<queue::FindHandler> {
 protected:
  bool (*stream_checker_)(const SStream &stream);
  std::unique_ptr<AbstractHandler> handler_;
  std::unique_ptr<queue::ItemReceiverEvent<queue::FindItem>> receiver_;

  std::vector<queue::FindItem> in_queue_vec_;
  // Queue<queue::FindHandler> *thread_ = nullptr;
  // std::unique_ptr<queue::FindHandler> queue_;
  queue::FindHandlerFlags thread_flags_ = queue::kFindHandlerDefault;

  // map to found (first) and source (second) stream
  std::unordered_map<const SStream *, const SStream *> found_source_map_;
  std::vector<std::unique_ptr<SStream>> loaded_stream_;

  wxEvtHandler *parent_ = nullptr;

 public:
  FindHandlerController(wxEvtHandler *parent,
                        int id = FindHandlerControllerIdDefault);
  virtual ~FindHandlerController();
  bool Open(const std::string &path);

  wxEvtHandler *GetParent() override { return parent_; }

  void SetEventId(int id) override;

  [[deprecated("Replaced by SetEventId")]] void SetThreadId(int id) {
    SetEventId(id);
  }
  [[deprecated("Replaced by GetEventId")]] int GetThreadId() const {
    return GetEventId();
  }

  const SStream *GetSourceStream(const SStream *found_stream);

  void AddFoundStream(const SStream *source,
                      std::unique_ptr<SStream> &&found_stream);

  bool IsInQueue(const SStream *stream) const;

  void Clear() override;

 protected:
  void DoPush(value_type item, QueuePushType type) override {
    in_queue_vec_.push_back(item);
    QueueThreadCtrl::DoPush(std::move(item), type);
  }
};

}  // namespace thread

}  // namespace fmr

#endif /* end of include guard: FMR_THREAD_FIND_HANDLER_CONTROLLER */
