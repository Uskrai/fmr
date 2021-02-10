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
  WindowDecoratorList(wxWindow *window) { SetWindow(window); }

  virtual wxWindow *GetWindow() { return window_; }
  virtual void SetWindow(wxWindow *window) { window_ = window; }

  void AddDecorator(WindowDecorator *decorator);
  void DrawDecorator(wxDC &dc, const wxRect &area);

 private:
  void OnDecoratorHide(wxTimerEvent &event);
};

class WindowDecorator : public wxEvtHandler {
 protected:
  WindowDecoratorList *parent_ = nullptr;
  bool is_shown_ = false;
  wxTimer hide_timer_ = wxTimer(this);

  virtual void OnDraw(wxDC &dc, const wxRect &area) = 0;

 public:
  WindowDecorator();
  virtual ~WindowDecorator() {}

  void SetParent(WindowDecoratorList *window);
  WindowDecoratorList *GetParent();

  virtual void Draw(wxDC &dc, const wxRect &area) final;

  bool IsShown() const { return is_shown_; }
  void Show(int miliseconds = -1, bool oneShot = wxTIMER_CONTINUOUS);
  void ShowOnce(const int miliseconds);
  void OnHideTimer(wxTimerEvent &event);
};

}  // namespace fmr

#endif /* end of include guard: FMR_WINDOW_DECORATOR */
