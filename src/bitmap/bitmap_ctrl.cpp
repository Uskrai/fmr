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

#include "fmr/bitmap/bitmap_ctrl.h"

namespace fmr {

namespace bitmap {

BitmapCtrl::BitmapCtrl(PositionCtrl *pos_ctrl, Rescaler *rescaler) {
  pos_ctrl_ = pos_ctrl;
  rescaler_ = rescaler;
}

void BitmapCtrl::RecalcPosition() { pos_ctrl_->RecalcPosition(GetBitmap()); }

wxSize BitmapCtrl::GetSize() const { return pos_ctrl_->GetSize(GetBitmap()); }

void BitmapCtrl::RecalcPosition(const std::vector<SBitmap *> &bitmap) const {
  pos_ctrl_->RecalcPosition(bitmap);
}

wxSize BitmapCtrl::GetSize(const std::vector<SBitmap *> &bitmap) const {
  return pos_ctrl_->GetSize(bitmap);
}

void BitmapCtrl::AdjustBitmap() {
  RecalcPosition(GetBitmap());
  for (const auto &it : GetBitmap()) {
    rescaler_->DoRescale(*it);
  }
}

}  // namespace bitmap

}  // namespace fmr
