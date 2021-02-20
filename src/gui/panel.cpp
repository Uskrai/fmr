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

#include "fmr/gui/panel.h"

#include <fmr/common/config.h>
#include <fmr/common/dimension.h>
#include <fmr/common/path.h>
#include <wx/popupwin.h>
#include <wx/sizer.h>
#include <wx/window.h>

#include <iostream>
#include <memory>

#include "fmr/explorer/controller.h"
#include "fmr/handler/handler_factory.h"
#include "fmr/nowide/string.h"
#include "fmr/reader/controller.h"
#include "fmr/window/grid_window.h"

namespace fmr {

Panel::Panel(wxWindow *parent, wxWindowID id, wxPoint position, wxSize size)
    : wxPanel(parent, id, position, size) {
  sizer_ = new wxBoxSizer(wxHORIZONTAL);

  PrepareReader();
  PrepareExplorer();
  SetSizer(sizer_);
  Layout();
  BindEvent();
};

void Panel::BindEvent() {
  explorer_->Bind(explorer::kEventOpenFile, &Panel::OnExplorerOpenFile, this);
  Bind(reader::kEventOpenFile, &Panel::OnReaderOpenFile, this, ReaderWindow);
}

void Panel::PrepareReader() {
  reader_ = std::make_unique<reader::Controller>(
      this, ReaderWindow, wxDefaultPosition, GetClientSize(), 0, "Reader");

  auto window = reader_->GetWindow();

  if (!window->IsTransparentBackgroundSupported())
    window->SetBackgroundColour(*wxBLACK);

  sizer_->Add(window, 1, wxALL | wxEXPAND);
}

void Panel::SetReaderSettings(const reader::Settings &setting) {
  reader_->SetSettings(setting);
}

void Panel::PrepareExplorer() {
  explorer_ = std::make_unique<explorer::Controller>();
  explorer_->CreateWindow(this, ExplorerWindow, wxDefaultPosition,
                          GetClientSize(), wxWANTS_CHARS, "Explorer");

  explorer_->GetWindow()->SetBackgroundColour(*wxBLACK);
  explorer_->GetWindow()->Hide();

  sizer_->Add(explorer_->GetWindow(), 1, wxALL | wxEXPAND);
}

bool Panel::LoadFile(std::string path) {
  bool ret = false;
  if (reader_) {
    bool is_reader_shown = reader_->GetWindow()->IsShown();
    bool is_reader_focus = reader_->GetWindow()->HasFocus();
    bool is_explorer_shown = explorer_->GetWindow()->IsShown();
    bool is_explorer_focus = explorer_->GetWindow()->HasFocus();

    explorer_->GetWindow()->Hide();
    reader_->GetWindow()->Show();
    reader_->GetWindow()->SetFocus();
    Layout();
    ret = reader_->Open(path);

    if (ret)
      return ret;
    else {
      reader_->GetWindow()->Show(is_reader_shown);
      explorer_->GetWindow()->Show(is_explorer_shown);

      if (is_reader_focus) {
        reader_->GetWindow()->SetFocus();
      } else if (is_explorer_focus) {
        explorer_->GetWindow()->SetFocus();
      }
    }
  }
  return ret;
}

bool Panel::OpenExplorer() {
  std::string select_path;

  if (reader_) {
    auto handler = reader_->GetHandler();
    if (handler) {
      select_path = handler->GetName();
    }
  }

  if (select_path == "") {
    select_path =
        String::Narrow(Config::Get()->Read("RecentlyOpened", wxString()));
  }

  if (!explorer_) return false;

  if (explorer_->GetWindow()->HasFocus()) {
    return explorer_->OpenParent();
  } else {
    if (reader_) {
      reader_->GetWindow()->Hide();
    }
    explorer_->GetWindow()->SetFocus();
    explorer_->GetWindow()->Show();
    Layout();
    wxSize size = explorer_->GetWindow()->GetClientSize();
    if (explorer_->OpenParent(select_path)) {
      explorer_->GetWindow()->Show();
      explorer_->GetWindow()->SetFocus();
      if (reader_) reader_->Clear();
      return true;
    } else {
      if (reader_) {
        reader_->GetWindow()->Show();
      }
    }
  }
  return false;
}

void Panel::OnKeyDown(wxKeyEvent &event) {
  // if (event.GetKeyCode() == WXK_BACK)
  // if (OpenExplorer()) return;
  event.Skip();
}

void Panel::OnExplorerOpenFile(wxCommandEvent &event) {
  explorer_->GetWindow()->Hide();

  if (event.GetString().empty()) return;

  if (LoadFile(String::Narrow(event.GetString())))
    explorer_->Clear();
  else {
    explorer_->GetWindow()->Show();
    explorer_->GetWindow()->SetFocus();
    explorer_->Open(String::Narrow(event.GetString()));
  }
}

void Panel::OnReaderOpenFile(wxCommandEvent &event) {
  Config::Get()->Write("RecentlyOpened", event.GetString());
}

bool Panel::Destroy() {
  if (reader_) reader_->GetWindow()->Destroy();
  if (explorer_) explorer_->GetWindow()->Destroy();
  return wxWindow::Destroy();
}

Panel::~Panel() { reader_.reset(); }

};  // namespace fmr
