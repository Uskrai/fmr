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

#include "fmr/position/grid_ctrl.h"

#include "fmr/common/dimension.h"

namespace fmr {

namespace position {

GridCtrl::GridCtrl() {}

void GridCtrl::CalculatePosition(const PositionVector &vec_item) const {
  dimension::Orientation direction = dimension::kHorizontal;
  dimension::Orientation non_direction = dimension::kVertical;

  auto min_item_size = GetMinimumItemSize(vec_item);

  int direction_pos = 0;
  int direction_largest = 0;
  int non_direction_pos = 0;
  int non_direction_max = 0;
  int min_size = dimension::GetSize(GetMinimumSize(), direction);
  int start_pos = 0;

  if (CheckFlags(kPositionAlignCenter)) {
    auto min_item_size =
        dimension::GetSize(GetMinimumItemSize(vec_item), direction);

    if (min_item_size < min_size) {
      start_pos = min_size / 2 - min_item_size / 2;
    }
  }

  for (auto &it : vec_item) {
    int curr_direction_size =
        dimension::GetSize(*it, direction) + GetBorderSize(direction);

    if (direction_pos + curr_direction_size >= min_size) {
      non_direction_pos += non_direction_max + GetBorderSize(non_direction);
      non_direction_max = 0;
      direction_pos = start_pos;
    }

    dimension::SetPoint(*it, direction, direction_pos);
    dimension::SetPoint(*it, non_direction, non_direction_pos);

    direction_pos +=
        dimension::GetSize(*it, direction) + GetBorderSize(direction);

    non_direction_max =
        std::max(dimension::GetSize(*it, non_direction), non_direction_max);
  }
}

wxSize GridCtrl::GetMinimumItemSize(const PositionVectorConst &vec_item) const {
  wxSize size;
  dimension::Orientation direction = dimension::kHorizontal,
                         non_direction = dimension::kVertical;

  int direction_pos = 0, direction_max = 0;
  int non_direction_pos = 0, non_direction_max = 0;
  int min_size = dimension::GetSize(GetMinimumSize(), direction);

  for (auto &it : vec_item) {
    int curr_direction_size =
        dimension::GetSize(*it, direction) + GetBorderSize(direction);

    if (direction_pos + curr_direction_size >= min_size) {
      non_direction_pos += non_direction_max + GetBorderSize(non_direction);
      non_direction_max = 0;
      direction_max = std::max(direction_pos, direction_max);
      direction_pos = 0;
    }

    direction_pos +=
        dimension::GetSize(*it, direction) + GetBorderSize(direction);
    non_direction_max =
        std::max(dimension::GetSize(*it, non_direction), non_direction_max);
  }

  dimension::SetSize(size, direction, direction_max);
  dimension::SetSize(size, non_direction, non_direction_pos);
  return size;
}

}  // namespace position

}  // namespace fmr
