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

#include "fmr/bitmap/rescale_loader.h"
#include "fmr/bitmap/rescaler.h"
#include "fmr/common/dimension.h"
#include "fmr/explorer/image_cell_explorer.h"
#include "fmr/handler/handler_factory.h"
#include "fmr/nowide/string.h"
#include "fmr/queue/event.h"
#include "fmr/queue/find_handler.h"
#include "fmr/queue/load_image.h"
#include "fmr/window/grid_window.h"
#include "wx/event.h"

namespace fmr {

namespace explorer {

wxDEFINE_EVENT(kEventOpenCell, wxNotifyEvent);

Controller::Controller() {
  window_ = new window::GridWindow();
  rescaler_ = std::make_unique<bitmap::Rescaler>();
  rescaler_->SetFlags(bitmap::kRescaleFitAll);
  loader_ = std::make_unique<bitmap::RescaleLoader>(this, kLoaderId);
  loader_->SetRescaler(rescaler_.get());
  loader_->SetFindFlags(queue::kFindHandlerOnlyFirstItem |
                        queue::kFindHandlerRecursive);

  window_->Bind(wxEVT_KEY_DOWN, &Controller::OnKeyDown, this);
  Bind(bitmap::kEventImageLoaded, &Controller::OnImageLoaded, this, kLoaderId);
  Bind(kEventOpenCell, &Controller::OnOpenCell, this);
}

bool Controller::CreateWindow(wxWindow *parent, int id, const wxPoint &pos,
                              const wxSize &size, long style,
                              const std::string &name) {
  return window_->Create(parent, id, pos, size, style, name);
}

bool Controller::Open(std::unique_ptr<AbstractOpenableHandler> handler) {
  if (!handler) return false;

  handler->Traverse(false);

  if (handler->Size() == 0) return false;
  if (handler_ && handler->GetName() == handler_->GetName()) {
    if (handler_->Size() == handler->Size()) return true;
  }

  Clear();

  for (auto &it : handler->GetChild()) {
    PushCell(it);
  }

  handler_ = std::move(handler);
  AdjustCell();
  window_->GoToCell(0);
  return true;
}

void Controller::PushCell(const SStream &item) {
  auto stream = std::make_unique<SStream>(item);

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

  auto handler = std::shared_ptr<AbstractOpenableHandler>(
      HandlerFactory::NewOpenableHandler(path));

  return Open(HandlerFactory::NewOpenableHandler(path));
}

bool Controller::OpenCell(size_t index) {
  auto opening_cell = window_->GetCellWindow(index);
  if (!opening_cell) return false;

  const SStream *stream = nullptr;
  for (const auto &it : stream_to_cell_) {
    if (it.second == opening_cell) {
      stream = it.first;
    }
  }
  if (!stream) return false;

  wxString path = String::Widen<wxString>(handler_->GetItemPath(*stream));

  auto event = wxNotifyEvent(kEventOpenCell, GetWindow()->GetId());
  event.SetString(path);
  wxPostEvent(this, event);
  return true;
}

bool Controller::OpenParent(const std::string &path) {
  auto child_handler =
      std::unique_ptr<AbstractHandler>(HandlerFactory::NewHandler(path));

  if (!child_handler) return false;

  std::string parent_path = child_handler->GetParent()->GetName();

  if (!Open(parent_path)) return false;

  size_t idx = handler_->Index(path);
  if (handler_->IsExist(idx)) Select(handler_->Item(idx).GetName());

  return true;
}

bool Controller::OpenParent() {
  if (!handler_) return false;

  auto path = handler_->GetName();
  return OpenParent(path);
}

void Controller::Select(std::string name) {
  if (!handler_) return;

  name = Path::GetName(name);

  size_t idx = 0;
  for (const auto &it : stream_vec_) {
    if (it) {
      if (it->GetName() == name) {
        window_->GoToCell(idx);
        break;
      }
    }
    idx++;
  }
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

void Controller::OnOpenCell(wxNotifyEvent &event) {
  if (event.IsAllowed()) {
    std::string path = String::Narrow(event.GetString());

    Open(path);
  }
}

void Controller::OnKeyDown(wxKeyEvent &event) {
  int key_code = event.GetKeyCode();

  if (key_code == WXK_RETURN || key_code == WXK_NUMPAD_ENTER)
    if (OpenCell(window_->GetSelectedIndex())) return;

  event.Skip();
}

void Controller::OnImageLoaded(bitmap::ImageLoadEvent &event) {
  auto item = stream_to_cell_.find(
      loader_->GetSourceStream(event.GetItem().GetStream()));
  if (item != stream_to_cell_.end()) {
    SBitmap &bitmap = item->second->GetBitmap();
    bitmap.SetImage(event.GetItem().GetImage());
    rescaler_->DoRescale(bitmap);
    GetWindow()->Refresh();
  }
}
void Controller::Clear() {
  window_->ClearCell();
  loader_->Clear();
  stream_to_cell_.clear();
  stream_vec_.clear();
  handler_.reset();
}

Controller::~Controller() {}

}  // namespace explorer

}  // namespace fmr
