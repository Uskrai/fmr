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

#include "fmr/bitmap/bitmap_ctrl.h"

#include "fmr/bitmap/bitmap_vector_event.h"
#include "fmr/bitmap/rescaler.h"
#include "fmr/common/vector.h"
#include "fmr/window/scrolled_image.h"

namespace fmr {

namespace bitmap {

wxDEFINE_EVENT(kEventBitmapChanging, BitmapVectorEvent);
wxDEFINE_EVENT(kEventBitmapChanged, BitmapVectorEvent);
wxDEFINE_EVENT(kEventBitmapNotChanged, BitmapVectorEvent);

std::vector<SBitmap *> BitmapPageToBitmapPtr(BitmapVector *vec) {
  std::vector<SBitmap *> vec_ptr;

  if (vec) {
    for (auto &it : vec->GetBitmap()) {
      vec_ptr.push_back(&it);
    }
  }
  return vec_ptr;
}
//
// PositionVector MakePositionVector(const std::vector<SBitmap> &page) {
// std::vector<PositionItemRef<SBitmap>> vec(page.begin(), page.end());
// return PositionVector(vec.begin(), vec.end());
// }
//
// PositionVectorConst MakePositionVectorConst(const std::vector<SBitmap> &page)
// { std::vector<PositionItemConstRef<SBitmap>> vec(page.begin(), page.end());
// return PositionVectorConst(vec.begin(), vec.end());
// }
//
BitmapCtrl::BitmapCtrl(ScrolledImageWindow *window, PositionCtrl *pos_ctrl,
                       Rescaler *rescaler) {
  window_ = window;
  pos_ctrl_ = pos_ctrl;
  rescaler_ = rescaler;
  Bind(kEventBitmapChanging, &BitmapCtrl::DoChangeBitmapVector, this);
}

void BitmapCtrl::RecalcPosition() {
  return RecalcPosition(GetBitmapVec()->GetBitmap());
  // std::vector<PositionItemRef<SBitmap>>
  // vec(GetBitmapVec()->GetBitmap().begin(),
  // GetBitmapVec()->GetBitmap().end());
  // std::vector<PositionItemRef<SBitmap> *> vec_ptr =
  // Vector::ConvertToPtr(vec); std::vector<PositionItem *>
  // vec_item(vec_ptr.begin(), vec_ptr.end());
}

wxSize BitmapCtrl::GetSize() const {
  return pos_ctrl_->GetSize(GetBitmapVec()->GetBitmap());
}

std::vector<const SBitmap *> BitmapCtrl::GetVectorPtr() const {
  std::vector<const SBitmap *> bmp_ptr;
  if (GetBitmapVec()) {
    for (const auto &it : GetBitmapVec()->GetBitmap()) {
      bmp_ptr.push_back(&it);
    }
  }
  return bmp_ptr;
}

std::vector<SBitmap *> BitmapCtrl::GetVectorPtr() {
  std::vector<SBitmap *> bmp_ptr;
  if (GetBitmapVec()) {
    for (auto &it : GetBitmapVec()->GetBitmap()) {
      bmp_ptr.push_back(&it);
    }
  }
  return bmp_ptr;
}

void BitmapCtrl::OnWindowSize(wxSizeEvent &event) {}

void BitmapCtrl::SetBitmapVec(BitmapVector *bmp_vec) {
  BitmapVectorEvent event(kEventBitmapChanging, wxID_ANY);
  event.SetBitmapVec(bmp_vec);
  wxPostEvent(this, event);
}

void BitmapCtrl::DoChangeBitmapVector(BitmapVectorEvent &event) {
  if (event.IsAllowed()) {
    bmp_vec_ = event.GetBitmapVec();
    GetWindow()->SetBitmapPage(GetBitmapVec());

    // AdjustBitmap();

    auto temp = event;
    temp.SetEventType(kEventBitmapChanged);
    wxPostEvent(this, temp);
  } else {
    auto temp = event;
    temp.SetEventType(kEventBitmapNotChanged);
    wxPostEvent(this, temp);
  }
}

void BitmapCtrl::RecalcPosition(std::vector<SBitmap> &bitmap) const {
  // pos_ctrl_->RecalcPosition(bitmap);
}

wxSize BitmapCtrl::GetSize(const std::vector<SBitmap> &bitmap) const {
  return wxSize();
  // return pos_ctrl_->GetSize(bitmap);
}

void BitmapCtrl::AdjustBitmap() {
  if (GetBitmapVec()) {
    GetPosCtrl()->SetMinimumSize(GetWindow()->GetClientSize());
    GetRescaler()->SetFitSize(GetWindow()->GetClientSize());

    for (auto &it : GetBitmapVec()->GetBitmap()) {
      GetRescaler()->DoRescale(it);
    }

    RecalcPosition();
    GetPosCtrl()->SetWindowSize(GetSize());
    RecalcPosition();
  }
}

void BitmapCtrl::Clear() { bmp_vec_ = nullptr; }

}  // namespace bitmap

}  // namespace fmr
