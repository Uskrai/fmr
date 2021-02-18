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

#include "fmr/gui/menu_item_toggler.h"

#include "wx/event.h"
#include "wx/menu.h"
#include "wx/menuitem.h"

namespace fmr {

namespace gui {

wxDEFINE_EVENT(kEventMenuItemToggled, wxCommandEvent);

void MenuItemToggler::AddMenuItem(wxMenuItem *item, int event_id,
                                  int toggled_id) {
  id_to_menu_item_.insert(std::make_pair(event_id, item));
  id_to_event_id_.insert(std::make_pair(event_id, toggled_id));
  id_to_checked_.insert(std::make_pair(event_id, item->IsChecked()));
}

void MenuItemToggler::BindEventMenu(wxEvtHandler *handler) {
  handler->Bind(wxEVT_MENU, &MenuItemToggler::OnMenuEvent, this);
}

wxMenuItem *MenuItemToggler::GetMenuItem(int id) {
  auto it = id_to_menu_item_.find(id);
  if (it != id_to_menu_item_.end()) return it->second;
  return nullptr;
}

int MenuItemToggler::GetEventId(int id) {
  auto it = id_to_event_id_.find(id);
  if (it != id_to_event_id_.end()) return it->second;
  return wxID_ANY;
}

bool MenuItemToggler::IsChecked(int id) {
  auto item = id_to_checked_.find(id);

  if (item != id_to_checked_.end()) {
    return item->second;
  }
  return false;
}

bool MenuItemToggler::Toggle(int id) {
  auto item = GetMenuItem(id);
  if (item) {
    return Check(id, !IsChecked(id));
  }
  return false;
}

bool MenuItemToggler::Enable(int id, bool enable) {
  auto item = GetMenuItem(id);
  if (item) {
    item->Enable(enable);
    return true;
  }
  return false;
}

bool MenuItemToggler::Check(int id, bool check) {
  auto item = GetMenuItem(id);
  if (item) {
    bool is_checked = IsChecked(id);
    item->Check(check);

    if (item->IsChecked() != is_checked) {
      auto checked = id_to_checked_.find(id);
      if (checked != id_to_checked_.end()) checked->second = item->IsChecked();

      wxCommandEvent event(kEventMenuItemToggled, GetEventId(id));
      wxPostEvent(GetEventDestination(), event);
    }
    return true;
  }
  return false;
}

void MenuItemToggler::OnMenuEvent(wxCommandEvent &event) {
  if (Toggle(event.GetId())) {
    return;
  }
  event.Skip();
}

}  // namespace gui

}  // namespace fmr
