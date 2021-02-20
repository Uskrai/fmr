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

#ifndef FMR_GUI_PANEL
#define FMR_GUI_PANEL

#include <wx/panel.h>

#include <memory>

class wxBoxSizer;

namespace fmr {

class StreamEvent;

namespace reader {
class Controller;
class Settings;
}  // namespace reader
namespace explorer {
class Controller;
}

enum WindowID {
  ReaderWindow = wxID_HIGHEST + 1,
  kReaderInfoWindow,
  kReaderInfoTimer,
  ExplorerWindow
};

class Panel : public wxPanel {
 protected:
 public:
  Panel(wxWindow *parent, wxWindowID id, wxPoint position, wxSize size);
  ~Panel();

  bool LoadFile(std::string path);
  bool OpenExplorer();
  bool Destroy();

  void SetReaderSettings(const reader::Settings &setting);

 private:
  void BindEvent();

  void PrepareReader();
  void PrepareExplorer();

  void OnKeyDown(wxKeyEvent &event);
  void OnCharHook(wxKeyEvent &event);
  void OnExplorerOpenFile(wxCommandEvent &event);
  void OnReaderOpenFile(wxCommandEvent &event);

  std::unique_ptr<reader::Controller> reader_;
  std::unique_ptr<explorer::Controller> explorer_;
  wxBoxSizer *sizer_;
};

};  // namespace fmr

#endif
