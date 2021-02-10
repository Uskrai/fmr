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
#include <fmr/common/dimension.h>
#include <fmr/common/path.h>
#include <fmr/gui/panel.h>
#include <wx/popupwin.h>
#include <wx/sizer.h>
#include <wx/window.h>

#include <iostream>

#include "fmr/handler/handler_factory.h"

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
  Bind(EVT_OPEN_FILE, &Panel::OnExplorerOpenFile, this);
  Bind(wxEVT_TIMER, &Panel::OnReaderInfoTimer, this, kReaderInfoTimer);
  Bind(reader::kEventOpenFile, &Panel::OnReaderOpenFile, this, ReaderWindow);
}

void Panel::PrepareReader() {
  reader_ = std::make_unique<reader::Controller>(
      this, ReaderWindow, wxDefaultPosition, GetClientSize(), 0, "Reader");

  auto window = reader_->GetWindow();

  if (!window->IsTransparentBackgroundSupported())
    window->SetBackgroundColour(*wxBLACK);

  sizer_->Add(window, 1, wxALL | wxEXPAND);
  SettingReader();
}

void Panel::SettingReader() {
  reader_->SetImagePerPage(Config::Get()->Read("Reader/ImageShowLimit", 1));
  reader_->SetPositionFlags(
      static_cast<bitmap::PositionFlags>(Config::Get()->Read(
          "Reader/ImagePosition",
          int(bitmap::kPositionAlignCenter | bitmap::kPositionVertical))));

  reader_->SetReadFromRight(Config::Get()->Read("Reader/ReadFromRight", false));
  reader_->SetScaleFlags(static_cast<bitmap::RescalerFlags>(
      Config::Get()->Read("Reader/ImageSize", int(bitmap::kRescaleNone))));
}

void Panel::PrepareExplorer() {
  explorer_ = new explorer::Window(this, ExplorerWindow, wxDefaultPosition,
                                   GetClientSize(), wxWANTS_CHARS, "Explorer");

  explorer_->SetBackgroundColour(*wxBLACK);
  explorer_->Hide();

  sizer_->Add(explorer_, 1, wxALL | wxEXPAND);
}

bool Panel::LoadFile(std::string path) {
  bool ret = false;
  if (reader_) {
    bool is_reader_shown = reader_->GetWindow()->IsShown();
    bool is_reader_focus = reader_->GetWindow()->HasFocus();
    bool is_explorer_shown = explorer_->IsShown();
    bool is_explorer_focus = explorer_->HasFocus();

    explorer_->Hide();
    reader_->GetWindow()->Show();
    reader_->GetWindow()->SetFocus();
    Layout();
    ret = reader_->Open(path);

    if (ret)
      return ret;
    else {
      reader_->GetWindow()->Show(is_reader_shown);
      explorer_->Show(is_explorer_shown);

      if (is_reader_focus) {
        reader_->GetWindow()->SetFocus();
      } else if (is_explorer_focus) {
        explorer_->SetFocus();
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
        String::ToString(Config::Get()->Read("RecentlyOpened", wxString()));
  }

  if (!explorer_) return false;

  if (explorer_->HasFocus()) {
    return explorer_->OpenParent();
  } else {
    if (reader_) {
      reader_->GetWindow()->Hide();
    }
    explorer_->SetFocus();
    explorer_->Show();
    Layout();
    if (explorer_->OpenParent(select_path)) {
      explorer_->Show();
      explorer_->SetFocus();
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

void Panel::OnExplorerOpenFile(StreamEvent &event) {
  explorer_->Hide();

  if (!event.GetStream()) return;

  auto stream = event.GetStream();

  auto handler = std::unique_ptr<AbstractOpenableHandler>(
      HandlerFactory::NewOpenableHandler(stream->GetHandlerPath()));

  if (!handler) return;

  std::string path = handler->GetItemPath(*stream);

  if (LoadFile(path))
    explorer_->Clear();
  else {
    explorer_->Show();
    explorer_->SetFocus();
    explorer_->Open(path);
  }
}

void Panel::OnReaderChangePage(wxCommandEvent &event) {
  // TODO: Make this work
  wxString text = wxString::Format("%d/%d", event.GetSelection(), 23);
  reader_info_->SetLabelText(text);
  wxRect rect(reader_->GetWindow()->GetPosition(),
              reader_->GetWindow()->GetClientSize());
  wxPoint pos = dimension::AlignPosition(
      reader_->GetWindow()->GetClientRect(), reader_info_->GetSize(),
      wxALIGN_TOP | wxALIGN_CENTER_HORIZONTAL);

  reader_info_->SetWindowStyleFlag(wxALIGN_CENTER_HORIZONTAL |
                                   wxST_NO_AUTORESIZE);

  reader_info_->SetPosition(pos);
  reader_info_->Show();
  reader_info_timer_.Start(1000, wxTIMER_ONE_SHOT);
}

void Panel::OnReaderOpenFile(wxCommandEvent &event) {
  Config::Get()->Write("RecentlyOpened", event.GetString());
}

void Panel::OnReaderInfoTimer(wxTimerEvent &event) { reader_info_->Hide(); }

bool Panel::Destroy() {
  if (reader_) reader_->GetWindow()->Destroy();
  if (explorer_) explorer_->Destroy();
  return wxWindow::Destroy();
}
};  // namespace fmr
