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

#include <algorithm>
#include <memory>
#include <queue>

namespace fmr {

namespace queue {

template <typename T>
class ItemReceiver {
 public:
  using value_type = T;
  virtual void TakeItem(T &&item) = 0;
};

enum EventType { kEventUsePost, kEventUseQueue };

template <typename T, typename SendItemClass>
class Base {
 public:
  using value_type = T;
  using pointer_type = std::unique_ptr<value_type>;
  using Container = typename std::deque<pointer_type>;
  using iterator = typename Container::iterator;
  using send_type = SendItemClass;
  using receiver_type = ItemReceiver<send_type>;

 private:
  Container queue_item_;
  receiver_type *receiver_ = nullptr;
  bool is_being_deleted_ = false;

  EventType event_type_ = kEventUseQueue;

 public:
  Base() {}
  Base(receiver_type *receiver) { SetReceiver(receiver); };
  virtual ~Base() = default;

  void SetReceiver(receiver_type *receiver) { receiver_ = receiver; };
  receiver_type *GetReceiver() { return receiver_; }
  void SendItem(send_type &&item) { GetReceiver()->TakeItem(std::move(item)); }

  void SetEventType(EventType type) { event_type_ = type; }

  /**
   * @brief: to Stop the task if using multi-thread
   */
  [[deprecated("Use Stop")]] virtual void Delete(bool cond = true) {
    Stop(cond);
  }

  /**
   * @see IsBeingStopped
   */
  [[deprecated("Use IsBeingStopped")]] virtual bool IsBeingDeleted() const {
    return IsBeingStopped();
  }

  /**
   * @brief: to Stop the task if using a multi threaded
   */
  virtual void Stop(bool cond = true) { is_being_deleted_ = cond; }

  /**
   * @brief: To check if the queue is being stopped
   */
  virtual bool IsBeingStopped() const { return is_being_deleted_; }

  /**
   * @brief: push item to the queue
   */
  template <typename... U>
  void Push(U &&...item) {
    Push(std::make_unique<value_type>(std::forward<U>(item)...));
  }

  void Push(pointer_type item) {
    return GetContainer().push_back(std::move(item));
  }

  /**
   * @brief: Push Item to the front of the queue
   */
  template <typename... U>
  void PushFront(U &&...item) {
    return PushFront(std::make_unique<value_type>(std::forward<U>(item)...));
  }

  void PushFront(pointer_type item) {
    return GetContainer().push_front(std::move(item));
  }

  /**
   * @brief: Make item to the front of the queue
   *
   * @param: item the item that need to be moved
   *
   * @return: true if the item exist within the queue
   */
  bool MakeFront(const value_type &item) {
    auto it = FindIterator(item);
    if (it == GetContainer().end()) return false;

    pointer_type ptr = std::move(*it);
    GetContainer().erase(it);
    PushFront(std::move(ptr));
    return true;
  }

  /**
   * @brief: access the first element
   * @return: reference to the first element in the container
   */
  pointer_type &Front() { return GetContainer().front(); }
  /**
   * @brief: remove the first elemt
   */
  void Pop() { return GetContainer().pop_front(); }

  /**
   * @brief: check whether the container is empty
   * @return: true if container is empty
   */
  bool IsEmpty() const { return GetContainer().empty(); }

  /**
   * @brief: The method that should be overriden
   *
   * @param: the item that should be processed
   *
   * @return: true if the task should be removed from queue
   */
  virtual bool ProcessTask(value_type &item) = 0;

  /**
   * @brief: Process front Queue
   * @return: @see ProcessTask
   */
  virtual bool PopTask() {
    if (ProcessTask(*Front())) {
      Pop();
      return true;
    }
    return false;
  };

  /**
   * @brief: Return the size of the container
   * @return:
   */
  size_t Size() { return GetContainer().size(); }

  /**
   * @brief: Clear the container
   */
  virtual void ClearTask() { GetContainer().clear(); }

 protected:
  const Container &GetContainer() const { return queue_item_; }
  Container &GetContainer() { return queue_item_; }

  /**
   * @brief: Compare function to compare value_type
   * override this if value_type doesnt support operator==
   * @param: a value from queue
   * @param: b value from caller
   * @return: true if a and b equal
   */
  virtual bool Compare(const value_type &a, const value_type &b) {
    return a == b;
  }

  iterator FindIterator(const value_type &item) {
    return std::find_if(GetContainer().begin(), GetContainer().end(),
                        [&](const pointer_type &compare) {
                          return compare && Compare(*compare, item);
                        });
  }
};

}  // namespace queue

}  // namespace fmr

#endif /* end of include guard: FMR_QUEUE_BASE */
