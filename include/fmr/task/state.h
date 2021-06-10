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

#ifndef INCLUDE_FMR_TASK_STATE_H_
#define INCLUDE_FMR_TASK_STATE_H_

#include <atomic>
namespace fmr {

namespace task {

class State {
  std::atomic_bool pause_, done_;
  State *parent_;

 public:
  State() : State(this) {}

  /**
   * Construct State with parent as the Parent
   *
   * The parent should not be null or destructed before this object
   * The parent is used to check whether the State is paused or not
   */
  State(State *state) : parent_(state) {}
  virtual ~State() {}

  State(State &state) { Copy(state); }
  State(State &&state) { Copy(state); }
  State &operator=(State &state) {
    Copy(state);
    return *this;
  }
  State &operator=(State &&state) {
    Copy(state);
    return *this;
  }
  /**
   * Pause task
   *
   * this wont affect anything if State has Parent
   */
  void Pause(bool pause) { pause_ = pause; }

  /**
   * Check if the State is Paused
   */
  bool IsPaused() const { return parent_->pause_; }

  /**
   * Check if the State is done
   */
  bool IsDone() const { return done_; }

 protected:
  /**
   * Set the State as done
   */
  void Done(bool done) { done_ = done; }

 private:
  void Copy(State &state) {
    pause_ = state.IsPaused();
    parent_ = state.parent_;
  }
};

}  // namespace task

}  // namespace fmr

#endif  // INCLUDE_FMR_TASK_STATE_H_
