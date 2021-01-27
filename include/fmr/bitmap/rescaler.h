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

#include <wx/gdicmn.h>
#include <wx/image.h>

namespace fmr {

namespace bitmap {

enum RescalerFlags {
  kRescaleNone = 0x00,
  kRescaleFitWidth = 0x01,
  kRescaleFitHeight = 0x02,
  kRescaleFitAll = kRescaleFitWidth | kRescaleFitHeight,
};

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

  void DoRescale(wxImage &image) {
    double scale_x = 1, scale_y;

    GetScale(image, scale_x, scale_y);

    wxSize size = image.GetSize();

    size.Scale(scale_x, scale_y);
    image.Rescale(size.GetWidth(), size.GetHeight());
  };

  void GetScale(const wxImage &image, double &x, double &y) {
    if (Is(kRescaleFitWidth)) {
      x = CalcScale(image.GetWidth(), max_size_.GetWidth());
      if (!Is(kRescaleFitHeight)) y = x;
    }

    if (Is(kRescaleFitHeight)) {
      y = CalcScale(image.GetHeight(), max_size_.GetHeight());
      if (!Is(kRescaleFitWidth)) x = y;
    }

    if (Is(kRescaleFitAll)) {
      if (x < y) {
        y = x;
      } else {
        x = y;
      }
    }
  }

  void SetMaximumSize(const wxSize &size) { max_size_ = size; }

  virtual ~Rescaler(){};
};

}  // namespace bitmap

}  // namespace fmr

#endif /* end of include guard: FMR_BITMAP_RESCALER */
