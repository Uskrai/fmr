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

#ifndef FMR_QUEUE_BASE
#define FMR_QUEUE_BASE

#include <wx/event.h>

#include <queue>

namespace fmr {

namespace queue {

enum EventType { kEventUsePost, kEventUseQueue };

template <class T>
class Base {
  std::queue<T> queue_item_;
  wxEvtHandler *parent_ = nullptr;
  bool is_being_deleted_ = false;
  int event_id_ = wxID_ANY;

  EventType event_type_ = kEventUseQueue;

 public:
  Base(wxEvtHandler *parent, int id) {
    parent_ = parent;
    event_id_ = id;
  };

  using value_type = T;

  const std::queue<T> &GetContainer() const { return queue_item_; }
  std::queue<T> &GetContainer() { return queue_item_; }

  wxEvtHandler *GetParent() { return parent_; }
  int GetEventId() const { return event_id_; }

  void SendEventToParent(wxEvent *event) {
    if (event_type_ == kEventUsePost) {
      wxPostEvent(GetParent(), *event);
      delete event;
    }
    if (event_type_ == kEventUseQueue) return wxQueueEvent(GetParent(), event);
  }

  void QueueEventToParent(wxEvent *event) = delete;

  void PostEventToParent(wxEvent *event) = delete;

  void SetEventType(EventType type) { event_type_ = type; }

  virtual void Delete() { is_being_deleted_ = true; }
  virtual bool IsBeingDeleted() const { return is_being_deleted_; }

  void Push(const T &item) { return queue_item_.push(item); }
  void Push(T &&item) { return queue_item_.push(item); }

  T &Front() { return queue_item_.front(); }
  void Pop() { return queue_item_.pop(); }

  bool IsEmpty() const { return queue_item_.empty(); }

  /**
   * @brief: The method that should be overriden
   *
   * @param: the item that should be processed
   *
   * @return: true if the task should be removed from queue if called from
   * PopTask
   */
  virtual bool ProcessTask(value_type &item) = 0;

  /**
   * @brief: Process front Queue
   * @return: @see ProcessTask
   */
  virtual bool PopTask() {
    if (ProcessTask(queue_item_.front())) {
      queue_item_.pop();
      return true;
    }
    return false;
  };

  size_t Size() { return queue_item_.size(); }

  virtual void ClearTask() {
    while (!queue_item_.empty()) queue_item_.pop();
  }
};

}  // namespace queue

}  // namespace fmr

#endif /* end of include guard: FMR_QUEUE_BASE */
