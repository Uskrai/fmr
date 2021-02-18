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

#include "fmr/bitmap/rescaler.h"

namespace fmr {

namespace bitmap {

void Rescaler::DoRescale(wxSize &size) {
  double scale_x = 1, scale_y = 1;
  GetScale(size, scale_x, scale_y);
  size.Scale(scale_x, scale_y);
}

void Rescaler::DoRescale(SBitmap &bitmap) {
  double scale_x = 1, scale_y = 1;
  GetScale(bitmap, scale_x, scale_y);
  bitmap.SetScale(scale_x, scale_y);
}

void Rescaler::DoRescale(wxImage &image) {
  if (!image.IsOk()) return;

  double x = 1, y = 1;
  GetScale(image, x, y);
  wxSize size = image.GetSize();
  size.Scale(x, y);
  image.Rescale(size.GetWidth(), size.GetHeight());
}

void Rescaler::GetScale(const wxSize &size, double &x, double &y) {
  if (Is(kRescaleFitWidth)) {
    if (size.GetWidth() > fit_size_.GetWidth() || Is(kRescaleEnlarge)) {
      x = CalcScale(size.GetWidth(), fit_size_.GetWidth());
      if (!Is(kRescaleFitHeight)) y = x;
    }
  }

  if (Is(kRescaleFitHeight)) {
    if (size.GetHeight() > fit_size_.GetHeight() || Is(kRescaleEnlarge)) {
      y = CalcScale(size.GetHeight(), fit_size_.GetHeight());
      if (!Is(kRescaleFitWidth)) x = y;
    }
  }

  if (Is(kRescaleFitAll)) {
    if (x < y) {
      y = x;
    } else {
      x = y;
    }
  }
}

void Rescaler::GetScale(const SBitmap &bitmap, double &x, double &y) {
  if (bitmap.IsOk()) GetScale(bitmap.GetImage().GetSize(), x, y);
}

void Rescaler::GetScale(const wxImage &image, double &x, double &y) {
  if (image.IsOk()) GetScale(image.GetSize(), x, y);
}

}  // namespace bitmap

}  // namespace fmr
