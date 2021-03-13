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

#ifndef FMR_POSITION_GRID_CTRL
#define FMR_POSITION_GRID_CTRL

#include <fmr/common/vector.h>
#include <fmr/position/ctrl_base.h>
#include <fmr/position/pos_flags.h>

namespace fmr {

namespace position {

class GridCellCount {
 public:
  int row = 0, col = 0;
};

class GridCtrl : public CtrlBase {
  wxSize border_size_{0, 0};
  PosFlags flags_ = kPositionNone;

 public:
  GridCtrl();

  void CalculatePosition(const PositionVector &vector) const override;
  using CtrlBase::CalculatePosition;

  wxSize GetMinimumItemSize(const PositionVectorConst &vector) const override;
  using CtrlBase::GetMinimumItemSize;

  void SetBorderSize(wxSize size) { border_size_ = size; }
  wxSize GetBorderSize() const { return border_size_; }
  int GetBorderSize(dimension::Orientation orient) const {
    return dimension::GetSize(GetBorderSize(), orient);
  }

  GridCellCount CountCell(const PositionVectorConst &vector) const;

  void SetFlags(position::PosFlags flags) { flags_ = flags; }
  bool CheckFlags(position::PosFlags flags) const { return flags_ & flags; }
};

}  // namespace position

}  // namespace fmr

#endif /* end of include guard: FMR_POSITION_GRID_CTRL */
