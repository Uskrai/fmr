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

  wxCriticalSection lock_;

  size_t thread_concurrency_ = 1;

 public:
  QueueThreadCtrl(wxEvtHandler *parent, int id) : ThreadController() {
    SetParent(parent);
    SetEventId(id);
    SetQueue(std::make_unique<QueueClass>(this, id));
    Bind(kEventThreadUpdate, &QueueThreadCtrl::OnThreadUpdate, this);
  }
  virtual ~QueueThreadCtrl() { Clear(); }

  wxEvtHandler *GetParent() { return parent_; }
  void SetParent(wxEvtHandler *parent) { parent_ = parent; }

  void SetQueue(std::unique_ptr<QueueClass> &&queue) {
    queue_ = std::move(queue);
  }
  QueueClass *GetQueue() { return queue_.get(); }
  const QueueClass *GetQueue() const { return queue_.get(); }

  using value_type = typename QueueClass::value_type;

  void SetConcurrency(size_t limit) { thread_concurrency_ = limit; }
  size_t GetConcurrency() const { return thread_concurrency_; }

  void SetThreadKind(wxThreadKind type = wxTHREAD_DETACHED) {
    thread_type_ = type;
  }
  wxThreadKind GetThreadKind() const { return thread_type_; }

  void DisableOnEmptyQueue(bool cond) { disable_on_empty_queue_ = cond; }
  bool IsDisableOnEmptyQueue() { return disable_on_empty_queue_; }

  /**
   * @brief: Get Queue's Locker
   * @return: Queue locker
   */
  wxCriticalSection &GetQueueLock() { return queue_lock_; }
  wxCriticalSection &GetLock() { return lock_; }

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
  }

  void Push(const value_type &item) {
    OnPush(item);
    queue_->Push(item);
  }

  virtual void OnPush(const value_type &item){};

  /**
   * @brief: Count Thread
   * @return: Thread Count
   */
  size_t CountThread() {
    return std::distance(thread_list_.begin(), thread_list_.end());
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
    PrepareThread();
    for (auto &it : thread_list_) {
      if (it && !it->IsRunning()) it->Run();
    }
    return true;
  }

  void AddThread(ThreadClass *thread) {
    thread->SetQueue(queue_.get());
    thread->SetQueueLocker(&GetQueueLock());

    thread_list_.push_front(thread);
  }

  void ClearThread() {
    for (auto &it : thread_list_) {
      auto ret = Delete(it, GetLock());
      if (ret != wxTHREAD_NO_ERROR) it = nullptr;
    }

    for (auto &it : thread_list_) {
      Wait(it, GetLock());
    }

    thread_list_.clear();
  }

  virtual void Clear() {
    ClearThread();
    disable_on_empty_queue_ = false;
    queue_->ClearTask();
    queue_->Delete(false);
  }

  void DoSetNull(BaseThread *thread) {
    for (auto &it : thread_list_) {
      if (it == thread) it = nullptr;
    }
  }

 private:
  void PrepareThread() {
    while (CountThread() < GetConcurrency()) AddThread(CreateThread());
  }

  void OnThreadUpdate(wxThreadEvent &event) {
    if (GetQueue()->IsEmpty() && IsDisableOnEmptyQueue()) {
      for (auto &it : thread_list_) {
        if (it) it->DisableOnEmptyQueue();
      }
    }
    event.Skip();
  }
};

}  // namespace thread

}  // namespace fmr

#endif /* end of include guard: FMR_THREAD_QUEUE_CTRL */
