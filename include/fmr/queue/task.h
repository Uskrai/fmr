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

#ifndef FMR_QUEUE_TASK
#define FMR_QUEUE_TASK

#include <fmr/queue/item_receiver.h>

#include <utility>

namespace fmr {

namespace queue {

template <typename ItemType>
class Task {
 public:
  using value_type = ItemType;
  using receiver_type = ItemReceiver<value_type>;

 private:
  receiver_type *receiver_;
  bool stopped_ = false;

 public:
  Task() {}
  Task(receiver_type *receiver) { SetReceiver(receiver); }
  virtual ~Task() {}

  void SetReceiver(receiver_type *receiver) { receiver_ = receiver; }
  void SendItem(value_type &&item) { receiver_->TakeItem(std::move(item)); }

  void Stop(bool stop) { stopped_ = stop; }
  bool IsBeingStopped() const { return stopped_; }

  /**
   * @brief: The method that should be overriden
   *
   * @param: the item that should be processed
   *
   * @return: true if the task is completed
   */
  virtual bool ProcessItem(value_type &item) = 0;
};

}  // namespace queue

}  // namespace fmr

#endif /* end of include guard: FMR_QUEUE_TASK */
