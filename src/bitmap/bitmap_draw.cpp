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

#include "fmr/bitmap/bitmap_draw.h"

#include "wx/dc.h"

namespace fmr {

namespace bitmap {

struct TempScaledDc {
  wxDC &dc_;
  double x_, y_;
  TempScaledDc(wxDC &dc, double x, double y) : dc_(dc) {
    dc.GetUserScale(&x_, &y_);
    dc.SetUserScale(x, y);
  }
  ~TempScaledDc() { dc_.SetUserScale(x_, y_); }
};

void BitmapDraw::Create(const wxImage &image, const wxPoint &start_pos) {
  wxSize img_size = image.GetSize();
  sliced_img_rect_.SetPosition(start_pos);

  if (start_pos.x > img_size.GetWidth() || start_pos.y > img_size.GetHeight())
    return;
  else if (image.IsOk()) {
    int limit_x = start_pos.x + kBitmapMaximumDraw;
    int limit_y = start_pos.y + kBitmapMaximumDraw;

    int img_limit_x = img_size.GetWidth();
    int img_limit_y = img_size.GetHeight();

    wxPoint pos;
    pos.x = std::min(limit_x, img_limit_x) - 1;
    pos.y = std::min(limit_y, img_limit_y) - 1;

    sliced_img_rect_.SetBottomRight(pos);
    bmp_ = image.GetSubImage(sliced_img_rect_);
  }
}

wxPoint BitmapDraw::GetDrawPosition() const {
  wxPoint pos = pos_;
  pos.x /= scale_x_;
  pos.y /= scale_y_;

  pos.x += sliced_img_rect_.GetLeft();
  pos.y += sliced_img_rect_.GetTop();

  return pos;
}

void BitmapDraw::Draw(wxDC &dc) {
  if (IsOk()) {
    TempScaledDc scale(dc, scale_x_, scale_y_);
    double x, y;
    dc.GetUserScale(&x, &y);
    dc.DrawBitmap(bmp_, GetDrawPosition());
  }
}

}  // namespace bitmap

}  // namespace fmr
