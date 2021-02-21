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

#ifndef FMR_EXPLORER_IMAGE_CELL_EXPLORER
#define FMR_EXPLORER_IMAGE_CELL_EXPLORER

#include <fmr/bitmap/bmp.h>
#include <fmr/window/grid_cell.h>

#include <memory>

namespace fmr {

namespace explorer {

class StringDraw;

class ImageCell : public window::GridCell {
  SBitmap bmp_;
  std::unique_ptr<StringDraw> string_;
  wxRect bmp_rect_, self_rect_on_bmp_;

 public:
  ImageCell();
  virtual ~ImageCell();
  SBitmap &GetBitmap() { return bmp_; }
  void SetString(const std::string &string);

  wxRect GetBitmapArea() const;

  void PrepareBitmap();
  void PrepareString();

  void SetStringFlags(int horizontal, int vertical);

  static wxSize GetBestBitmapSize(wxSize size);

  void DrawBitmap(wxDC &dc);
  void DrawString(wxDC &dc);
  void Draw(wxDC &dc) override;
};

}  // namespace explorer

}  // namespace fmr

#endif /* end of include guard: FMR_EXPLORER_IMAGE_CELL_EXPLORER */
