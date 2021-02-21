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

#include "fmr/position/box_ctrl.h"

#include "fmr/common/dimension.h"
#include "fmr/position/pos_flags.h"

namespace fmr {

namespace position {

BoxCtrl::BoxCtrl(PosFlags flags) { flags_ = flags; }

void BoxCtrl::CalculatePosition(const PositionVector &page) const {
  using dimension::Orientation;
  wxSize win_size = GetWindowSize();
  wxSize page_size =
      GetMinimumItemSize(PositionVectorConst(page.begin(), page.end()));

  auto get_start_position = [](int bmp_size, int min_size) {
    if (bmp_size < min_size) {
      return min_size / 2 - bmp_size / 2;
    }
    return 0;
  };

  auto make_line = [&page](int start_pos, Orientation orient) {
    int pos = start_pos;
    for (auto &it : page) {
      dimension::SetPoint(*it, orient, pos);
      pos += dimension::GetSize(*it, orient);
    }
  };

  auto make_centered = [&page](int min_size, Orientation orient) {
    for (auto &it : page) {
      int pos = min_size / 2 - dimension::GetSize(*it, orient) / 2;
      pos = std::max(0, pos);
      dimension::SetPoint(*it, orient, pos);
    }
  };

  if (CheckFlags(kPositionHorizontal)) {
    int start_pos =
        get_start_position(page_size.GetWidth(), GetMinimumSize().GetWidth());
    make_line(start_pos, dimension::kHorizontal);
    if (CheckFlags(kPositionAlignCenter))
      make_centered(win_size.GetHeight(), dimension::kVertical);
  }

  if (CheckFlags(kPositionVertical)) {
    int start_pos =
        get_start_position(page_size.GetHeight(), GetMinimumSize().GetHeight());
    make_line(start_pos, dimension::kVertical);
    if (CheckFlags(kPositionAlignCenter))
      make_centered(win_size.GetWidth(), dimension::kHorizontal);
  }
}

wxSize BoxCtrl::GetMinimumItemSize(const PositionVectorConst &page) const {
  wxSize size;

  using dimension::Orientation;
  auto iterate_page = [&page](Orientation increment, Orientation largest) {
    wxSize size;
    for (const auto &it : page) {
      dimension::SetSize(size, increment,
                         dimension::GetSize(*it, increment) +
                             dimension::GetSize(size, increment));

      if (dimension::GetSize(*it, largest) > dimension::GetSize(size, largest))
        dimension::SetSize(size, largest, dimension::GetSize(*it, largest));
    }
    return size;
  };

  if (CheckFlags(kPositionHorizontal)) {
    size = iterate_page(dimension::kHorizontal, dimension::kVertical);
  }

  if (CheckFlags(kPositionVertical)) {
    size = iterate_page(dimension::kVertical, dimension::kHorizontal);
  }

  return size;
}

}  // namespace position

}  // namespace fmr
