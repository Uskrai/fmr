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

#include <queue>

#include "fmr/thread/thread.h"

namespace fmr {

namespace thread {

template <class T>
class Queue : public BaseThread {
  std::queue<T> queue_item_;
  bool disable_on_empty_queue_ = false;

 public:
  Queue(ThreadController *parent, wxThreadKind type, int id)
      : BaseThread(parent, type, id){};
  virtual ~Queue(){};

  void Push(const T &item) { queue_item_.push(item); }
  void Push(T &&item) { queue_item_.push(std::move(item)); }

  T &Front() { return queue_item_.front(); }

  T FrontAndPop() {
    T item = Front();
    Pop();
    return item;
  }

  void Pop() { queue_item_.pop(); }

  void DisableOnEmptyQueue(bool disable = true) {
    disable_on_empty_queue_ = disable;
  }

  virtual bool TestDestroy() {
    return BaseThread::TestDestroy() ||
           (disable_on_empty_queue_ && queue_item_.empty());
  }

  virtual bool QueueEmpty() { return queue_item_.empty(); }
};

}  // namespace thread

}  // namespace fmr

#endif /* end of include guard: FMR_THREAD_QUEUE */
