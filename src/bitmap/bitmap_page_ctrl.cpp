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

#include "fmr/bitmap/bitmap_page_ctrl.h"

namespace fmr {

namespace bitmap {

std::vector<SBitmap *> BitmapPageToBitmapPtr(BitmapPage *page) {
  std::vector<SBitmap *> vec_ptr;

  if (page) {
    for (auto &it : page->GetBitmap()) {
      vec_ptr.push_back(&it);
    }
  }
  return vec_ptr;
}

BitmapPageCtrl::BitmapPageCtrl(PositionCtrl *pos_ctrl, Rescaler *rescaler,
                               size_t bitmap_per_page)
    : BitmapCtrl(pos_ctrl, rescaler) {
  SetBitmapPerPage(bitmap_per_page);
}

void BitmapPageCtrl::AddBitmap(const SBitmap &bitmap, size_t page_pos,
                               size_t bitmap_pos) {
  if (page_pos == size_t(-1) || bitmap_pos == size_t(-1)) {
    return;
  }

  while (vec_page_.size() <= page_pos) {
    vec_page_.push_back(BitmapPage());
  }

  BitmapPage &page = vec_page_.at(page_pos);

  while (page.Size() <= bitmap_pos) {
    page.GetBitmap().push_back(SBitmap());
  }

  page.GetBitmap().at(bitmap_pos) = bitmap;

  std::vector<SBitmap *> page_ptr;
  for (auto &it : page.GetBitmap()) {
    page_ptr.push_back(&it);
  }
  RecalcPosition(page_ptr);

  if (page_pos == curr_page_) {
    SetBitmapPage(curr_page_);
  }
}

void BitmapPageCtrl::SetBitmapPage(size_t page) {
  curr_page_ = page;

  if (size_t(page) < vec_page_.size()) {
    std::vector<SBitmap *> page_ptr;
    for (auto &it : vec_page_.at(page).GetBitmap()) page_ptr.push_back(&it);
    RecalcPosition(page_ptr);
    SetBitmap(page_ptr);
  }
}

BitmapPage *BitmapPageCtrl::GetBitmapPage() {
  if (curr_page_ < vec_page_.size()) {
    return &vec_page_.at(curr_page_);
  }
  return nullptr;
}

void BitmapPageCtrl::RecalcPosition(BitmapPage *page) {
  return RecalcPosition(BitmapPageToBitmapPtr(page));
}

wxSize BitmapPageCtrl::GetSize(BitmapPage *page) {
  return GetSize(BitmapPageToBitmapPtr(page));
}

void BitmapPageCtrl::Clear() {
  BitmapCtrl::Clear();
  vec_page_.clear();
}

}  // namespace bitmap

}  // namespace fmr
