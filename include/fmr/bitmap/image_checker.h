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

#ifndef FMR_BITMAP_IMAGE_CHECKER
#define FMR_BITMAP_IMAGE_CHECKER

#include <fmr/queue/find_handler_checker.h>

namespace fmr {

namespace bitmap {

class ImageChecker : public queue::FindHandlerChecker {
 public:
  virtual queue::FindStatus Check(file_handler::ReadStream &stream,
                                  queue::FindHandler *parent) override;
};

}  // namespace bitmap

}  // namespace fmr

#endif /* end of include guard: FMR_BITMAP_IMAGE_CHECKER */
