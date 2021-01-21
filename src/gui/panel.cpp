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
#include <fmr/common/path.h>
#include <fmr/gui/panel.h>
#include <fmr/handler/abstract_handler.h>
#include <wx/sizer.h>

namespace fmr {

Panel::Panel(wxWindow *parent, wxWindowID id, wxPoint position, wxSize size)
    : wxPanel(parent, id, position, size) {
  sizer_ = new wxBoxSizer(wxHORIZONTAL);
  reader_ = new reader::Window(this, ReaderWindow, wxDefaultPosition,
                               GetClientSize(), 0, "Reader");
  if (!reader_->IsTransparentBackgroundSupported())
    reader_->SetBackgroundColour(*wxBLACK);
  sizer_->Add(reader_, 1, wxALL | wxEXPAND);

  explorer_ = new explorer::Window(this, ExplorerWindow, wxDefaultPosition,
                                   GetClientSize(), wxWANTS_CHARS, "Explorer");
  explorer_->SetBackgroundColour(*wxBLACK);
  explorer_->Hide();
  sizer_->Add(explorer_, 1, wxALL | wxEXPAND);

  SetSizer(sizer_);
  BindEvent();
};

void Panel::BindEvent() {
  Bind(EVT_OPEN_FILE, &Panel::OnExplorerOpenFile, this);
}

bool Panel::LoadFile(std::string path) {
  bool ret = reader_->Open(path);
  reader_->Show();
  if (ret) reader_->SetFocus();

  if (ret && explorer_) {
    explorer_->Hide();
    explorer_->Clear();
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
    explorer_->SetFocus();
    explorer_->Show();
    if (explorer_->OpenParent(select_path)) {
      explorer_->Show();
      if (reader_) {
        reader_->Clear();
        reader_->Hide();
      }
      explorer_->SetFocus();
      return true;
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
  else
    explorer_->Show();
}

bool Panel::Destroy() {
  if (reader_) reader_->Destroy();
  if (explorer_) explorer_->Destroy();
  return wxWindow::Destroy();
}
};  // namespace fmr
