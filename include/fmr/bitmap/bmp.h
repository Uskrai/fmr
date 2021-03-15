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

#include <wx/bitmap.h>

namespace fmr {

struct SBitmap {
  wxImage image_;
  wxBitmap visible_bitmap_;
  wxString m_name = wxEmptyString;
  bool prepare_ = false;
  wxPoint draw_pos_;
  size_t m_index = -1;
  double scale_x_ = 1, scale_y_ = 1;
  wxPoint m_pos = wxPoint(0, 0);
  wxRect visible_area_;
  wxRect visible_bitmap_rect_;
  bool m_isOk = false;  // determine bitmap status
  bool m_isLoaded = false;

  SBitmap() {}
  SBitmap(bool isLoaded);
  SBitmap(const wxImage& image);

  const wxImage& GetImage() const { return image_; }
  wxImage& GetImage() { return image_; }
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
  [[deprecated]] double GetScale();
  void GetScale(double& x, double& y) const;
  int GetWidth() const;
  int GetHeight() const;
  int GetY() const;
  int GetX() const;

  [[deprecated("Use SetImage instead")]] void SetBitmap(const wxBitmap& bmp);
  void SetImage(const wxImage& image);

  void SetVisibleArea(const wxPoint& pos, const wxSize& size) {
    return SetVisibleArea(wxRect(pos, size));
  }
  void SetVisibleArea(const wxRect& rect);
  void SetLoaded(bool stat = true);
  void SetName(const wxString& name);
  void SetIndex(size_t idx);
  void SetScale(double scale);
  void SetScale(double x, double y);
  void SetPosition(const wxPoint& pos);
  void SetY(int PosY);
  void SetX(int PosX);

  void Prepare(bool prepare = true) { prepare_ = prepare; }
};

}  // namespace fmr

#endif
