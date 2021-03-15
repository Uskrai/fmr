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

#include <fmr/queue/base.h>
#include <fmr/queue/task.h>
#include <fmr/thread/queue.h>

#include <algorithm>
#include <forward_list>
#include <memory>

namespace fmr {
namespace thread {

enum QueuePushType { kQueuePushFront, kQueuePushBack };

template <typename ItemType>
class QueueThreadCtrl : public ThreadController {
 public:
  using value_type = ItemType;
  using queue_type = queue::Base<value_type>;
  using QueueClass = queue::Base<ItemType>;
  using task_type = queue::Task<value_type>;
  using ThreadClass = Queue<ItemType>;

 private:
  wxEvtHandler *parent_ = nullptr;
  int event_id_ = wxID_ANY;
  std::unique_ptr<QueueClass> queue_;
  std::unique_ptr<task_type> task_;
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
    SetQueue(std::make_unique<QueueClass>());
    Bind(kEventThreadUpdate, &QueueThreadCtrl::OnThreadUpdate, this);
    Bind(kEventThreadCompleted, &QueueThreadCtrl::OnThreadCompleted, this);
  }

  virtual ~QueueThreadCtrl() {
    Clear();
    ClearThread();
  }

  void SetQueue(std::unique_ptr<QueueClass> queue) {
    queue_ = std::move(queue);
  }

  void SetTask(std::unique_ptr<task_type> task) {
    task_ = std::move(task);
    for (auto &it : thread_list_)
      if (it) it->SetTask(task_.get());
  }

  template <typename Type, typename... U>
  Type *CreateTask(U &&...u) {
    SetTask(std::make_unique<Type>(std::forward<U>(u)...));
    return static_cast<Type *>(task_.get());
  }

  QueueClass *GetQueue() { return queue_.get(); }
  const QueueClass *GetQueue() const { return queue_.get(); }

  wxCriticalSection &GetLock() { return lock_; }

  virtual wxEvtHandler *GetParent() override { return parent_; }
  void SetParent(wxEvtHandler *parent) { parent_ = parent; }

  void SetConcurrency(size_t limit) {
    thread_concurrency_ = limit;
    PrepareThread();
  }
  size_t GetConcurrency() const { return thread_concurrency_; }

  void SetAutoRun(bool cond) { is_auto_run_ = cond; }
  bool IsAutoRun() const { return is_auto_run_; }

  bool IsRunning() const { return CountRunning() != 0; }

  bool IsEmpty() const {
    std::scoped_lock locker(queue_mutex_);

    auto ret = GetQueue()->IsEmpty();
    for (const auto &it : thread_list_) {
      if (it && !it->IsEmpty()) ret = false;
    }

    return ret;
  }

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

  int GetEventId() const { return event_id_; }
  virtual void SetEventId(int id) { event_id_ = id; }

  void Push(value_type item) { DoPush(std::move(item), kQueuePushBack); }

  void PushFront(value_type item) { DoPush(std::move(item), kQueuePushFront); }

  bool MakeFront(const value_type &item) {
    std::scoped_lock locker(GetQueueMutex());
    return GetQueue()->MakeFront(item);
  }

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
   * get the thread from this first if this method is overriden by derived
   * class
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
    thread->SetTask(task_.get());
    thread->Run();
    thread_list_.push_front(thread);
  }

  void ClearThread() {
    for (ThreadClass *&it : thread_list_) {
      if (it) it->SetDisableQueueWait();
    }

    queue_wait_.notify_all();
    for (auto &it : thread_list_) {
      queue_wait_.notify_all();
      if (it) {
        auto ret = Delete(it, GetLock());
        if (ret != wxTHREAD_NO_ERROR) it = nullptr;
      }
    }

    for (auto &it : thread_list_) {
      queue_wait_.notify_all();
      Wait(it, GetLock());
    }

    std::scoped_lock locker(GetQueueMutex());
    thread_list_.clear();
  }

  virtual void Clear() {
    std::scoped_lock locker(GetQueueMutex());
    struct task_stopper {
      task_type *task_;
      task_stopper(task_type *task) : task_(task) { task_->Stop(true); }
      ~task_stopper() { task_->Stop(false); }
    };

    task_stopper stopper(task_.get());
    queue_->ClearTask();

    // wait for thread to go to waiting state
    for (const auto &it : thread_list_) {
      while (it && it->IsOnTask()) {
      }
    }
  }

  void DoSetNull(BaseThread *thread) override {
    std::scoped_lock locker(GetQueueMutex());
    for (auto &it : thread_list_) {
      if (it == thread) {
        it = nullptr;
      }
    }
  }

  virtual void DoPush(value_type item, QueuePushType type) {
    if (type == kQueuePushBack)
      GetQueue()->Push(std::move(item));
    else if (type == kQueuePushFront)
      GetQueue()->PushFront(std::move(item));
    queue_wait_.notify_one();
    if (IsAutoRun()) Run();
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
    if (IsEmpty()) {
      Completed(GetEventId());
    }
    event.Skip();
  }

  void OnThreadCompleted(wxThreadEvent &event) {}
};

}  // namespace thread

}  // namespace fmr

#endif /* end of include guard: FMR_THREAD_QUEUE_CTRL */
