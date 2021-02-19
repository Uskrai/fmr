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

#include "fmr/bitmap/rescaler.h"
#include "fmr/position/box_ctrl_flags.h"

namespace fmr {

namespace reader {

class Settings {
 public:
  bool read_from_right_ = false;
  size_t image_per_page_ = 1;
  bitmap::RescalerFlags rescale_flags_ = bitmap::kRescaleNone;
  position::BoxFlags position_flags_ =
      position::kBoxAlignCenter | position::kBoxVertical;
};

}  // namespace reader

}  // namespace fmr
