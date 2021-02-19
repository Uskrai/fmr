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

#ifndef FMR_POSITION_BOX_CTRL_FLAG
#define FMR_POSITION_BOX_CTRL_FLAG

#include <fmr/common/bitmask.h>

namespace fmr {

namespace position {

enum BoxFlags {
  kBoxVertical = 0x01,
  kBoxHorizontal = 0x02,
  kBoxAlignCenter = 0x04
};

DEFINE_BITMASK_TYPE(BoxFlags);

}  // namespace position

}  // namespace fmr

#endif /* end of include guard: FMR_POSITION_BOX_CTRL_FLAG */
