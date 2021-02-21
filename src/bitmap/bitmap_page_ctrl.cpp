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

#include "fmr/bitmap/bitmap_vector_event.h"

namespace fmr {

namespace bitmap {

wxDEFINE_EVENT(kEventBitmapPageNotFound, BitmapVectorEvent);
wxDEFINE_EVENT(kEventBitmapVectorPushed, BitmapVectorEvent);

BitmapPageCtrl::BitmapPageCtrl(ScrolledImageWindow *window,
                               position::BoxCtrl *pos_ctrl, Rescaler *rescaler,
                               size_t bitmap_per_page)
    : BitmapCtrl(window, pos_ctrl, rescaler) {
  SetBitmapPerPage(bitmap_per_page);
  Bind(kEventBitmapChanging, &BitmapPageCtrl::OnBitmapChanging, this);
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

  RecalcPosition(page->GetBitmap());

  BitmapVectorEvent event(kEventBitmapVectorPushed, wxID_ANY);
  event.SetBitmapVec(page);
  event.SetPagePos(page_pos);
  wxPostEvent(this, event);
}

void BitmapPageCtrl::SetBitmapPage(size_t page) { GoToPage(page); }

BitmapPage *BitmapPageCtrl::GetBitmapPage() {
  if (curr_page_ < vec_page_.size()) {
    return vec_page_.at(curr_page_).get();
  }
  return nullptr;
}

void BitmapPageCtrl::RecalcPosition(BitmapPage *page) {
  return RecalcPosition(page->GetBitmap());
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

wxSize BitmapPageCtrl::GetSize(BitmapPage *page) { return GetSize(page); }

bool BitmapPageCtrl::GoToPage(size_t idx, wxDirection direction) {
  BitmapVectorEvent event(kEventBitmapChanging, wxID_ANY);
  event.SetBitmapVec((IsPageExist(idx)) ? GetPage(idx).get() : nullptr);
  event.SetPagePos(idx);
  event.SetDirection(direction);

  wxPostEvent(this, event);

  return true;
}

void BitmapPageCtrl::OnBitmapChanging(BitmapVectorEvent &event) {
  if (event.IsAllowed()) {
    if (IsPageExist(event.GetPagePos())) {
      curr_page_ = event.GetPagePos();
    } else {
      auto temp = event;
      temp.SetEventType(kEventBitmapPageNotFound);
      wxPostEvent(this, temp);
      event.Veto();
    }
  }
  event.Skip();
}

bool BitmapPageCtrl::ChangePage(int step) {
  size_t idx = GetPagePos() + step;
  for (const auto &it : GetVectorPtr()) {
    if (!it->IsLoaded()) return false;
  }

  return GoToPage(idx);
}

void BitmapPageCtrl::Clear() {
  BitmapCtrl::Clear();
  vec_page_.clear();
}

}  // namespace bitmap

}  // namespace fmr
