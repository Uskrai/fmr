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

#ifndef FMR_BITMAP_POSITION_BOX_CTRL
#define FMR_BITMAP_POSITION_BOX_CTRL

#include <fmr/common/vector.h>
#include <fmr/position/ctrl_base.h>
#include <fmr/position/item.h>
#include <fmr/position/pos_flags.h>
#include <wx/gdicmn.h>

namespace fmr {

namespace position {

class BoxCtrl : public CtrlBase {
 private:
  PosFlags flags_;

 public:
  BoxCtrl() {}
  BoxCtrl(PosFlags flags);

  void SetFlags(PosFlags flags) { flags_ = flags; }
  PosFlags GetFlags() const { return flags_; }
  bool CheckFlags(PosFlags flags) const { return flags_ & flags; }

  void CalculatePosition(const PositionVector &vec_item) const override;
  using CtrlBase::CalculatePosition;

  wxSize GetMinimumItemSize(const PositionVectorConst &vec_item) const override;
  using CtrlBase::GetMinimumItemSize;
};

}  // namespace position

}  // namespace fmr

#endif /* end of include guard: FMR_BITMAP_POSITION_BOX_CTRL */
