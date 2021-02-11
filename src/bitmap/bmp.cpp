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

#include <fmr/bitmap/bmp.h>
#include <wx/dc.h>

namespace fmr {

SBitmap::SBitmap(bool isLoaded) { m_isLoaded = isLoaded; }

bool SBitmap::IsOk() const { return m_isOk; }
bool SBitmap::IsLoaded() const { return m_isLoaded; }

bool SBitmap::IsPointed(const wxPoint& area, const wxPoint& position) const {
  if (!IsOk()) return false;

  int posY = area.y + position.y, posX = area.x + position.x, bmpPosY = GetY(),
      bmpAfterY = bmpPosY + GetHeight(), bmpPosX = GetX(),
      bmpAfterX = bmpPosX + GetWidth();

  return (posY >= bmpPosY && posY <= bmpAfterY && posX >= bmpPosX &&
          posX <= bmpAfterX);
}

bool SBitmap::IsShown(const wxPoint& area, const wxSize& size) const {
  if (!IsOk()) return false;

  int top = area.y, bottom = top + size.GetHeight(), left = area.x,
      right = left + size.GetHeight(), bmpPosY = GetY(),
      bmpAfterY = bmpPosY + GetHeight(), bmpPosX = GetX(),
      bmpAfterX = bmpPosX + GetWidth();

  return ((bmpPosY >= top || bmpAfterY >= top) &&
          (bmpPosY <= bottom || bmpAfterY <= bottom)) &&
         ((bmpPosX >= left || bmpAfterY >= left) ||
          (bmpPosX <= right || bmpAfterX <= right));
}

wxPoint SBitmap::GetPosition() const { return m_pos; }
wxSize SBitmap::GetSize() const { return wxSize(GetWidth(), GetHeight()); }

void SBitmap::Draw(wxDC& dc, const wxPoint& view_start, const wxSize& area) {
  Draw(dc, wxRect(view_start, area));
}

wxRect SBitmap::CalcMinimumRect(const wxRect& rect, wxPoint* pos_start) const {
  wxRect ret_rect;
  if (!IsShown(rect.GetPosition(), rect.GetSize())) return ret_rect;

  double scale_x, scale_y;
  GetScale(scale_x, scale_y);

  if (pos_start) {
    pos_start->x = GetPosition().x / scale_x;
    pos_start->y = GetPosition().y / scale_y;
  }

  ret_rect.SetSize(GetSize());
  wxRect bmp_rect(GetPosition(), GetSize());

  auto get_limit = [](int size, int pos, int view_start) {
    int limit = size + (view_start > pos ? pos - view_start : 0);
    return limit;
  };

  auto get_size = [&](int size, int pos, int view_start, int max_size) {
    int limit = get_limit(size, pos, view_start);
    int ret = max_size - (pos > view_start ? pos - view_start : 0);
    return ret < limit ? ret : limit;
  };

  auto get_bitmap_position = [](int bmp_pos, int view_start, double scale) {
    int ret = (bmp_pos < view_start) ? view_start : bmp_pos;
    return ret / scale;
  };

  auto get_bitmap_pos_start = [](int bmp_pos, int view_start) {
    int ret = view_start - bmp_pos;
    return (ret > 0) ? ret : 0;
  };

  if (rect.GetWidth() < bmp_rect.GetWidth()) {
    ret_rect.SetWidth(get_size(bmp_rect.GetWidth(), bmp_rect.GetX(),
                               rect.GetX(), rect.GetWidth()));
    ret_rect.SetX(get_bitmap_pos_start(bmp_rect.GetX(), rect.GetX()));
    if (pos_start)
      pos_start->x = get_bitmap_position(bmp_rect.GetX(), rect.GetX(), scale_x);
  }

  if (rect.GetHeight() < bmp_rect.GetHeight()) {
    ret_rect.SetHeight(get_size(bmp_rect.GetHeight(), bmp_rect.GetY(),
                                rect.GetY(), rect.GetHeight()));
    ret_rect.SetY(get_bitmap_pos_start(bmp_rect.GetY(), rect.GetY()));

    if (pos_start)
      pos_start->y = get_bitmap_position(bmp_rect.GetY(), rect.GetY(), scale_y);
  };

  ret_rect.width /= scale_x;
  ret_rect.x /= scale_x;
  ret_rect.height /= scale_y;
  ret_rect.y /= scale_y;

  return ret_rect;
}

void SBitmap::Draw(wxDC& dc, const wxRect& rect) {
  wxPoint pos;
  wxRect minimum_rect = CalcMinimumRect(visible_area_, &pos);

  double x, y;
  dc.GetUserScale(&x, &y);
  dc.SetUserScale(scale_x_, scale_y_);
  if (!minimum_rect.IsEmpty()) dc.DrawBitmap(visible_bitmap_, pos);
  dc.SetUserScale(x, y);
}

void SBitmap::PrepareBitmap() {
  wxRect rect = CalcMinimumRect(visible_area_);
  if (rect != visible_bitmap_rect_) {
    visible_bitmap_ = GetImage().GetSubImage(rect);
    visible_bitmap_rect_ = rect;
  }
}

wxString SBitmap::GetName() { return m_name; }
size_t SBitmap::GetIndex() { return m_index; }
void SBitmap::GetScale(double& x, double& y) const {
  x = scale_x_;
  y = scale_y_;
}
double SBitmap::GetScale() { return scale_x_; }
int SBitmap::GetWidth() const { return GetImage().GetWidth() * scale_x_; }
int SBitmap::GetHeight() const { return GetImage().GetHeight() * scale_y_; }
int SBitmap::GetY() const { return m_pos.y; }
int SBitmap::GetX() const { return m_pos.x; }

void SBitmap::SetBitmap(const wxBitmap& bmp) {
  m_isOk = true;
  SetLoaded();
}

void SBitmap::SetImage(const wxImage& image) {
  image_ = image;
  m_isOk = image_.IsOk();
  SetLoaded();

  PrepareBitmap();
}

void SBitmap::SetVisibleArea(const wxRect& rect) {
  visible_area_ = rect;
  PrepareBitmap();
}

void SBitmap::SetLoaded(bool stat) { m_isLoaded = stat; }
void SBitmap::SetName(const wxString& name) { m_name = name; }
void SBitmap::SetIndex(size_t idx) { m_index = idx; }
void SBitmap::SetScale(double scale) {
  scale_x_ = scale;
  scale_y_ = scale;
  PrepareBitmap();
}
void SBitmap::SetScale(double x, double y) {
  scale_x_ = x;
  scale_y_ = y;
  PrepareBitmap();
}
void SBitmap::SetPosition(const wxPoint& pos) {
  m_pos = pos;
  PrepareBitmap();
}

void SBitmap::SetY(int PosY) { m_pos.y = PosY; }
void SBitmap::SetX(int PosX) { m_pos.x = PosX; }

};  // namespace fmr
