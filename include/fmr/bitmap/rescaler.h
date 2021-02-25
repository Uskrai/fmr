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
  kRescaleEnlarge = 0x04,
  kRescaleShrink = 0x08
};

DEFINE_BITMASK_TYPE(RescalerFlags)

class Rescaler {
 protected:
  wxSize fit_size_, min_size_;
  RescalerFlags flags_;

 public:
  Rescaler() {}
  Rescaler(RescalerFlags flags) { flags_ = flags; };
  Rescaler(const Rescaler &other) {
    flags_ = other.flags_;
    fit_size_ = other.fit_size_;
  }

  void SetFlags(RescalerFlags flags) { flags_ = flags; }
  bool Is(RescalerFlags flags) { return flags_ & flags; }

  float CalcScale(int before, int after) {
    return float(after) / float(before);
  }

  void DoRescale(wxSize &size);
  void DoRescale(SBitmap &bitmap);
  void DoRescale(wxImage &image);

  void GetScale(const wxSize &size, double &x, double &y);

  void GetScale(const SBitmap &bitmap, double &x, double &y);
  void GetScale(const wxImage &image, double &x, double &y);

  [[deprecated("Use SetFitSize instead")]] void SetMaximumSize(
      const wxSize &size) {
    fit_size_ = size;
  }
  void SetFitSize(const wxSize &size) { fit_size_ = size; }
  [[deprecated("Typo")]] wxSize GetFitSise() const { return fit_size_; }
  wxSize GetFitSize() const { return fit_size_; }

  bool ShouldRescale(const wxSize &size, RescalerFlags flags);

  virtual ~Rescaler(){};
};

}  // namespace bitmap

}  // namespace fmr

#endif /* end of include guard: FMR_BITMAP_RESCALER */
