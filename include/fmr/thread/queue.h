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

#ifndef FMR_THREAD_QUEUE
#define FMR_THREAD_QUEUE

#include "fmr/thread/thread.h"

namespace fmr {

namespace thread {

template <class QueueClass>
class Queue : public BaseThread {
  bool is_disable_on_empty_queue_;
  QueueClass *queue_ = nullptr;
  wxCriticalSection *queue_lock_ = nullptr;

 public:
  Queue(ThreadController *parent, wxThreadKind type, int event_id,
        wxCriticalSection *queue_lock = nullptr)
      : BaseThread(parent, type, event_id) {
    queue_lock_ = queue_lock;
  }

  ExitCode Entry() {
    while (!TestDestroy()) {
      if (!GetQueue()->IsEmpty()) {
        if (queue_lock_) queue_lock_->Enter();
        value_type item = std::move(GetQueue()->Front());
        GetQueue()->Pop();
        if (queue_lock_) queue_lock_->Leave();
        GetQueue()->ProcessTask(item);
        Update();
      }
    }
    Completed();

    return (wxThread::ExitCode)0;
  }

  QueueClass *GetQueue() { return queue_; }
  const QueueClass *GetQueue() const { return queue_; }

  void SetQueue(QueueClass *queue) { queue_ = queue; }

  void SetQueueLocker(wxCriticalSection *queue_lock) {
    queue_lock_ = queue_lock;
  }

  using value_type = typename QueueClass::value_type;

  void Push(const value_type &item) { GetQueue()->Push(item); }

  void Push(value_type &&item) { GetQueue()->Push(std::move(item)); }

  void DisableOnEmptyQueue(bool cond = true) {
    is_disable_on_empty_queue_ = cond;
  }

  bool TestDestroy() {
    return BaseThread::TestDestroy() ||
           (is_disable_on_empty_queue_ && GetQueue()->IsEmpty());
  }

  wxThreadError Delete(ExitCode *rc = NULL,
                       wxThreadWait waitMode = wxTHREAD_WAIT_DEFAULT) {
    GetQueue()->Delete();
    return BaseThread::Delete();
  }
};

}  // namespace thread

}  // namespace fmr

#endif /* end of include guard: FMR_THREAD_QUEUE */
