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

#ifndef FMR_BITMAP_BITMAP_DRAW
#define FMR_BITMAP_BITMAP_DRAW

#include <wx/bitmap.h>

namespace fmr {

namespace bitmap {

inline int kBitmapMaximumDraw = 5000;

class BitmapDraw {
  wxBitmap bmp_;
  double scale_x_ = 1, scale_y_ = 1;
  wxPoint pos_;
  wxRect sliced_img_rect_;

 public:
  /**
   * @see Create
   */
  BitmapDraw(const wxImage &img, const wxPoint &start_pos = wxPoint(0, 0)) {
    Create(img, start_pos);
  };

  /**
   * @brief: Construct Bitmap from image
   *
   * @param: img Image Object
   * @param: start_pos Position to start the image splicing
   *
   * @return: Construct Bitmap from image
   */
  void Create(const wxImage &image, const wxPoint &start_pos = wxPoint(0, 0));

  /**
   * @brief: Seting Scale when drawing
   */
  void SetScale(double x, double y) {
    scale_x_ = x;
    scale_y_ = y;
  };
  /**
   * @brief: Getting Scale applied when drawing
   */
  void GetScale(double &x, double &y) const {
    x = scale_x_;
    y = scale_y_;
  };

  bool IsOk() const { return bmp_.IsOk(); }

  void Draw(wxDC &dc);

  int GetOriginalWidth() const { return IsOk() ? bmp_.GetWidth() : 0; }
  int GetOriginalHeight() const { return IsOk() ? bmp_.GetHeight() : 0; }

  /**
   * @brief: Setting Image Start Position
   * Pass the image position passed from Constructor or Create method instead of
   * this object individual bitmap position
   * @param: Image Start Position to draw
   *
   * @return: Setting Image Start Position
   */
  void SetPosition(const wxPoint &pos) { pos_ = pos; }
  wxPoint GetPosition() const { return pos_; }

  int GetWidth() const { return GetOriginalWidth() * scale_x_; }
  int GetHeight() const { return GetOriginalHeight() * scale_y_; }

  wxRect GetSlicedArea() const { return sliced_img_rect_; }

  wxPoint GetDrawPosition() const;

 protected:
};

}  // namespace bitmap

}  // namespace fmr

#endif /* end of include guard: FMR_BITMAP_BITMAP_DRAW */
