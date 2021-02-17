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

#include <fmr/window/decorator.h>

namespace fmr {

void WindowDecoratorList::AddDecorator(WindowDecorator *decorator) {
  decorator->Bind(wxEVT_TIMER, &WindowDecoratorList::OnDecoratorHide, this);
  list_decorator_.push_back(decorator);
}

void WindowDecoratorList::OnDecoratorHide(wxTimerEvent &event) {
  GetWindow()->Refresh();
  event.Skip();
}

void WindowDecoratorList::Draw(wxDC &dc) {
  for (auto &it : list_decorator_) {
    it->Draw(dc, area_);
  }
}

WindowDecorator::WindowDecorator() {
  Bind(wxEVT_TIMER, &WindowDecorator::OnHideTimer, this);
}

void WindowDecorator::Show(int miliseconds, bool one_shot) {
  is_shown_ = true;
  hide_timer_.Start(miliseconds, one_shot);
}

void WindowDecorator::ShowOnce(const int miliseconds) {
  is_shown_ = true;
  hide_timer_.StartOnce(miliseconds);
}

void WindowDecorator::Draw(wxDC &dc, const wxRect &area) {
  if (IsShown()) return OnDraw(dc, area);
}

void WindowDecorator::SetParent(WindowDecoratorList *window) {
  parent_ = window;
  window->AddDecorator(this);
}

void WindowDecorator::OnHideTimer(wxTimerEvent &event) { is_shown_ = false; }

WindowDecoratorList *WindowDecorator::GetParent() { return parent_; }

}  // namespace fmr
