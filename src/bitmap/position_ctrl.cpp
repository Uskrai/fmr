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

#include "fmr/bitmap/position_ctrl.h"

#include "fmr/common/dimension.h"

namespace fmr {

namespace bitmap {

std::vector<const SBitmap *> ConvertVecBitmapToConst(
    const std::vector<SBitmap *> vec) {
  std::vector<const SBitmap *> temp;
  for (const auto &it : vec) temp.push_back(it);
  return temp;
}

PositionCtrl::PositionCtrl(PositionFlags flags) { flags_ = flags; }

void PositionCtrl::RecalcPosition(const std::vector<SBitmap *> &page) const {
  wxSize win_size = GetWindowSize();
  wxSize page_size = GetMinimumBitmapSize(page);

  auto get_start_position = [](int bmp_size, int min_size) {
    if (bmp_size < min_size) {
      return min_size / 2 - bmp_size / 2;
    }
    return 0;
  };

  auto make_line = [&page](int start_pos, wxOrientation orient) {
    int pos = start_pos;
    for (auto &it : page) {
      if (!it->IsOk()) continue;
      dimension::SetPoint(*it, orient, pos);
      pos += dimension::GetSize(*it, orient);
    }
  };

  auto make_centered = [&page](int min_size, wxOrientation orient) {
    for (auto &it : page) {
      if (!it->IsOk()) continue;
      int pos = min_size / 2 - dimension::GetSize(*it, orient) / 2;
      pos = std::max(0, pos);
      dimension::SetPoint(*it, orient, pos);
    }
  };

  if (flags_ & kPositionHorizontal) {
    int start_pos =
        get_start_position(page_size.GetWidth(), GetMinimumSize().GetWidth());
    make_line(start_pos, wxHORIZONTAL);
    if (flags_ & kPositionAlignCenter)
      make_centered(win_size.GetHeight(), wxVERTICAL);
  }

  if (flags_ & kPositionVertical) {
    int start_pos =
        get_start_position(page_size.GetHeight(), GetMinimumSize().GetHeight());
    make_line(start_pos, wxVERTICAL);
    if (flags_ & kPositionAlignCenter)
      make_centered(win_size.GetWidth(), wxHORIZONTAL);
  }
}

wxSize PositionCtrl::GetSize(const std::vector<SBitmap *> &vec_bitmap) const {
  return GetSize(ConvertVecBitmapToConst(vec_bitmap));
}

wxSize PositionCtrl::GetSize(const std::vector<const SBitmap *> &page) const {
  wxSize size = GetMinimumBitmapSize(page);

  if (size.GetHeight() < GetMinimumSize().GetHeight()) {
    size.SetHeight(GetMinimumSize().GetHeight());
  }
  if (size.GetWidth() < GetMinimumSize().GetWidth()) {
    size.SetWidth(GetMinimumSize().GetWidth());
  }

  return size;
}

wxSize PositionCtrl::GetMinimumBitmapSize(
    const std::vector<SBitmap *> &page) const {
  return GetMinimumBitmapSize(ConvertVecBitmapToConst(page));
}

wxSize PositionCtrl::GetMinimumBitmapSize(
    const std::vector<const SBitmap *> &page) const {
  wxSize size;

  auto iterate_page = [&page](wxOrientation increment, wxOrientation largest) {
    wxSize size;
    for (const auto &it : page) {
      if (!it->IsOk()) continue;
      dimension::SetSize(size, increment,
                         dimension::GetSize(*it, increment) +
                             dimension::GetSize(size, increment));

      if (dimension::GetSize(*it, largest) > dimension::GetSize(size, largest))
        dimension::SetSize(size, largest, dimension::GetSize(*it, largest));
    }
    return size;
  };

  if (flags_ & kPositionVertical) {
    size = iterate_page(wxVERTICAL, wxHORIZONTAL);
  }

  if (flags_ & kPositionHorizontal) {
    size = iterate_page(wxHORIZONTAL, wxVERTICAL);
  }

  return size;
}

}  // namespace bitmap

}  // namespace fmr
