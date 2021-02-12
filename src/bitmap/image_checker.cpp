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

#include "fmr/bitmap/image_checker.h"

#include "fmr/bitmap/image_util.h"
#include "fmr/queue/find_handler.h"

namespace fmr {

namespace bitmap {

queue::FindHandlerCheckStatus ImageChecker::Check(const SStream &stream) const {
  if (image_util::CanRead(stream)) {
    return queue::kCheckStatusCanRead;
  }
  return queue::kCheckStatusCannotRead;
}

queue::FindHandlerCheckStatus ImageChecker::Check(
    const std::string &filename) const {
  if (image_util::CanRead(filename)) {
    return queue::kCheckStatusCanRead;
  }
  return queue::kCheckStatusCannotRead;
}

}  // namespace bitmap

}  // namespace fmr
