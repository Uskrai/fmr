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

#ifndef FMR_GUI_MENU_BAR
#define FMR_GUI_MENU_BAR

#include <wx/menu.h>
#include <wx/menuitem.h>

#include <unordered_map>

namespace fmr {

namespace window {

class MenuBar : public wxMenuBar {
  std::unordered_map<std::string, wxMenu *> map_name_to_menu_;
  std::unordered_map<int, wxMenuItem *> map_id_to_menu_item_;

 public:
  MenuBar(long style = 0);

  bool CreateMenu(int id, const std::string &group_name,
                  const std::string &item = "",
                  const std::string &help_string = "",
                  wxItemKind kind = wxITEM_NORMAL);
  bool CreateMenuAndGroup(int id, const std::string &group_name,
                          const std::string &item = "",
                          const std::string &help_string = "",
                          wxItemKind kind = wxITEM_NORMAL);

  bool AppendMenuItem(const std::string &group_name, wxMenuItem *item);

  wxMenu *GetMenu(const std::string &menu);

  wxMenuItem *GetMenuItem(int id);
};

}  // namespace window

}  // namespace fmr

#endif /* end of include guard: FMR_GUI_MENU_BAR */
