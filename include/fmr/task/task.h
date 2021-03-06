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

#ifndef INCLUDE_FMR_TASK_TASK_H_
#define INCLUDE_FMR_TASK_TASK_H_

#include <fmr/task/state.h>

#include <atomic>

namespace fmr {

namespace task {

class Task {
 public:
  virtual void Next() = 0;
  virtual bool HasNext() const = 0;
  virtual ~Task() {}
};

}  // namespace task

}  // namespace fmr

#endif  // INCLUDE_FMR_TASK_TASK_H_
