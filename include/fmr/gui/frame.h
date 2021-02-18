/*
 *  Copyright (c) 2020-2021 Uskrai
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

#ifndef FMR_GUI_FRAME
#define FMR_GUI_FRAME

#include <wx/frame.h>

#include <memory>

class wxBoxSizer;
class wxMenuBar;
class wxMenu;
class wxStatusBar;

namespace fmr {

namespace window {
class MenuBar;
}

namespace gui {
class MenuItemToggler;
class AcceleratorTable;
}  // namespace gui

class Panel;

enum {
  kPanel = wxID_HIGHEST + 2001,
  kFrameOpenExplorer,
  kFrameOpenFile,
  kFrameReaderScaleFitWidth,
  kFrameReaderScaleFitHeight,
  kFrameReaderScaleEnlarge,
  kFrameReaderReadFromRight,
  kFrameReaderChangeImageLimit,
  kFrameReaderMenuToggle
};

class Frame : public wxFrame {
 public:
  Frame(const wxString& title, const wxPoint& pos, const wxSize& size,
        long style);
  std::unique_ptr<gui::MenuItemToggler> item_toggler_;

  Panel* m_panel = NULL;
  wxBoxSizer* sizer;

 private:
  void BindEvent();
  void SetPanel();

  // event
  void OnHello(wxCommandEvent& event);
  void OnExit(wxCommandEvent& event);
  void OnAbout(wxCommandEvent& event);

  void OnClose(wxCloseEvent& event);
  void OpenFile(wxCommandEvent& event);
  void OnOpenExplorer(wxCommandEvent& event);

  wxStatusBar* StatusBar();

  window::MenuBar* MenuBar();
  void CreateMenuFile(window::MenuBar& menu_bar, gui::AcceleratorTable& table);
  void CreateMenuReader(window::MenuBar& menu_bar, gui::AcceleratorTable& table,
                        gui::MenuItemToggler& toggler);
  void SettingReader();

  void OnReaderMenuChanged(wxCommandEvent& event);
  void OnReaderChangeImageLimit(wxCommandEvent& event);
};

};  // namespace fmr

#endif
