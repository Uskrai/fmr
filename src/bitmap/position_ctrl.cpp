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

namespace fmr {

namespace bitmap {

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

  auto make_line = [&page](void (SBitmap::*set_pos)(int),
                           int (SBitmap::*get_size)() const, int start_pos) {
    int pos = start_pos;
    for (auto &it : page) {
      (*it.*set_pos)(pos);
      pos += (*it.*get_size)();
    }
  };

  auto make_centered = [&page](int min_size, void (SBitmap::*set_pos)(int),
                               int (SBitmap::*get_size)() const) {
    for (auto &it : page) {
      int pos = min_size / 2 - (*it.*get_size)() / 2;
      if (pos < 0) pos = 0;
      (*it.*set_pos)(pos);
    }
  };

  if (flags_ & kPositionHorizontal) {
    int start_pos =
        get_start_position(page_size.GetWidth(), GetMinimumSize().GetWidth());
    make_line(&SBitmap::SetX, &SBitmap::GetWidth, start_pos);
    if (flags_ & kPositionAlignCenter)
      make_centered(win_size.GetHeight(), &SBitmap::SetY, &SBitmap::GetHeight);
  }

  if (flags_ & kPositionVertical) {
    int start_pos =
        get_start_position(page_size.GetHeight(), GetMinimumSize().GetHeight());
    make_line(&SBitmap::SetY, &SBitmap::GetHeight, start_pos);
    if (flags_ & kPositionAlignCenter)
      make_centered(win_size.GetWidth(), &SBitmap::SetX, &SBitmap::GetWidth);
  }
}

wxSize PositionCtrl::GetMinimumBitmapSize(
    const std::vector<SBitmap *> &page) const {
  wxSize size;

  auto iterate = [&page](int (SBitmap::*bmp_incrementer)() const,
                         int (wxSize::*size_incrementer)() const,
                         void (wxSize::*set_incremented)(int),
                         int (SBitmap::*search_largest)() const,
                         int (wxSize::*get_largest)() const,
                         void (wxSize::*set_largest)(int)) {
    wxSize size;
    for (const auto &it : page) {
      (size.*set_incremented)((*it.*bmp_incrementer)() +
                              (size.*size_incrementer)());

      if ((*it.*search_largest)() > (size.*get_largest)()) {
        (size.*set_largest)((*it.*search_largest)());
      }
    }
    return size;
  };

  if (flags_ & kPositionVertical) {
    size = iterate(&SBitmap::GetHeight, &wxSize::GetHeight, &wxSize::SetHeight,
                   &SBitmap::GetWidth, &wxSize::GetWidth, &wxSize::SetWidth);
  }

  if (flags_ & kPositionHorizontal) {
    size = iterate(&SBitmap::GetWidth, &wxSize::GetWidth, &wxSize::SetWidth,
                   &SBitmap::GetHeight, &wxSize::GetHeight, &wxSize::SetWidth);
  }

  return size;
}

wxSize PositionCtrl::GetSize(const std::vector<SBitmap *> &page) const {
  wxSize size = GetMinimumBitmapSize(page);

  if (size.GetHeight() < GetMinimumSize().GetHeight()) {
    size.SetHeight(GetMinimumSize().GetHeight());
  }
  if (size.GetWidth() < GetMinimumSize().GetWidth()) {
    size.SetWidth(GetMinimumSize().GetWidth());
  }

  return size;
}

}  // namespace bitmap

}  // namespace fmr
