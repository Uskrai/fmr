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

#ifndef FMR_THREAD_QUEUE_CTRL
#define FMR_THREAD_QUEUE_CTRL

#include <algorithm>
#include <forward_list>
#include <memory>

#include "fmr/thread/queue.h"

namespace fmr {

namespace thread {

template <typename QueueClass>
class QueueThreadCtrl : public ThreadController {
 public:
  using ThreadClass = Queue<QueueClass>;

 private:
  wxEvtHandler *parent_ = nullptr;
  int event_id_ = wxID_ANY;
  std::unique_ptr<QueueClass> queue_;
  wxCriticalSection queue_lock_;
  bool disable_on_empty_queue_ = false;

  std::forward_list<ThreadClass *> thread_list_;
  wxThreadKind thread_type_ = wxTHREAD_DETACHED;

  mutable wxCriticalSection lock_;

  mutable std::mutex queue_mutex_;
  std::condition_variable queue_wait_;

  size_t thread_concurrency_ = 1;
  bool is_auto_run_ = false;

 public:
  QueueThreadCtrl(wxEvtHandler *parent, int id) : ThreadController() {
    SetParent(parent);
    SetEventId(id);
    SetQueue(std::make_unique<QueueClass>(this, id));
    PrepareThread();
    Bind(kEventThreadUpdate, &QueueThreadCtrl::OnThreadUpdate, this);
    Bind(kEventThreadCompleted, &QueueThreadCtrl::OnThreadCompleted, this);
  }
  virtual ~QueueThreadCtrl() {
    Clear();
    ClearThread();
  }

  wxEvtHandler *GetParent() { return parent_; }
  void SetParent(wxEvtHandler *parent) { parent_ = parent; }

  void SetQueue(std::unique_ptr<QueueClass> &&queue) {
    queue_ = std::move(queue);
  }
  QueueClass *GetQueue() { return queue_.get(); }
  const QueueClass *GetQueue() const { return queue_.get(); }

  using value_type = typename QueueClass::value_type;

  void SetConcurrency(size_t limit) {
    thread_concurrency_ = limit;
    PrepareThread();
  }
  size_t GetConcurrency() const { return thread_concurrency_; }

  void SetAutoRun(bool cond) { is_auto_run_ = cond; }
  bool IsAutoRun() const { return is_auto_run_; }

  bool IsRunning() const { return CountRunning() != 0; }

  void SetThreadKind(wxThreadKind type = wxTHREAD_DETACHED) {
    thread_type_ = type;
  }
  wxThreadKind GetThreadKind() const { return thread_type_; }

  [[deprecated("Thread is automatically disabled ")]] void DisableOnEmptyQueue(
      bool cond) {
    disable_on_empty_queue_ = cond;
  }
  [[deprecated("Thread is automatically disabled ")]] bool
  IsDisableOnEmptyQueue() {
    return disable_on_empty_queue_;
  }

  /**
   * @brief: Get Queue's Locker
   * @return: Queue locker
   */
  std::mutex &GetQueueMutex() { return queue_mutex_; }
  // wxCriticalSection &GetQueueLock() { return queue_lock_; }
  wxCriticalSection &GetLock() const { return lock_; }

  int GetEventId() const { return event_id_; }
  virtual void SetEventId(int id) {
    event_id_ = id;
    if (queue_) {
      queue_->SetEventId(id);
    }
  }

  void Push(value_type &&item) {
    OnPush(item);
    queue_->Push(std::move(item));
    queue_wait_.notify_one();
    if (IsAutoRun()) Run();
  }

  void Push(const value_type &item) {
    OnPush(item);
    queue_->Push(item);
    queue_wait_.notify_one();
    if (IsAutoRun()) Run();
  }

  virtual void OnPush(const value_type &item){};

  /**
   * @brief: Count Thread
   * @return: Thread Count
   */
  size_t CountThread() {
    return std::distance(thread_list_.begin(), thread_list_.end());
  }

  size_t CountRunning() const {
    return std::count_if(thread_list_.begin(), thread_list_.end(),
                         [](const void *ptr) { return ptr != nullptr; });
  }

  /**
   * @brief: Create Thread
   * get the thread from this first if this method is overriden by derived class
   * @return: Created Thread
   */
  virtual ThreadClass *CreateThread() {
    auto thread = new ThreadClass(this, GetThreadKind(), GetEventId());
    thread->SetQueue(queue_.get());
    return thread;
  }

  bool Run() {
    std::scoped_lock locker(GetQueueMutex());
    PrepareThread();
    for (auto &it : thread_list_) {
      if (it && !it->IsRunning()) it->Run();
    }
    return true;
  }

  void AddThread(ThreadClass *thread) {
    thread->SetQueue(queue_.get());
    thread->SetQueueLocker(&GetQueueMutex(), &queue_wait_);
    thread->Run();
    thread_list_.push_front(thread);
  }

  void ClearThread() {
    for (auto &it : thread_list_) {
      auto ret = Delete(it, GetLock());
      if (ret != wxTHREAD_NO_ERROR) it = nullptr;
    }

    queue_wait_.notify_all();

    for (auto &it : thread_list_) {
      Wait(it, GetLock());
    }

    std::scoped_lock locker(GetQueueMutex());
    thread_list_.clear();
  }

  virtual void Clear() {
    std::scoped_lock locker(GetQueueMutex());
    queue_->Delete(true);
    queue_->ClearTask();

    // wait for thread to go to waiting state
    for (const auto &it : thread_list_) {
      while (it && it->IsOnTask()) {
      }
    }

    queue_->Delete(false);
  }

  void DoSetNull(BaseThread *thread) {
    std::scoped_lock locker(GetQueueMutex());
    for (auto &it : thread_list_) {
      if (it == thread) {
        it = nullptr;
      }
    }
  }

 private:
  void PrepareThread() {
    while (CountRunning() < GetConcurrency()) AddThread(CreateThread());
    while (CountRunning() > GetConcurrency()) {
      auto thread = thread_list_.front();
      Delete(thread, GetLock());
      thread_list_.pop_front();
    }
  }

  void OnThreadUpdate(wxThreadEvent &event) {
    // check for all thread if any is not empty then return
    for (const auto &it : thread_list_)
      if (it && !it->IsEmpty()) return;

    Completed(GetEventId());

    event.Skip();
  }

  void OnThreadCompleted(wxThreadEvent &event) {}
};

}  // namespace thread

}  // namespace fmr

#endif /* end of include guard: FMR_THREAD_QUEUE_CTRL */
