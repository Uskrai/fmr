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

#include <wx/event.h>

class wxMenuItem;
class wxMenu;

#include <unordered_map>

namespace fmr {

namespace gui {

wxDECLARE_EVENT(kEventMenuItemToggled, wxCommandEvent);

class MenuItemToggler : public wxEvtHandler {
  std::unordered_map<int, wxMenuItem *> id_to_menu_item_;
  std::unordered_map<int, int> id_to_event_id_;
  std::unordered_map<int, bool> id_to_checked_;
  wxEvtHandler *event_dest_ = this;

 public:
  MenuItemToggler() { BindEventMenu(this); };

  void BindEventMenu(wxEvtHandler *handler);

  void AddMenuItem(wxMenuItem *item, int event_id, int toggled_id = wxID_ANY);
  wxMenuItem *GetMenuItem(int id);
  int GetEventId(int id);

  bool Toggle(int id);
  bool Check(int id, bool check);
  bool Enable(int id, bool enable);

  wxEvtHandler *GetEventDestination() { return event_dest_; }
  void SetEventDestination(wxEvtHandler *dest) { event_dest_ = dest; }

  bool IsChecked(int id);

 private:
  void OnMenuEvent(wxCommandEvent &event);
};

}  // namespace gui

}  // namespace fmr
