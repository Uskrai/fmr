/*
 *  Copyright (c) 2020-2021 Uskrai
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

#ifndef FMR_BITMAP_BMP
#define FMR_BITMAP_BMP

#include <fmr/bitmap/bitmap_draw.h>

#include <vector>

namespace fmr {

struct SBitmap {
 private:
  wxRect rect_, visible_area_;
  double scale_x_ = 1, scale_y_ = 1;
  wxPoint pos_ = wxPoint(0, 0);
  bool is_ok_ = false;  // determine bitmap status
  bool m_isLoaded = false;
  std::vector<bitmap::BitmapDraw> vec_bmp_;

 public:
  SBitmap();
  SBitmap(bool isLoaded);
  SBitmap(const wxImage& image);
  virtual ~SBitmap();

  bool IsOk() const;
  bool IsLoaded() const;

  wxRect CalcMinimumRect(const wxRect& rect, wxPoint* pos = nullptr) const;

  void Draw(wxDC& dc, const wxPoint& view_start, const wxSize& area);
  void Draw(wxDC& dc, const wxRect& rect);

  void PrepareBitmap();

  bool IsPointed(const wxPoint& area, const wxPoint& position) const;
  bool IsShown(const wxPoint& area, const wxSize& size) const;

  wxPoint GetPosition() const;
  wxSize GetSize() const;

  wxString GetName();
  size_t GetIndex();
  void GetScale(double& x, double& y) const;
  int GetWidth() const;
  int GetHeight() const;
  int GetY() const;
  int GetX() const;

  int GetOriginalWidth() const;
  int GetOriginalHeight() const;
  wxSize GetOriginalSize() const;

  void SetImage(const wxImage& image);

  void SetVisibleArea(const wxPoint& pos, const wxSize& size) {
    return SetVisibleArea(wxRect(pos, size));
  }
  void SetVisibleArea(const wxRect& rect);
  void SetLoaded(bool stat = true);
  void SetName(const wxString& name);
  void SetIndex(size_t idx);
  void SetScale(double x, double y);
  void SetPosition(const wxPoint& pos);
  void SetY(int PosY);
  void SetX(int PosX);
};

}  // namespace fmr

#endif
