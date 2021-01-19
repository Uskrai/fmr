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

namespace fmr {

enum {
  PANEL = wxID_HIGHEST + 2001,
};

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
}

void Frame::BindEvent() {
  Bind(wxEVT_MENU, &Frame::OnAbout, this, wxID_ABOUT);
  Bind(wxEVT_MENU, &Frame::OpenFile, this, kFrameOpenFile);
  Bind(wxEVT_CLOSE_WINDOW, &Frame::OnClose, this);
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

wxMenuBar* Frame::MenuBar() {
  wxMenuBar* menuBar = new wxMenuBar;
  menuBar->Append(Frame::MenuFile(), "&File");
  menuBar->Append(Frame::MenuHelp(), "&Help");
  return menuBar;
}

wxMenu* Frame::MenuFile() {
  wxMenu* menuFile = new wxMenu;
  menuFile->Append(kFrameOpenFile, "&Open File \tCtrl+O");
  menuFile->AppendSeparator();
  menuFile->Append(wxID_EXIT);

  return menuFile;
}

wxMenu* Frame::MenuHelp() {
  wxMenu* menuHelp = new wxMenu;
  menuHelp->Append(wxID_ABOUT);
  return menuHelp;
}

void Frame::OpenFile(wxCommandEvent& event) {
  wxFileDialog* openDialog = new wxFileDialog(
      this, _("Choose a file to open"), wxEmptyString,
      Config::Get()->Read("RecentlyOpened", wxString()), _("All Files(*)|*| \
            Images Files(*.jpg;*.png)|*.jpg;*.png|\
            Archive Files (*.zip)|*.zip"),
      wxFD_OPEN, wxDefaultPosition);

  if (openDialog->ShowModal() == wxID_OK) {
    m_panel->LoadFile(String::ToString(openDialog->GetPath()));
  }
  event.Skip();
  openDialog->Destroy();
}

void Frame::SetPanel() {
  m_panel = new Panel(this, PANEL, wxPoint(-1, -1), GetSize());
  this->sizer->Add(m_panel, 1, wxALL);
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
