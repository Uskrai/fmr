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

#include "fmr/explorer/controller.h"

#include "fmr/bitmap/rescaler.h"
#include "fmr/common/dimension.h"
#include "fmr/compare/natural.h"
#include "fmr/explorer/image_cell_explorer.h"
#include "fmr/file_handler/factory.h"
#include "fmr/loader/rescale.h"
#include "fmr/nowide/string.h"
#include "fmr/queue/event.h"
#include "fmr/queue/load_image_item.h"
#include "fmr/window/grid_window.h"
#include "wx/event.h"

namespace fmr {

namespace explorer {

wxDEFINE_EVENT(kEventOpenCell, OpenCellEvent);

Controller::Controller() {
  window_ = new window::GridWindow();
  rescaler_ = std::make_unique<bitmap::Rescaler>();
  rescaler_->SetFlags(bitmap::kRescaleFitAll | bitmap::kRescaleShrink);
  loader_ = std::make_unique<loader::Rescale>(kLoaderId);
  loader_->SetRescaler(rescaler_.get());
  loader_->SetFindFlags(queue::kFindHandlerOnlyFirstItem |
                        queue::kFindHandlerRecursive);

  window_->Bind(wxEVT_KEY_DOWN, &Controller::OnKeyDown, this);
  loader_->Bind(loader::kEventImageLoaded, &Controller::OnImageLoaded, this,
                kLoaderId);

  Bind(kEventOpenCell, &Controller::OnOpenCell, this);
}

bool Controller::CreateWindow(wxWindow *parent, int id, const wxPoint &pos,
                              const wxSize &size, long style,
                              const std::string &name) {
  return window_->Create(parent, id, pos, size, style, name);
}

bool Controller::Open(std::unique_ptr<file_handler::Handler> handler) {
  if (!handler) return false;

  handler->Read()->Traverse(false);

  if (handler->Read()->Size() == 0) return false;
  if (handler_ && handler->GetPath() == handler_->GetPath()) {
    if (handler_->Read()->Size() == handler->Read()->Size()) return true;
  }

  Clear();

  handler->Read()->Sort(compare::Natural());
  for (auto &it : *handler->Read()) PushCell(it);

  handler_ = std::move(handler);
  AdjustCell();
  window_->GoToCell(0);
  return true;
}

void Controller::PushCell(const ReadStream &item) {
  auto stream = item.Clone();

  auto cell = std::make_unique<ImageCell>();
  cell->SetString(stream->GetName());
  cell->SetStringFlags(wxALIGN_CENTER_HORIZONTAL, wxALIGN_CENTER_VERTICAL);
  auto temp = cell.get();
  window_->AddCell(std::move(cell));

  loader_->PushFind(stream.get());
  stream_to_cell_.insert(std::make_pair(stream.get(), temp));
  stream_vec_.push_back(std::move(stream));
}

bool Controller::Open(const std::string &path) {
  if (path.empty()) return false;

  auto handler = factory_->NewHandler(path);

  return Open(std::move(handler));
}

bool Controller::OpenCell(size_t index) {
  auto opening_cell = window_->GetCellWindow(index);
  if (!opening_cell) return false;

  const ReadStream *stream = nullptr;
  for (const auto &it : stream_to_cell_) {
    if (it.second == opening_cell) {
      stream = it.first;
    }
  }
  if (!stream) return false;

  auto event = new OpenCellEvent(GetWindow()->GetId(), kEventOpenCell, *stream);
  wxQueueEvent(this, event);
  return true;
}

bool Controller::OpenParent(const std::string &path) {
  auto child_handler = factory_->NewHandler(path);

  if (!child_handler || !child_handler->GetParent()) return false;

  std::string parent_path = child_handler->GetParent()->GetPath();

  if (!Open(parent_path)) return false;

  return Select(path);
}

bool Controller::OpenParent() {
  if (!handler_) return false;

  auto path = handler_->GetPath();
  return OpenParent(path);
}

bool Controller::Select(std::string name) {
  if (!handler_) return false;

  auto idx = handler_->Read()->Index(name);

  bool change = idx < handler_->Read()->Size();
  if (change) window_->GoToCell(idx);
  return change;
}

void Controller::AdjustCell() {
  // 11 * 2 * column is an offset from border
  // and gap multiplied by column
  column_ = 5;
  double max_size = double(window_->GetClientSize().GetWidth() -
                           (border_size_.GetWidth() * 2 * column_));
  double both_size = max_size / double(column_);

  wxSize child_size = wxSize(both_size, both_size);
  wxSize best_bitmap_size = ImageCell::GetBestBitmapSize(child_size);
  rescaler_->SetFitSize(best_bitmap_size);

  GetWindow()->SetTableSize(GetWindow()->GetClientSize());
  GetWindow()->SetCellSize(child_size);
  GetWindow()->SetBorderSize(border_size_);
  GetWindow()->AdjustCell();

  if (GetWindow()->GetClientSize() != GetWindow()->GetTableSize())
    return AdjustCell();
}

void Controller::OnOpenCell(OpenCellEvent &event) {
  Open(factory_->NewHandler(*event.GetStream()));
}

void Controller::OnKeyDown(wxKeyEvent &event) {
  int key_code = event.GetKeyCode();

  if (key_code == WXK_RETURN || key_code == WXK_NUMPAD_ENTER)
    if (OpenCell(window_->GetSelectedIndex())) return;

  event.Skip();
}

void Controller::OnImageLoaded(loader::LoadEvent &event) {
  auto item =
      stream_to_cell_.find(loader_->GetSourceStream(event.GetFoundStream()));

  if (item != stream_to_cell_.end()) {
    SBitmap &bitmap = item->second->GetBitmap();
    bitmap = event.GetBitmap();
    rescaler_->DoRescale(bitmap);
    GetWindow()->Refresh();
  }
}
void Controller::Clear() {
  window_->ClearCell();
  loader_->Clear();
  stream_to_cell_.clear();
  handler_.reset();
}

Controller::~Controller() {}

}  // namespace explorer

}  // namespace fmr
