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

#ifndef FMR_BITMAP_BITMAP_CTRL
#define FMR_BITMAP_BITMAP

#include <wx/window.h>

#include "fmr/bitmap/loader.h"
#include "fmr/bitmap/position_ctrl.h"

namespace fmr {

namespace bitmap {

class BitmapCtrl {
 private:
  std::vector<SBitmap *> vec_bitmap_;
  PositionCtrl *pos_ctrl_ = nullptr;
  Rescaler *rescaler_ = nullptr;

 public:
  BitmapCtrl(PositionCtrl *position, Rescaler *rescaler);

  std::vector<SBitmap *> &GetBitmap() { return vec_bitmap_; }
  const std::vector<SBitmap *> &GetBitmap() const { return vec_bitmap_; }
  void SetBitmap(const std::vector<SBitmap *> &bitmap) {
    vec_bitmap_ = bitmap;
  };

  void AdjustBitmap();

  void RecalcPosition(const std::vector<SBitmap *> &bitmap) const;
  void RecalcPosition();
  wxSize GetSize() const;
  wxSize GetSize(const std::vector<SBitmap *> &bitmap) const;

  void Clear();

 private:
  void OnWindowSize(wxSizeEvent &event);
  void OnImageLoaded(thread::LoadImageEvent &event);
};

}  // namespace bitmap

}  // namespace fmr

#endif /* end of include guard: FMR_BITMAP_BITMAP */
