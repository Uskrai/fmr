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

#include <fmr/common/config.h>
#include <fmr/gui/frame.h>
#include <fmr/gui/panel.h>
#include <wx/event.h>
#include <wx/filedlg.h>
#include <wx/frame.h>
#include <wx/menu.h>
#include <wx/sizer.h>

#include <sstream>

#include "fmr/bitmap/rescaler.h"
#include "fmr/gui/accelerator_table.h"
#include "fmr/gui/menu_item_toggler.h"
#include "fmr/nowide/string.h"
#include "fmr/reader/settings.h"
#include "fmr/window/menu_bar.h"
#include "wx/accel.h"
#include "wx/textdlg.h"

namespace fmr {

Frame::Frame(const wxString& title, const wxPoint& pos, const wxSize& size,
             long style)
    : wxFrame(NULL, wxID_ANY, title, pos, size, style) {
  SetMenuBar(Frame::MenuBar());
  SetStatusBar(Frame::StatusBar());
  SetBackgroundColour(*wxBLACK);

  this->sizer = new wxBoxSizer(wxHORIZONTAL);
  SetPanel();
  SetSizer(this->sizer);
  BindEvent();
  SettingReader();
}

void Frame::BindEvent() {
  Bind(wxEVT_CLOSE_WINDOW, &Frame::OnClose, this);
  Bind(wxEVT_MENU, &Frame::OpenFile, this, kFrameOpenFile);
  Bind(wxEVT_MENU, &Frame::OnOpenExplorer, this, kFrameOpenExplorer);

  Bind(gui::kEventMenuItemToggled, &Frame::OnReaderMenuChanged, this,
       kFrameReaderMenuToggle);
  Bind(wxEVT_MENU, &Frame::OnReaderChangeImageLimit, this,
       kFrameReaderChangeImageLimit);
}

void Frame::OnClose(wxCloseEvent& event) {
  m_panel->Destroy();
  event.Skip();
}

wxStatusBar* Frame::StatusBar() {
  wxStatusBar* bar = new wxStatusBar(this, wxID_ANY, wxSB_FLAT);
  bar->Raise();
  return bar;
}

window::MenuBar* Frame::MenuBar() {
  auto menuBar = new window::MenuBar();
  gui::AcceleratorTable table;

  item_toggler_ = std::make_unique<gui::MenuItemToggler>();
  item_toggler_->BindEventMenu(this);
  item_toggler_->BindEventMenu(menuBar);
  item_toggler_->SetEventDestination(this);
  CreateMenuFile(*menuBar, table);
  CreateMenuReader(*menuBar, table, *item_toggler_);

  SetAcceleratorTable(table.GetTable());
  return menuBar;
}

void Frame::CreateMenuFile(window::MenuBar& menu_bar,
                           gui::AcceleratorTable& table) {
  int id = kFrameOpenFile;
  menu_bar.CreateMenuAndGroup(id, "&File", "Open File");
  table.AddEntry(wxACCEL_CTRL, 'o', id, menu_bar.GetMenuItem(id));

  id = kFrameOpenExplorer;
  menu_bar.CreateMenuAndGroup(id, "&File", "Open Explorer");
  table.AddEntry(wxACCEL_NORMAL, WXK_BACK, id, menu_bar.GetMenuItem(id));
}

void Frame::CreateMenuReader(window::MenuBar& menu_bar,
                             gui::AcceleratorTable& table,
                             gui::MenuItemToggler& toggler) {
  menu_bar.CreateMenuAndGroup(wxID_SEPARATOR, "Reader", "Scaling", "",
                              wxITEM_SEPARATOR);

  int id = kFrameReaderScaleFitWidth;
  menu_bar.CreateMenuAndGroup(id, "Reader", "Fit Width", "", wxITEM_CHECK);
  table.AddEntry(wxACCEL_CTRL, 'w', id, menu_bar.GetMenuItem(id));
  toggler.AddMenuItem(menu_bar.GetMenuItem(id), id, kFrameReaderMenuToggle);

  id = kFrameReaderScaleFitHeight;
  menu_bar.CreateMenuAndGroup(id, "Reader", "Fit Height", "", wxITEM_CHECK);
  table.AddEntry(wxACCEL_CTRL, 'h', id, menu_bar.GetMenuItem(id));
  toggler.AddMenuItem(menu_bar.GetMenuItem(id), id, kFrameReaderMenuToggle);

  id = kFrameReaderScaleEnlarge;
  menu_bar.CreateMenu(id, "Reader", "Allow Enlarge Image", "", wxITEM_CHECK);
  table.AddEntry(wxACCEL_CTRL, 'e', id, menu_bar.GetMenuItem(id));
  toggler.AddMenuItem(menu_bar.GetMenuItem(id), id, kFrameReaderMenuToggle);

  menu_bar.CreateMenu(wxID_SEPARATOR, "Reader", "", "", wxITEM_SEPARATOR);

  id = kFrameReaderReadFromRight;
  menu_bar.CreateMenu(id, "Reader", "Read From Right", "", wxITEM_CHECK);
  table.AddEntry(wxACCEL_CTRL, 'j', id, menu_bar.GetMenuItem(id));
  toggler.AddMenuItem(menu_bar.GetMenuItem(id), id, kFrameReaderMenuToggle);

  id = kFrameReaderChangeImageLimit;
  menu_bar.CreateMenu(id, "Reader", "Change Image Limit", "");
  table.AddEntry(wxACCEL_CTRL, 't', id, menu_bar.GetMenuItem(id));
}

void Frame::SettingReader() {
  reader::Settings setting;
  setting.image_per_page_ =
      Config::Get()->Read("Reader/ImageLimit", int(setting.image_per_page_));

  setting.position_flags_ = static_cast<position::PosFlags>(Config::Get()->Read(
      "Reader/ImagePosition", int(setting.position_flags_)));

  setting.read_from_right_ =
      Config::Get()->Read("Reader/ReadFromRight", setting.read_from_right_);

  setting.rescale_flags_ = static_cast<bitmap::RescalerFlags>(
      Config::Get()->Read("Reader/ImageSize", int(setting.rescale_flags_)));

  auto check_scale = [&, this](int id, int flags) {
    item_toggler_->Check(id, flags & setting.rescale_flags_);
  };

  check_scale(kFrameReaderScaleFitHeight, bitmap::kRescaleFitHeight);
  check_scale(kFrameReaderScaleFitWidth, bitmap::kRescaleFitWidth);
  check_scale(kFrameReaderScaleEnlarge, bitmap::kRescaleEnlarge);

  m_panel->SetReaderSettings(setting);
}

void Frame::OpenFile(wxCommandEvent& event) {
  wxFileDialog* openDialog = new wxFileDialog(
      this, _("Choose a file to open"), wxEmptyString,
      Config::Get()->Read("RecentlyOpened", wxString()),
      "All Files(*)|*|Image File" + wxImage::GetImageExtWildcard() +
          "|Archive Files (*.zip)|*.zip",
      wxFD_OPEN, wxDefaultPosition);

  if (openDialog->ShowModal() == wxID_OK) {
    m_panel->LoadFile(String::Narrow(openDialog->GetPath()));
  }
  openDialog->Destroy();
}

void Frame::OnOpenExplorer(wxCommandEvent& event) {
  m_panel->OpenExplorer();
  event.Skip();
}

void Frame::OnReaderMenuChanged(wxCommandEvent& event) {
  bitmap::RescalerFlags flags;
  auto check_and_add = [&, this](int id, bitmap::RescalerFlags value) {
    if (item_toggler_->IsChecked(id)) flags |= value;
  };

  check_and_add(kFrameReaderScaleFitHeight, bitmap::kRescaleFitHeight);
  check_and_add(kFrameReaderScaleFitWidth, bitmap::kRescaleFitWidth);
  check_and_add(kFrameReaderScaleEnlarge, bitmap::kRescaleEnlarge);

  Config::Get()->Write("Reader/ImageSize", int(flags));

  bool read_from_right = item_toggler_->IsChecked(kFrameReaderReadFromRight);
  Config::Get()->Write("Reader/ReadFromRight", read_from_right);
  SettingReader();
}

void Frame::OnReaderChangeImageLimit(wxCommandEvent& event) {
  auto text_ctrl = new wxTextEntryDialog(
      this, "Input Image Limit Number (Negative show all image)", "");
  text_ctrl->SetTextValidator(wxTextValidator(wxFILTER_NUMERIC));
  if (text_ctrl->ShowModal() == wxID_OK) {
    Config::Get()->Write("Reader/ImageLimit", text_ctrl->GetValue());
    SettingReader();
  }
}

void Frame::SetPanel() {
  m_panel = new Panel(this, kPanel, wxDefaultPosition, GetSize());
  sizer->Add(m_panel, wxSizerFlags().Expand().Align(wxALL));
}

void Frame::OnExit(wxCommandEvent& event) { Close(true); }

void Frame::OnAbout(wxCommandEvent& event) {
  // wxMessageBox( "This is a wxWidgets' Hello world sample",
  //               "About Hello World", wxOK | wxICON_INFORMATION );
}

void Frame::OnHello(wxCommandEvent& event) {
  // wxLogMessage("Hello world from wxWidgets!");
}

}  // namespace fmr
