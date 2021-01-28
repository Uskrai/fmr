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

#ifndef FMR_BITMAP_POSITION_CTRL
#define FMR_BITMAP_POSITION_CTRL

#include <vector>

#include "fmr/bitmap/bmp.h"
#include "fmr/common/bitmask.h"

namespace fmr {

namespace bitmap {

enum PositionFlags {
  kPositionAlignCenter = 0x01,
  kPositionVertical = 0x02,
  kPositionHorizontal = 0x04
};

DEFINE_BITMASK_TYPE(PositionFlags);

class PositionCtrl {
 private:
  std::vector<SBitmap *> bitmap_;
  wxSize window_size_;
  wxSize minimum_size_;
  PositionFlags flags_;

 public:
  PositionCtrl(PositionFlags flags);

  PositionFlags GetFlags() const { return flags_; }

  void SetMinimumSize(const wxSize &size) { minimum_size_ = size; }
  const wxSize &GetMinimumSize() const { return minimum_size_; }

  void RecalcPosition(const std::vector<SBitmap *> &vec_bitmap) const;
  wxSize GetSize(const std::vector<SBitmap *> &vec_bitmap) const;

  void SetWindowSize(const wxSize &size) { window_size_ = size; }
  const wxSize &GetWindowSize() const { return window_size_; }
};

}  // namespace bitmap

}  // namespace fmr

#endif /* end of include guard: FMR_BITMAP_POSITION_CTRL */
