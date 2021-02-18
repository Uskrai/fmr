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

#include "fmr/window/menu_bar.h"

namespace fmr {

namespace window {

MenuBar::MenuBar(long style) : wxMenuBar(style) {}

bool MenuBar::CreateMenu(int id, const std::string &group_name,
                         const std::string &item,
                         const std::string &help_string, wxItemKind kind) {
  wxMenuItem *menu_item = new wxMenuItem(nullptr, id, item, help_string, kind);
  if (AppendMenuItem(group_name, menu_item)) return true;
  delete menu_item;
  return false;
}

bool MenuBar::CreateMenuAndGroup(int id, const std::string &group_name,
                                 const std::string &item,
                                 const std::string &help_string,
                                 wxItemKind kind) {
  wxMenu *group = GetMenu(group_name);

  if (!group) {
    wxMenu *menu = new wxMenu();
    Append(menu, group_name);

    map_name_to_menu_.insert(std::make_pair(
        wxMenuItem::GetLabelText(group_name).ToStdString(), menu));
  }

  return CreateMenu(id, group_name, item, help_string, kind);
}

bool MenuBar::AppendMenuItem(const std::string &group_name, wxMenuItem *item) {
  wxMenu *group = GetMenu(group_name);

  if (group) {
    group->Append(item);
    map_id_to_menu_item_.insert(std::make_pair(item->GetId(), item));
    return true;
  }
  return false;
}

wxMenu *MenuBar::GetMenu(const std::string &menu) {
  auto it =
      map_name_to_menu_.find(wxMenuItem::GetLabelText(menu).ToStdString());

  if (it != map_name_to_menu_.end()) {
    return it->second;
  }
  return nullptr;
}

wxMenuItem *MenuBar::GetMenuItem(int id) {
  auto it = map_id_to_menu_item_.find(id);

  if (it != map_id_to_menu_item_.end()) {
    return it->second;
  }
  return nullptr;
}

}  // namespace window

}  // namespace fmr
