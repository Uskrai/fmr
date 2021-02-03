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

  EnlargePage(page_pos + 1);
  EnlargeBitmapPage(page_pos, bitmap_pos + 1);

  auto page = vec_page_.at(page_pos).get();

  page->GetBitmap().at(bitmap_pos) = bitmap;

  std::vector<SBitmap *> page_ptr;
  for (auto &it : page->GetBitmap()) {
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
    for (auto &it : vec_page_.at(page)->GetBitmap()) page_ptr.push_back(&it);
    RecalcPosition(page_ptr);
    SetBitmap(page_ptr);
  }
}

BitmapPage *BitmapPageCtrl::GetBitmapPage() {
  if (curr_page_ < vec_page_.size()) {
    return vec_page_.at(curr_page_).get();
  }
  return nullptr;
}

void BitmapPageCtrl::RecalcPosition(BitmapPage *page) {
  return RecalcPosition(BitmapPageToBitmapPtr(page));
}

void BitmapPageCtrl::EnlargePage(size_t size) {
  while (GetAllPage().size() < size) {
    GetAllPage().push_back(std::make_unique<BitmapPage>());
  }
}

void BitmapPageCtrl::EnlargeBitmapPage(size_t page_pos, size_t size) {
  EnlargePage(page_pos + 1);

  auto page = GetAllPage().at(page_pos).get();

  if (page->GetBitmap().size() < size) page->GetBitmap().resize(size);
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
