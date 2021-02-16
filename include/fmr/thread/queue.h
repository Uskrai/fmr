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

#include <condition_variable>
#include <mutex>

#include "fmr/thread/thread.h"

namespace fmr {

namespace thread {

template <class QueueClass>
class Queue : public BaseThread {
  bool is_disable_on_empty_queue_ = false, disable_queue_wait_ = false;
  bool is_on_task_ = false;
  bool is_waiting_queue_ = false;

  QueueClass *queue_ = nullptr;

  std::condition_variable *queue_wait_ = nullptr;
  std::mutex *queue_mutex_ = nullptr;

 public:
  Queue(ThreadController *parent, wxThreadKind type, int event_id)
      : BaseThread(parent, type, event_id) {}

  ExitCode Entry() {
    while (!TestDestroy()) {
      WaitForQueue();

      Lock();

      if (!GetQueue()->IsEmpty() && !TestDestroy()) {
        value_type item = std::move(GetQueue()->Front());
        GetQueue()->Pop();

        Unlock();

        SetOnTask(true);
        GetQueue()->ProcessTask(item);
        SetOnTask(false);

        Update();

        // make sure current thread not leave critical section from another
        // thread
        Lock();
      }
      Unlock();
    }
    Completed();

    return (wxThread::ExitCode)0;
  }

  QueueClass *GetQueue() { return queue_; }
  const QueueClass *GetQueue() const { return queue_; }

  void SetQueue(QueueClass *queue) { queue_ = queue; }

  void Lock() {
    if (queue_mutex_) queue_mutex_->lock();
  }
  void Unlock() {
    if (queue_mutex_) queue_mutex_->unlock();
  }

  void SetQueueLocker(std::mutex *mutex, std::condition_variable *cond) {
    queue_mutex_ = mutex;
    queue_wait_ = cond;
  }

  void WaitForQueue() {
    if (!IsDisableQueueWait() && GetQueue()->IsEmpty()) {
      if (queue_wait_ && queue_mutex_) {
        SetWaitingQueue(true);
        std::unique_lock<std::mutex> locker(*queue_mutex_);
        queue_wait_->wait(locker);
        SetWaitingQueue(false);
      }
    }
  }

  bool IsOnTask() const { return is_on_task_; }
  bool IsEmpty() { return GetQueue()->IsEmpty() && !IsOnTask(); }
  bool IsWaitingQueue() const { return is_waiting_queue_; }

  using value_type = typename QueueClass::value_type;

  void Push(const value_type &item) { GetQueue()->Push(item); }
  void Push(value_type &&item) { GetQueue()->Push(std::move(item)); }

  void DisableOnEmptyQueue(bool cond = true) {
    is_disable_on_empty_queue_ = cond;
  }

  bool IsDisableQueueWait() { return disable_queue_wait_; }
  void SetDisableQueueWait(bool cond = true) { disable_queue_wait_ = cond; }

  bool TestDestroy() {
    return BaseThread::TestDestroy() ||
           (is_disable_on_empty_queue_ && IsEmpty());
  }

  wxThreadError Delete(ExitCode *rc = NULL,
                       wxThreadWait waitMode = wxTHREAD_WAIT_DEFAULT) {
    return BaseThread::Delete();
  }

 private:
  void SetOnTask(bool cond = true) { is_on_task_ = cond; }
  void SetWaitingQueue(bool cond) { is_waiting_queue_ = cond; }
};

}  // namespace thread

}  // namespace fmr

#endif /* end of include guard: FMR_THREAD_QUEUE */
