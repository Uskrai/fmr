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

#include "fmr/explorer/image_cell_explorer.h"

#include "fmr/common/dimension.h"
#include "fmr/explorer/string_draw.h"

namespace fmr {

namespace explorer {

ImageCell::ImageCell() {}

void ImageCell::SetString(const std::string &string) {
  string_ = std::make_unique<StringDraw>(string);
}

wxSize ImageCell::GetBestBitmapSize(wxSize size) {
  size.Scale(1, 0.8);
  return size;
}

wxRect ImageCell::GetBitmapArea() const {
  return wxRect(GetRect().GetPosition(),
                GetBestBitmapSize(GetRect().GetSize()));
}

void ImageCell::PrepareBitmap() {
  wxRect bmp_rect = GetBitmapArea();
  bmp_rect_.SetSize(bmp_.GetSize());

  bmp_rect_ = bmp_rect_.CenterIn(bmp_rect);
  bmp_.SetPosition(bmp_rect_.GetPosition());
  bmp_.SetVisibleArea(bmp_rect_);

  self_rect_on_bmp_ = GetRect();
}

void ImageCell::PrepareString() {
  wxRect bmp_rect = GetBitmapArea();

  wxSize size = wxSize(GetWidth(), GetHeight() - bmp_rect.GetHeight());
  wxRect str_rect_ = wxRect(bmp_rect.GetBottomLeft(), size);

  if (string_ && str_rect_ != string_->GetRect()) string_->SetRect(str_rect_);
}

void ImageCell::SetStringFlags(int horizontal, int vertical) {
  string_->SetHorizontalFlags(horizontal);
  string_->SetVerticalFlags(vertical);
}

void ImageCell::Draw(wxDC &dc) {
  DrawBitmap(dc);
  DrawString(dc);
}

void ImageCell::DrawString(wxDC &dc) {
  PrepareString();
  if (string_) string_->Draw(dc);
}

void ImageCell::DrawBitmap(wxDC &dc) {
  if (bmp_rect_.GetSize() != bmp_.GetSize() || self_rect_on_bmp_ != GetRect())
    PrepareBitmap();

  bmp_.Draw(dc, bmp_rect_);
}

ImageCell::~ImageCell() {
  //
}

}  // namespace explorer

}  // namespace fmr
