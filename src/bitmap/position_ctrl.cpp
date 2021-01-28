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
  wxPoint temp_pos = wxPoint(0, 0);
  wxSize size = GetWindowSize();

  if (flags_ & kPositionVertical) {
    temp_pos = wxPoint(0, 0);
    for (auto &it : page) {
      it->SetY(temp_pos.y);
      temp_pos.y += it->GetHeight();

      if (flags_ & kPositionAlignCenter) {
        int x = size.GetWidth() / 2 - it->GetWidth() / 2;
        if (x < 0) x = 0;
        it->SetX(x);
      }
    }
  }

  if (flags_ & kPositionHorizontal) {
    temp_pos = wxPoint(0, 0);
    for (auto &it : page) {
      it->SetX(temp_pos.x);
      temp_pos.x += it->GetWidth();

      if (flags_ & kPositionAlignCenter) {
        int y = size.GetHeight() / 2 - it->GetHeight() / 2;
        if (y < 0) y = 0;
        it->SetY(y);
      }
    }
  }
}

wxSize PositionCtrl::GetSize(const std::vector<SBitmap *> &page) const {
  wxSize size;
  if (flags_ & kPositionVertical) {
    for (const auto &it : page) {
      if (it->GetWidth() > size.GetWidth()) {
        size.SetWidth(it->GetWidth());
      }
      size.SetHeight(size.GetHeight() + it->GetHeight());
    }
  }

  if (flags_ & kPositionHorizontal) {
    for (const auto &it : page) {
      if (it->GetHeight() > size.GetHeight()) {
        size.SetHeight(it->GetHeight());
      }
      size.SetHeight(size.GetHeight() + it->GetHeight());
    }
  }

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
