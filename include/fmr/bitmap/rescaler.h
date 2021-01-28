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

#ifndef FMR_BITMAP_RESCALER
#define FMR_BITMAP_RESCALER

#include "fmr/bitmap/bmp.h"
#include "fmr/common/bitmask.h"

namespace fmr {

namespace bitmap {

enum RescalerFlags {
  kRescaleNone = 0x00,
  kRescaleFitWidth = 0x01,
  kRescaleFitHeight = 0x02,
  kRescaleFitAll = kRescaleFitWidth | kRescaleFitHeight,
  kRescaleEnlarge = 0x04
};

DEFINE_BITMASK_TYPE(RescalerFlags);

class Rescaler {
 protected:
  wxSize max_size_, min_size_;
  RescalerFlags flags_;

 public:
  Rescaler() {}
  Rescaler(RescalerFlags flags) { flags_ = flags; };
  Rescaler(const Rescaler &other) {
    flags_ = other.flags_;
    max_size_ = other.max_size_;
  }

  bool Is(RescalerFlags flags) { return flags_ & flags; }

  float CalcScale(int before, int after) {
    return float(after) / float(before);
  }

  void DoRescale(wxSize &size) {
    double scale_x = 1, scale_y = 1;
    GetScale(size, scale_x, scale_y);
    size.Scale(scale_x, scale_y);
  }

  void DoRescale(SBitmap &bitmap) {
    double scale_x = 1, scale_y = 1;
    GetScale(bitmap, scale_x, scale_y);
    bitmap.SetScale(scale_x, scale_y);
  };

  void DoRescale(wxImage &image) {
    double x = 1, y = 1;
    GetScale(image, x, y);
    wxSize size = image.GetSize();
    size.Scale(x, y);
    image.Rescale(size.GetWidth(), size.GetHeight());
  }

  void GetScale(const wxSize &size, double &x, double &y) {
    if (Is(kRescaleFitWidth)) {
      if (size.GetWidth() > max_size_.GetWidth() || Is(kRescaleEnlarge)) {
        x = CalcScale(size.GetWidth(), max_size_.GetWidth());
        if (!Is(kRescaleFitHeight)) y = x;
      }
    }

    if (Is(kRescaleFitHeight)) {
      if (size.GetHeight() > max_size_.GetHeight() || Is(kRescaleEnlarge)) {
        y = CalcScale(size.GetHeight(), max_size_.GetHeight());
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

  void GetScale(const SBitmap &bitmap, double &x, double &y) {
    GetScale(bitmap.GetBitmap().GetSize(), x, y);
  }

  void GetScale(const wxImage &image, double &x, double &y) {
    GetScale(image.GetSize(), x, y);
  }

  void SetMaximumSize(const wxSize &size) { max_size_ = size; }

  virtual ~Rescaler(){};
};

}  // namespace bitmap

}  // namespace fmr

#endif /* end of include guard: FMR_BITMAP_RESCALER */
