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

class GridCtrlItemHelper {
 public:
  GridCtrlItemHelper(const GridCtrl *parent) {
    min_size = parent->GetMinimumSize();
    border_size = parent->GetBorderSize();
  }

  dimension::Orientation direction, non_direction;
  int direction_pos = 0, direction_max = 0, non_direction_pos = 0,
      non_direction_max = 0, current_direction_pos = 0;
  int start_pos = 0;
  wxSize min_size, min_item_size, border_size;

  void NextItem(PositionItemConst *item);

  bool IsPastSize(PositionItemConst *item);
};

bool GridCtrlItemHelper::IsPastSize(PositionItemConst *item) {
  int curr_direction_size = dimension::GetSize(*item, direction) +
                            dimension::GetSize(border_size, direction);
  return direction_pos + curr_direction_size >=
         dimension::GetSize(min_size, direction);
}

void GridCtrlItemHelper::NextItem(PositionItemConst *item) {
  int curr_direction_size = dimension::GetSize(*item, direction) +
                            dimension::GetSize(border_size, direction);

  if (IsPastSize(item)) {
    non_direction_pos +=
        non_direction_max + dimension::GetSize(border_size, non_direction);

    non_direction_max = 0;

    direction_max = std::max(direction_max, direction_pos);
    direction_pos = start_pos;
  }

  non_direction_max =
      std::max(dimension::GetSize(*item, non_direction), non_direction_max);
}

GridCtrl::GridCtrl() {}

void GridCtrl::CalculatePosition(const PositionVector &vec_item) const {
  GridCtrlItemHelper helper(this);
  helper.direction = dimension::kHorizontal;
  helper.non_direction = dimension::kVertical;
  helper.min_item_size = GetMinimumItemSize(vec_item);

  if (CheckFlags(kPositionAlignCenter)) {
    auto min_item_size =
        dimension::GetSize(GetMinimumItemSize(vec_item), helper.direction);

    int min_size = dimension::GetSize(helper.min_size, helper.direction);

    if (min_item_size < min_size) {
      helper.start_pos = min_size / 2 - min_item_size / 2;
    }
  }

  for (auto &it : vec_item) {
    helper.NextItem(it);
    dimension::SetPoint(*it, helper.direction, helper.direction_pos);
    dimension::SetPoint(*it, helper.non_direction, helper.non_direction_pos);

    helper.direction_pos +=
        dimension::GetSize(*it, helper.direction) +
        dimension::GetSize(helper.border_size, helper.direction);
  }
}

wxSize GridCtrl::GetMinimumItemSize(const PositionVectorConst &vec_item) const {
  wxSize size;

  GridCtrlItemHelper helper(this);
  helper.direction = dimension::kHorizontal;
  helper.non_direction = dimension::kVertical;

  for (auto &it : vec_item) {
    helper.NextItem(it);

    helper.direction_pos +=
        dimension::GetSize(*it, helper.direction) +
        dimension::GetSize(helper.border_size, helper.direction);
  }

  dimension::SetSize(size, helper.direction, helper.direction_max);
  dimension::SetSize(size, helper.non_direction,
                     helper.non_direction_pos + helper.non_direction_max);
  return size;
}

GridCellCount GridCtrl::CountCell(const PositionVectorConst &vector) const {
  GridCellCount count;
  GridCtrlItemHelper helper(this);
  helper.direction = dimension::kHorizontal;
  helper.non_direction = dimension::kVertical;

  int non_direction_pos = -1;
  int count_col = 0;
  for (const auto &it : vector) {
    helper.NextItem(it);

    ++count_col;
    if (non_direction_pos != helper.non_direction_pos) {
      ++count.row;
      non_direction_pos = helper.non_direction_pos;
      count.col = std::max(count.col, count_col);
      count_col = 0;
    }

    helper.direction_pos +=
        dimension::GetSize(*it, helper.direction) +
        dimension::GetSize(helper.border_size, helper.direction);
  }
  return count;
}

}  // namespace position

}  // namespace fmr
