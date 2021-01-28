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

#include <fmr/explorer/window_explorer.h>
#include <wx/filename.h>

namespace fmr {

wxDEFINE_EVENT(EVT_OPEN_FILE, StreamEvent);

namespace explorer {

Window::Window(wxWindow *parent, const wxWindowID &id, const wxPoint &pos,
               const wxSize &size, const long &style, const wxString &name)
    : FlexGridWindow(parent, id, pos, size, style, name) {
  // HideColLabels();
  // HideRowLabels();
  // DisableCellEditControl();
  // EnableEditing( false );
  // SetCellHighlightColour( *wxWHITE );
  // SetGridLineColour( *wxBLACK );
  CreateGrid(0, 0);
  // grid_table_ = new wxGridStringTable();
  // SetTable( grid_table_, true );
  BindEvent();
  loader_.SetFindFlags(thread::kFindHandlerOnlyFirstItem |
                       thread::kFindHandlerRecursive);
}

void Window::BindEvent() {
  Bind(EVT_COMMAND_THREAD_UPDATE, &Window::OnThreadUpdate, this);
  Bind(EVT_COMMAND_THREAD_COMPLETED, &Window::OnThreadUpdate, this);
  Bind(wxEVT_KEY_DOWN, &Window::OnKeyDown, this);
  Bind(thread::kEventImageLoaded, &Window::OnImageLoaded, this, kLoaderId);
}

bool Window::Destroy() { return wxWindow::Destroy(); }

bool Window::Open(std::shared_ptr<AbstractOpenableHandler> handler) {
  if (!handler) return false;

  handler->Traverse(false);

  int column = 5;
  int row = ceil(double(handler->Size()) / double(column));

  Clear();

  // 11 * 2 * column is an offset from border
  // and gap multiplied by column
  int both_size = ((GetSize().GetWidth() - (11 * 2 * column)) / column);

  wxSize child_size = wxSize(both_size, both_size);
  wxSize best_bitmap_size = ImageWindow::GetBestBitmapSize(child_size);

  rescaler_ = std::make_unique<bitmap::Rescaler>(bitmap::kRescaleFitAll);
  rescaler_->SetMaximumSize(best_bitmap_size);

  CreateGrid(row, column, wxSize(1, 1));

  std::vector<StreamBitmap> list_item;
  list_item.assign(handler->Size(), StreamBitmap());

  size_t idx = 0;
  for (auto &it : handler->GetChild()) {
    auto stream = std::make_unique<SStream>(it);
    // auto bitmap = std::unique_ptr<SBitmap>(new SBitmap());

    ImageWindow *image_cell_window =
        new ImageWindow(this, wxID_ANY, wxDefaultPosition, child_size);

    image_cell_window->SetBackgroundColour(*wxBLACK);
    image_cell_window->SetForegroundColour(*wxWHITE);
    // image_cell_window->SetBitmap(bitmap.get());
    image_cell_window->SetStream(stream.get());

    Add(image_cell_window);

    loader_.PushFind(stream.get());
    map_window_.insert(std::make_pair(stream.get(), image_cell_window));
    list_stream_.push_back(std::move(stream));
    idx++;
  }

  SetCellBorderWidth(10);
  SetCellHighlightPenWidth(3);

  loader_.Run();
  Refresh();
  handler_ = handler;
  Layout();
  FitInside();
  return true;
}

bool Window::Open(const std::string &name) {
  if (name.empty()) return false;

  auto handler = std::shared_ptr<AbstractOpenableHandler>(
      HandlerFactory::NewOpenableHandler(name));

  return Open(handler);
}

bool Window::OpenCell(int index) {
  auto opening_cell = GetCellWindow(index);
  if (!opening_cell) return false;

  const SStream *stream = nullptr;
  for (const auto &it : map_window_) {
    if (it.second == opening_cell->GetCellWindow()) {
      stream = it.first;
    }
  }
  if (!stream) return false;

  std::string path = handler_->GetItemPath(*stream);

  auto handler = std::shared_ptr<AbstractOpenableHandler>(
      HandlerFactory::NewOpenableHandler(path));

  if (handler) {
    handler->Traverse();

    for (const auto &it : handler->GetChild()) {
      auto child_path = handler->GetItemPath(it);

      auto child_handler = std::unique_ptr<AbstractHandler>(
          HandlerFactory::NewHandler(child_path));

      if (child_handler)
        if (child_handler->GetName() == child_path) return Open(path);
    }
  }

  std::shared_ptr<SStream> opened_stream =
      std::shared_ptr<SStream>(new SStream(*stream));
  StreamEvent *event = new StreamEvent(EVT_OPEN_FILE, GetId());
  event->SetStream(opened_stream);

  event->SetString(path);

  wxQueueEvent(GetParent(), event);

  return true;
}

bool Window::OpenParent(const std::string &path) {
  auto child_handler =
      std::unique_ptr<AbstractHandler>(HandlerFactory::NewHandler(path));

  if (!child_handler) return false;

  std::string parent_path = child_handler->GetParent()->GetName();

  if (!Open(parent_path)) return false;

  std::shared_ptr<AbstractOpenableHandler> handler(
      HandlerFactory::NewOpenableHandler(parent_path));

  if (!handler) return false;

  handler->Traverse();

  size_t idx = handler->Index(path);
  if (handler->IsExist(idx)) Select(handler->Item(idx).GetName());

  return true;
}

bool Window::OpenParent() {
  if (!handler_) return false;

  std::string path = handler_->GetName();
  return OpenParent(path);
}

void Window::Clear() {
  ClearCell(true);
  ResetCellPosition();
  loader_.Clear();
  map_window_.clear();
  list_stream_.clear();
  handler_.reset();
}

void Window::OnThreadUpdate(wxThreadEvent &event) { Refresh(); }

void Window::OnKeyDown(wxKeyEvent &event) {
  int key_code = event.GetKeyCode();

  if (key_code == WXK_RETURN || key_code == WXK_NUMPAD_ENTER)
    if (OpenCell(selected_index_)) return;
  //
  // if (key_code == WXK_BACK)
  // if (OpenParent()) return;

  event.Skip();
}

void Window::OnImageLoaded(thread::LoadImageEvent &event) {
  auto item = map_window_.find(loader_.GetSourceStream(event.GetStream()));
  if (item != map_window_.end()) {
    SBitmap &bitmap = item->second->GetBitmap();
    bitmap = event.GetBitmap();
    rescaler_->DoRescale(bitmap);
    Refresh();
  }
}

void Window::Select(std::string name) {
  if (!handler_) return;

  name = Path::GetName(name);

  size_t idx = 0;
  for (const auto &it : list_stream_) {
    if (it) {
      if (it->GetName() == name) {
        GoToCell(idx);
        break;
      }
    }
    idx++;
  }
}

// void Window::OnGridSelect( wxGridEvent &event )
// {
//     wxGridCellCoords eve( event.GetRow(), event.GetCol() );

//     bool is_change = false;
//     for ( const auto &it : list_cell_pos_ )
//         if ( event.GetRow() == it.GetRow() && event.GetCol() == it.GetCol())
//             is_change = true;

//     if ( is_change )
//         selected_cell_.Set( event.GetRow(), event.GetCol() );
//     else
//         event.Veto();

//     Refresh();
// }

};  // namespace explorer

};  // namespace fmr
