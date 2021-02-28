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

#ifndef FMR_BITMAP_BITMAP_VECTOR
#define FMR_BITMAP_BITMAP_VECTOR

#include <vector>

#include "fmr/bitmap/bmp.h"

namespace fmr {

namespace bitmap {

class BitmapVector {
 private:
  std::vector<SBitmap> vec_bitmap_;
  wxSize size_;

 public:
  BitmapVector() {}
  BitmapVector(std::vector<SBitmap> vec_bitmap) { SetBitmap(vec_bitmap_); }

  void SetBitmap(std::vector<SBitmap> vec_bitmap) { vec_bitmap_ = vec_bitmap; }
  void PushBack(const SBitmap &bitmap) { vec_bitmap_.push_back(bitmap); }
  void PushFront(const SBitmap &bitmap) { InsertBitmap(0, bitmap); }

  void InsertBitmap(size_t idx, const SBitmap &bitmap) {
    size_t i = 0;
    for (auto it = vec_bitmap_.begin(); it != vec_bitmap_.end(); ++it, ++i)
      if (idx == i) vec_bitmap_.insert(it, bitmap);
  }

  std::vector<SBitmap> &GetBitmap() { return vec_bitmap_; }
  const std::vector<SBitmap> &GetBitmap() const { return vec_bitmap_; }

  const wxSize &GetBitmapSize() const { return size_; }
  void SetBitmapSize(const wxSize &size) { size_ = size; }

  size_t Size() { return vec_bitmap_.size(); }

  void Draw(wxDC &dc, const wxRect &rect) {
    for (auto &it : vec_bitmap_) {
      it.Draw(dc, rect);
    }
  }

  void SetVisibleArea(const wxPoint &pos, const wxSize &size) {
    SetVisibleArea(wxRect(pos, size));
  }
  void SetVisibleArea(const wxRect &rect) {
    for (auto &it : vec_bitmap_) it.SetVisibleArea(rect);
  }

  void Draw(wxDC &dc, const wxPoint &view_start, const wxSize &size) {
    return Draw(dc, wxRect(view_start, size));
  }

  void ClearImage() {
    for (auto &it : GetBitmap()) {
      it.SetImage(wxImage());
    }
  }
};

typedef BitmapVector BitmapPage;

}  // namespace bitmap

}  // namespace fmr

#endif /* end of include guard: FMR_BITMAP_BITMAP_VECTOR */
