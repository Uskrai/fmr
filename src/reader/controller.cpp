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

#include "fmr/reader/controller.h"

#include "fmr/common/dimension.h"
#include "fmr/common/event.h"
#include "fmr/common/string.h"

namespace fmr {

namespace reader {

wxDEFINE_EVENT(kEventOpenFile, wxCommandEvent);

Controller::Controller() {
  window_ = new ScrolledImageWindow();

  position_ctrl_ = std::make_unique<bitmap::PositionCtrl>(
      bitmap::kPositionAlignCenter | bitmap::kPositionVertical);

  rescaler_ = std::make_unique<bitmap::Rescaler>(bitmap::kRescaleNone);

  bitmap_ctrl_ = std::make_unique<bitmap::BitmapPageCtrl>(position_ctrl_.get(),
                                                          rescaler_.get());

  loader_ =
      std::make_unique<bitmap::PageLoader>(this, GetBitmapCtrl(), kLoaderId);

  Bind(thread::kEventImageLoaded, &Controller::OnLoadedImage, this, kLoaderId);

  Bind(bitmap::kEventOpenedStreamFound, &Controller::OnOpenedStreamFound, this,
       kLoaderId);
}

Controller::Controller(wxWindow *parent, wxWindowID id, const wxPoint &pos,
                       const wxSize &size, long style, const wxString &name)
    : Controller() {
  parent_ = parent;
  CreateWindow(parent, id, pos, size, style, name);
}

bool Controller::CreateWindow(wxWindow *parent, wxWindowID id,
                              const wxPoint &pos, const wxSize &size,
                              long style, const wxString &name) {
  bool ret = window_->Create(parent, id, pos, size, style, name);

  event::Bind(window_, kScrollWinEventAll, &Controller::OnWindowScroll, this);
  return ret;
}

bool Controller::Open(const std::string &path) {
  Clear();
  if (loader_->Open(path)) {
    window_->Scroll(0, 0);
    window_->SetVirtualSize(window_->GetClientSize());
    if (loader_->Run()) {
      auto event = wxCommandEvent(kEventOpenFile, window_->GetId());
      event.SetString(String::FromString<wxString>(path).c_str());
      wxPostEvent(GetParent(), event);
      return true;
    }
  }

  return false;
}

void Controller::SetWindow(ScrolledImageWindow *window) {
  if (window_) window_->Destroy();

  window_ = window;

  if (window_) {
  }
}

void Controller::SetPositionFlags(bitmap::PositionFlags flags) {
  position_ctrl_->SetFlags(flags);
  AdjustBitmap();
}

void Controller::SetScaleFlags(bitmap::RescalerFlags flags) {
  rescaler_->SetFlags(flags);
  AdjustBitmap();
}

void Controller::AdjustBitmap() {
  bitmap::BitmapPage *page = GetBitmapCtrl()->GetBitmapPage();
  if (page) {
    wxPoint first_shown_pos;
    const SBitmap *first_shown = nullptr;
    for (const auto &it : page->GetBitmap()) {
      if (it.IsShown(window_->GetViewStart(), window_->GetClientSize())) {
        first_shown_pos = it.GetPosition();
        first_shown = &it;
        break;
      }
    }

    position_ctrl_->SetMinimumSize(window_->GetClientSize());
    rescaler_->SetMaximumSize(window_->GetClientSize());
    GetBitmapCtrl()->AdjustBitmap();

    wxSize size = GetBitmapCtrl()->GetSize(page);

    window_->SetVirtualSize(size);
    position_ctrl_->SetWindowSize(size);
    GetBitmapCtrl()->RecalcPosition(page);

    if (window_->GetVirtualSize().GetHeight() >
            window_->GetClientSize().GetHeight() &&
        window_->GetVirtualSize().GetWidth() >
            window_->GetClientSize().GetWidth()) {
      for (const auto &it : page->GetBitmap()) {
        if (&it == first_shown) {
          if (it.GetPosition() != first_shown_pos) {
            wxPoint scroll = window_->GetViewStart();
            scroll.x += it.GetX() - first_shown_pos.x;
            scroll.y += it.GetY() - first_shown_pos.y;

            window_->AdjustScrollbars();
            window_->Scroll(scroll);
            break;
          }
        }
      }
    }
    window_->Refresh();
  }
  window_->SetBitmapPage(GetBitmapCtrl()->GetBitmapPage());
}

bool Controller::IsOnEdge(const wxOrientation &orient, int pos) const {
  int bottom_edge = window_->GetScrollRangeLimit(orient);
  int scroll_pos = window_->GetScrollPos(orient);
  bool is_top = scroll_pos == 0;

  bool is_bottom = scroll_pos == bottom_edge;
  is_bottom = is_bottom || window_->GetScrollRange(orient) <
                               dimension::Get(window_->GetClientSize(), orient);

  bool is_below_bottom = is_bottom && pos >= bottom_edge;
  bool is_over_top = is_top && pos <= 0;
  bool is_edge = (is_top && is_over_top) || (is_bottom && is_below_bottom);

  return is_edge;
}

void Controller::OnLoadedImage(thread::LoadImageEvent &event) {
  GetBitmapCtrl()->AddBitmap(event.GetBitmap(),
                             loader_->GetStreamPage(event.GetStream()),
                             loader_->GetStreamPosInPage(event.GetStream()));
  AdjustBitmap();
  event.Skip();
}

void Controller::OnOpenedStreamFound(wxCommandEvent &event) {
  GetBitmapCtrl()->SetBitmapPage(loader_->GetOpenedPagePos());
  AdjustBitmap();
}

int Controller::GetStep(wxDirection direction) {
  int step = 0;

  if (direction == wxUP || direction == wxLEFT) {
    step = -1;
  } else if (direction == wxDOWN || direction == wxRIGHT) {
    step = 1;
  }

  if (dimension::GetOrient(direction) == wxHORIZONTAL && is_read_from_right_) {
    step *= -1;
  }

  return step;
}

int Controller::GetStartX(wxDirection direction) {
  auto vec_bitmap = GetBitmapCtrl()->GetBitmap();

  if (vec_bitmap.empty()) return 0;

  if (direction == wxRIGHT && is_read_from_right_) {
    direction = wxLEFT;
  } else if (direction == wxLEFT && is_read_from_right_) {
    direction = wxRIGHT;
  }

  auto get_center = [](const SBitmap *bitmap, const wxSize &size) -> int {
    return (size.GetWidth() / 2 - bitmap->GetWidth() / 2) / 2;
  };

  if (direction == wxUP || direction == wxLEFT) {
    auto bitmap = vec_bitmap.back();
    if (bitmap) {
      if (is_read_from_right_) {
        if (bitmap->GetWidth() < window_->GetVirtualSize().GetWidth()) {
          return get_center(bitmap, window_->GetVirtualSize());
        }
      } else {
        return bitmap->GetX() + bitmap->GetWidth() -
               window_->GetClientSize().GetWidth();
      }
    }
  } else if (direction == wxDOWN || direction == wxRIGHT) {
    auto bitmap = vec_bitmap.at(0);
    if (bitmap) {
      if (is_read_from_right_) {
        return bitmap->GetX() + bitmap->GetWidth() -
               window_->GetClientSize().GetWidth();
      } else {
        if (bitmap->GetWidth() < window_->GetVirtualSize().GetWidth()) {
          return get_center(bitmap, window_->GetVirtualSize());
        }
      }
    }
  }

  return window_->GetViewStart().x;

  return 0;
}
int Controller::GetStartY(wxDirection direction) {
  auto vec_bitmap = GetBitmapCtrl()->GetBitmap();
  if (vec_bitmap.empty()) return 0;

  if (direction == wxRIGHT && is_read_from_right_) {
    direction = wxLEFT;
  } else if (direction == wxLEFT && is_read_from_right_) {
    direction = wxRIGHT;
  }

  if (direction == wxUP || direction == wxLEFT) {
    return window_->GetVirtualSize().GetHeight() -
           window_->GetClientSize().GetHeight();
  } else if (direction == wxDOWN || direction == wxRIGHT) {
    return 0;
  }

  return 0;
}

wxPoint Controller::GetStartPosition(wxDirection direction) {
  wxPoint pos = window_->GetViewStart();

  return pos;
}

void Controller::ResetScroll(wxDirection direction) {
  window_->Scroll(-1, GetStartY(direction));
  window_->Scroll(GetStartX(direction), -1);
}

bool Controller::ChangePage(wxDirection direction) {
  int step = GetStep(direction);

  size_t idx = GetBitmapCtrl()->GetPagePos() + step;

  for (const auto &it : GetBitmapCtrl()->GetBitmap()) {
    if (!it->IsLoaded()) return true;
  }

  if (GetBitmapCtrl()->IsPageExist(idx)) {
    GetBitmapCtrl()->SetBitmapPage(idx);
    AdjustBitmap();
    window_->AdjustScrollBar();
    ResetScroll(direction);
    return true;
  }
  return false;
}

bool Controller::ChangeFolder(wxDirection direction) {
  int step = GetStep(direction);

  AbstractHandler *handler = loader_->GetHandler();
  if (handler && handler->GetParent()) {
    handler->GetParent()->Traverse();
    std::string path = handler->GetFromCurrent(step);
    if (path != "") {
      return Open(path);
    }
  }
  return false;
}

bool Controller::Change(wxDirection direction) {
  if (ChangePage(direction)) {
    return true;
  }

  return ChangeFolder(direction);
}

void Controller::OnWindowScroll(wxScrollWinEvent &event) {
  if (event.GetEventType() == wxEVT_SCROLLWIN_LINEUP ||
      event.GetEventType() == wxEVT_SCROLLWIN_LINEDOWN) {
    window_->AdjustScrollBar();
    wxOrientation orient = dimension::GetOrient(event.GetOrientation());
    if (IsOnEdge(orient, event.GetPosition())) {
      wxDirection direction =
          dimension::GetDirection(orient, event.GetPosition());
      if (Change(direction)) return;
    }
  }
  event.Skip();
}

void Controller::Clear() {
  window_->ClearBitmap();
  loader_->Clear();
  GetBitmapCtrl()->Clear();
}

}  // namespace reader

}  // namespace fmr
