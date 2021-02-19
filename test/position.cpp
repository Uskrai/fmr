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

#include <fmr/position/grid_ctrl.h>
#include <gtest/gtest.h>
#include <wx/gdicmn.h>

#include "fmr/common/dimension.h"

namespace fmr {

TEST(GridCtrlTest, Position) {
  position::GridCtrl ctrl;
  std::vector<wxRect> rect;
  rect.resize(8, wxRect(0, 0, 200, 200));

  ctrl.SetMinimumSize(wxSize(300, 300));
  ctrl.CalculatePosition(rect);

  EXPECT_EQ(rect[1].GetPosition(), wxPoint(200, 0));
  EXPECT_EQ(rect[4].GetPosition(), wxPoint(0, 400));
  EXPECT_EQ(rect[6].GetPosition(), wxPoint(0, 600));
  EXPECT_EQ(rect[7].GetPosition(), wxPoint(200, 600));

  ctrl.SetBorderSize(wxSize(30, 5));
  ctrl.CalculatePosition(rect);

  EXPECT_EQ(rect[3].GetPosition(), wxPoint(230, 205));
  EXPECT_EQ(rect[4].GetPosition(), wxPoint(0, 410));
  EXPECT_EQ(rect[5].GetPosition(), wxPoint(230, 410));

  ctrl.SetBorderSize(wxSize(300, 0));
  ctrl.CalculatePosition(rect);
  for (const auto &it : rect) EXPECT_EQ(it.GetPosition().x, 0);

  ctrl.SetBorderSize(wxSize(0, 300));
  ctrl.SetMinimumSize(wxSize(200, 200));
  ctrl.CalculatePosition(rect);

  int val = 0;
  for (const auto &it : rect) {
    EXPECT_EQ(it.GetPosition().y, val);
    val += it.GetHeight() + ctrl.GetBorderSize(dimension::kVertical);
  }
}

}  // namespace fmr

int main(int argc, char **argv) {
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
