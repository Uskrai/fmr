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

#include <fmr/window/scrolled_image.h>
#include <wx/dc.h>

#include "fmr/common/dimension.h"
#include "fmr/common/event.h"

namespace fmr {

wxDEFINE_EVENT(kEventPageChanged, wxCommandEvent);

bool ScrolledImageWindow::Create(wxWindow *parent, wxWindowID id,
                                 const wxPoint &pos, const wxSize &size,
                                 long style, const wxString &name) {
  bool ret = ScrolledWindow::Create(parent, id, pos, size, style, name);
  event::Bind(kScrollWinEventAll, &ScrolledImageWindow::OnScroll, this);

  return ret;
}

void ScrolledImageWindow::DrawBitmap(wxDC &dc) {
  if (GetPage()) {
    GetPage()->Draw(dc, GetViewStart(), GetClientSize());
  }
}

void ScrolledImageWindow::DoScroll(int x, int y) {
  wxPoint pos = GetViewStart();
  ScrolledWindow::DoScroll(x, y);

  if (page_ && pos != GetViewStart())
    page_->SetVisibleArea(GetViewStart(), GetClientSize());
}

void ScrolledImageWindow::OnScroll(wxScrollWinEvent &event) {
  wxOrientation orient = dimension::GetOrient(event.GetOrientation());

  wxPoint scroll_pos = GetViewStart();
  if (event.GetEventType() == wxEVT_SCROLLWIN_TOP) {
    scroll_pos = wxPoint(-1, 0);
  }
  if (event.GetEventType() == wxEVT_SCROLLWIN_BOTTOM) {
    scroll_pos = wxPoint(-1, GetVirtualSize().GetHeight());
  } else if (event.GetEventType() == wxEVT_SCROLLWIN_PAGEUP) {
    dimension::Set(
        scroll_pos, orient,
        dimension::Get(scroll_pos, orient) - GetScrollPageSize(orient));
  } else if (event.GetEventType() == wxEVT_SCROLLWIN_PAGEDOWN) {
    dimension::Set(
        scroll_pos, orient,
        dimension::Get(scroll_pos, orient) + GetScrollPageSize(orient));
  } else {
    dimension::Set(scroll_pos, orient, event.GetPosition());
  }
  DoScroll(scroll_pos.x, scroll_pos.y);
}

void ScrolledImageWindow::OnDraw(wxDC &dc) {
  DrawBitmap(dc);
  if (decorator_) decorator_->DrawDecorator(dc);
}

void ScrolledImageWindow::SetBitmapPage(bitmap::BitmapPage *page) {
  page_ = page;
  if (page_) page_->SetVisibleArea(GetViewStart(), GetClientSize());
  if (!IsBeingDeleted()) {
    QueueEvent(new wxCommandEvent(kEventPageChanged, GetId()));
  }
}

}  // namespace fmr
