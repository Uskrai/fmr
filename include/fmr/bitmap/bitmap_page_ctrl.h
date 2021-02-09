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

#ifndef FMR_BITMAP_PAGED_BITMAP_CTRL
#define FMR_BITMAP_PAGED_BITMAP_CTRL

#include "fmr/bitmap/bitmap_ctrl.h"
#include "fmr/bitmap/bitmap_page.h"

namespace fmr {

namespace bitmap {

class BitmapPageCtrl : public BitmapCtrl {
  std::vector<std::unique_ptr<BitmapPage>> vec_page_;
  size_t bitmap_per_page_ = -1;
  size_t curr_page_ = -1;

 public:
  BitmapPageCtrl(ScrolledImageWindow *window, PositionCtrl *pos_ctrl,
                 Rescaler *rescaler, size_t bitmap_per_page = -1);

  void SetBitmapPerPage(size_t size) { bitmap_per_page_ = size; };

  void SetBitmapPage(size_t page);
  BitmapPage *GetBitmapPage();

  using BitmapCtrl::RecalcPosition;
  void RecalcPosition(BitmapPage *page);

  using BitmapCtrl::GetSize;
  wxSize GetSize(BitmapPage *page);

  std::unique_ptr<BitmapPage> &GetPage(size_t idx) { return vec_page_.at(idx); }
  const std::unique_ptr<BitmapPage> &GetPage(size_t idx) const {
    return vec_page_.at(idx);
  }

  std::vector<std::unique_ptr<BitmapPage>> &GetAllPage() { return vec_page_; }
  const std::vector<std::unique_ptr<BitmapPage>> &GetAllPage() const {
    return vec_page_;
  }

  size_t CountPage() const { return vec_page_.size(); }
  size_t GetPagePos() const { return curr_page_; }

  bool IsPageExist(size_t idx) { return idx < CountPage(); }

  void AddBitmap(const SBitmap &bitmap, size_t page_pos, size_t bitmap_pos);

  void EnlargePage(size_t size);
  void EnlargeBitmapPage(size_t page_pos, size_t size);

  bool GoToPage(size_t idx);
  bool ChangePage(int step);

  void Clear();

 private:
  void OnImageLoaded(queue::LoadImageEvent &event);
};

}  // namespace bitmap

}  // namespace fmr

#endif /* end of include guard: FMR_BITMAP_PAGED_BITMAP_CTRL */
