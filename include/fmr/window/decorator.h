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

#ifndef FMR_WINDOW_DECORATOR
#define FMR_WINDOW_DECORATOR

#include <wx/timer.h>
#include <wx/window.h>

#include <vector>

namespace fmr {

class WindowDecorator;

class WindowDecoratorList {
 protected:
  std::vector<WindowDecorator *> list_decorator_;
  wxWindow *window_ = nullptr;

 public:
  WindowDecoratorList(){};
  WindowDecoratorList(wxWindow *window) { window_ = window; }

  wxWindow *GetWindow() { return window_; }
  void AddDecorator(WindowDecorator *decorator);
  void DrawDecorator(wxDC &dc);
};

class WindowDecorator : public wxEvtHandler {
 protected:
  WindowDecoratorList *parent_ = nullptr;
  wxTimer hide_timer_ = wxTimer(this);

 public:
  WindowDecorator() {}
  WindowDecorator(WindowDecoratorList *window);
  virtual ~WindowDecorator() {}

  void SetParent(WindowDecoratorList *window);
  WindowDecoratorList *GetParent();

  virtual void Draw(wxDC &dc) = 0;

  void OnHideTimer(wxTimerEvent &event);
};

}  // namespace fmr

#endif /* end of include guard: FMR_WINDOW_DECORATOR */
