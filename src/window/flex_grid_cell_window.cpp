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

#include <fmr/window/flex_grid_cell_window.h>

namespace fmr {

FlexGridCellWindow::FlexGridCellWindow(wxWindow *parent, wxWindowID id) {
  Create(parent, id);
}

bool FlexGridCellWindow::Create(wxWindow *parent, wxWindowID id) {
  bool ret = wxWindow::Create(parent, id);
  sizer_ = new wxBoxSizer(wxVERTICAL);
  SetSizer(sizer_);
  BindEvent();
  return ret;
}

void FlexGridCellWindow::BindEvent() {
  Bind(wxEVT_PAINT, &FlexGridCellWindow::OnPaint, this);
  Bind(wxEVT_SIZE, &FlexGridCellWindow::OnSize, this);
}

void FlexGridCellWindow::OnPaint(wxPaintEvent &event) {
  wxPaintDC dc(this);
  DrawBorder(dc);
  event.Skip();
}

void FlexGridCellWindow::OnSize(wxSizeEvent &event) {
  wxRect rect = wxRect(wxPoint(0, 0), event.GetSize());

  if (!cell_window_) return;

  rect = cell_window_->GetRect().CenterIn(rect, wxBOTH);
  cell_window_->SetPosition(rect.GetPosition());
}

wxSize FlexGridCellWindow::GetMinSize() const {
  if (!cell_window_) return wxWindow::GetMinSize();

  return cell_window_->GetSize() +
         wxSize(GetBorderWidth() * 2, GetBorderWidth() * 2);
}

void FlexGridCellWindow::SetCellWindow(wxWindow *cell_window) {
  cell_window_ = cell_window;

  if (!cell_window) return;

  cell_window->Reparent(this);
  SetBackgroundColour(cell_window->GetBackgroundColour());
  SetBackgroundStyle(cell_window->GetBackgroundStyle());
  SetFont(cell_window->GetFont());
  SendSizeEvent();
}

wxRect FlexGridCellWindow::GetCellRect() const {
  wxRect rect;

  if (!cell_window_) return rect;

  rect.SetPosition(cell_window_->GetPosition());
  rect.SetSize(cell_window_->GetSize());
  return rect;
}

void FlexGridCellWindow::DrawBorder(wxDC &dc) {
  if (!is_selected_) return;

  dc.SetBrush(*wxWHITE_BRUSH);
  dc.SetPen(*wxWHITE_PEN);

  wxRect rect(wxPoint(0, 0), GetSize());
  for (int i = 0; i < GetHighlightPenWidth() && i < GetBorderWidth(); i++) {
    dc.DrawLine(rect.GetTopLeft(), rect.GetTopRight());
    dc.DrawLine(rect.GetTopLeft(), rect.GetBottomLeft());
    dc.DrawLine(rect.GetBottomLeft(), rect.GetBottomRight());
    dc.DrawLine(rect.GetTopRight(), rect.GetBottomRight());

    rect.SetTop(rect.GetTop() + 1);
    rect.SetBottom(rect.GetBottom() - 2);
    rect.SetLeft(rect.GetLeft() + 1);
    rect.SetRight(rect.GetRight() - 2);
  }
}

}  // namespace fmr
