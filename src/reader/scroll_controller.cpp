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

#include "fmr/reader/scroll_controller.h"

#include "fmr/common/dimension.h"

namespace fmr {

namespace reader {

void ScrollController::Create(ScrolledImageWindow *window) {
  window_ = window;
  window_->Bind(kEventPageChanged, &ScrollController::OnPageChanged, this);
}

void ScrollController::Scroll(const wxPoint &pos) { window_->Scroll(pos); }

void ScrollController::Scroll(SBitmap *bitmap, ScrollBitmapFlags flags) {
  Scroll(GetPosition(bitmap, flags));
}

wxPoint ScrollController::GetPosition(const SBitmap *bitmap,
                                      ScrollBitmapFlags flags) const {
  wxPoint pos = bitmap->GetPosition();

  if (flags == kScrollToBitmapRightTop || flags == kScrollToBitmapRightBottom) {
    pos.x = bitmap->GetX() + bitmap->GetWidth() -
            window_->GetClientSize().GetWidth();
  }

  if (flags == kScrollToBitmapLeftBottom ||
      flags == kScrollToBitmapRightBottom) {
    pos.y = bitmap->GetY() + bitmap->GetWidth() -
            window_->GetClientSize().GetWidth();
  }

  return pos;
}

void ScrollController::GetFirstShown(const SBitmap *&bitmap,
                                     wxPoint *pos) const {
  if (window_->GetPage()) {
    for (const auto &it : window_->GetPage()->GetBitmap()) {
      if (it.IsShown(window_->GetViewStart(), window_->GetClientSize())) {
        bitmap = &it;
        if (pos) *pos = it.GetPosition();
        break;
      }
    }
  }
}

void ScrollController::SetFirstShown(const SBitmap *bitmap,
                                     const wxPoint *pos) {
  wxPoint scroll_pos = bitmap->GetPosition();

  if (pos) {
    for (const auto &it : window_->GetPage()->GetBitmap()) {
      if (&it == bitmap) {
        if (it.GetPosition() != *pos) {
          scroll_pos.x -= pos->x;
          scroll_pos.y -= pos->y;
          break;
        }
      }
    }
  }

  window_->AdjustScrollbars();
  window_->Scroll(scroll_pos);
}

int ScrollController::GetStep(wxDirection direction) const {
  int step = 0;

  if (direction == wxUP || direction == wxLEFT) {
    step = -1;
  } else if (direction == wxDOWN || direction == wxRIGHT) {
    step = 1;
  }

  if (dimension::GetOrient(direction) == wxHORIZONTAL && IsReadFromRight()) {
    step *= -1;
  }

  return step;
}

ScrollBitmapFlags ScrollController::GetBitmapFlags(int step) const {
  if (step == ChangeForward) {
    if (IsReadFromRight())
      return kScrollToBitmapRightTop;
    else
      return kScrollToBitmapLeftTop;
  } else if (step == ChangeBackward) {
    if (IsReadFromRight()) {
      return kScrollToBitmapLeftBottom;
    } else
      return kScrollToBitmapRightBottom;
  }
  return kScrollToBitmapLeftTop;
}

wxPoint ScrollController::GetStartPosition(wxDirection direction) const {
  if (!GetWindow()->GetPage()) return wxDefaultPosition;

  if (GetWindow()->GetPage()->GetBitmap().empty()) return wxDefaultPosition;

  int step = GetStep(direction);
  const SBitmap *bitmap = nullptr;
  ScrollBitmapFlags flags = GetBitmapFlags(step);

  if (step == ChangeForward) {
    bitmap = &GetWindow()->GetPage()->GetBitmap().front();
  } else if (step == ChangeBackward) {
    bitmap = &GetWindow()->GetPage()->GetBitmap().back();
  } else
    return wxDefaultPosition;

  return GetPosition(bitmap, flags);
}

void ScrollController::ResetScroll(wxDirection direction) {
  Scroll(GetStartPosition(direction));
}

}  // namespace reader

}  // namespace fmr
