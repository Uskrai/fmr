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

#include "fmr/bitmap/rescaler.h"
#include "fmr/common/dimension.h"
#include "fmr/common/event.h"
#include "fmr/common/string.h"
#include "fmr/handler/handler_factory.h"
#include "fmr/thread/load_image_controller.h"

namespace fmr {

namespace reader {

wxDEFINE_EVENT(kEventOpenFile, wxCommandEvent);

Controller::Controller() {
  auto window = new ScrolledImageWindow();
  SetWindow(window);

  position_ctrl_ = std::make_unique<bitmap::PositionCtrl>(
      bitmap::kPositionAlignCenter | bitmap::kPositionVertical);

  rescaler_ = std::make_unique<bitmap::Rescaler>(bitmap::kRescaleNone);

  decorator_ = std::make_unique<DecoratorList>();

  bitmap_ctrl_ = std::make_unique<bitmap::BitmapPageCtrl>(position_ctrl_.get(),
                                                          rescaler_.get());

  loader_ =
      std::make_unique<bitmap::PageLoader>(this, GetBitmapCtrl(), kLoaderId);

  Bind(queue::kEventImageLoaded, &Controller::OnLoadedImage, this, kLoaderId);

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
  bool ret = GetWindow()->Create(parent, id, pos, size, style, name);
  SetWindow(GetWindow());
  return ret;
}

bool Controller::Open(const std::string &path) {
  Clear();
  if (loader_->Open(path)) {
    GetWindow()->Scroll(0, 0);
    GetWindow()->SetVirtualSize(GetWindow()->GetClientSize());
    if (loader_->Run()) {
      auto event = wxCommandEvent(kEventOpenFile, GetWindow()->GetId());
      event.SetString(String::FromString<wxString>(path).c_str());
      wxPostEvent(GetParent(), event);
      return true;
    }
  }

  return false;
}

void Controller::SetWindow(ScrolledImageWindow *window) {
  ScrollController::SetWindow(window);
  window->SetDecorator(decorator_.get());
  event::Bind(GetWindow(), kScrollWinEventAll, &Controller::OnWindowScroll,
              this);
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

    GetFirstShown(first_shown, &first_shown_pos);

    position_ctrl_->SetMinimumSize(GetWindow()->GetClientSize());
    rescaler_->SetFitSize(GetWindow()->GetClientSize());
    GetBitmapCtrl()->AdjustBitmap();

    wxSize size = GetBitmapCtrl()->GetSize(page);

    GetWindow()->SetVirtualSize(size);
    GetWindow()->AdjustScrollbars();
    position_ctrl_->SetWindowSize(size);
    GetBitmapCtrl()->RecalcPosition(page);

    SetFirstShown(first_shown, &first_shown_pos);

    // to make sure the bitmap is really fit
    if (rescaler_->GetFitSise() != GetWindow()->GetClientSize())
      return AdjustBitmap();

    GetWindow()->Refresh();
  }
  GetWindow()->SetBitmapPage(page);
}

void Controller::OnLoadedImage(queue::LoadImageEvent &event) {
  GetBitmapCtrl()->AddBitmap(event.GetBitmap(),
                             loader_->GetStreamPage(event.GetStream()),
                             loader_->GetStreamPosInPage(event.GetStream()));
  AdjustBitmap();
  event.Skip();
}

void Controller::OnOpenedStreamFound(wxCommandEvent &event) {
  GetBitmapCtrl()->EnlargePage(loader_->GetOpenedPagePos() + 1);
  GoToPage(loader_->GetOpenedPagePos());
  AdjustBitmap();
}

bool Controller::GoToPage(size_t idx, wxDirection direction) {
  if (!GetBitmapCtrl()->IsPageExist(idx)) return false;

  GetBitmapCtrl()->SetBitmapPage(idx);
  AdjustBitmap();
  GetWindow()->AdjustScrollBar();
  ResetScroll(direction);
  decorator_->GetPageIndicator()->SetPage(idx);
  decorator_->GetPageIndicator()->SetPageLimit(
      GetBitmapCtrl()->GetAllPage().size());

  decorator_->GetPageIndicator()->SetRect(wxPoint(0, 0),
                                          GetWindow()->GetClientSize());
  return true;
}

bool Controller::ChangePage(wxDirection direction) {
  int step = GetStep(direction);

  size_t idx = GetBitmapCtrl()->GetPagePos() + step;

  for (const auto &it : GetBitmapCtrl()->GetBitmap()) {
    if (!it->IsLoaded()) return true;
  }

  return GoToPage(idx, direction);
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
    GetWindow()->AdjustScrollBar();
    wxOrientation orient = dimension::GetOrient(event.GetOrientation());
    if (IsOnEdge(orient, event.GetPosition())) {
      wxDirection direction =
          dimension::GetDirection(orient, event.GetPosition());
      if (Change(direction)) return;
    }
  }
  GetWindow()->Refresh();
  event.Skip();
}

void Controller::Clear() {
  GetWindow()->ClearBitmap();
  loader_->Clear();
  GetBitmapCtrl()->Clear();
}

}  // namespace reader

}  // namespace fmr
