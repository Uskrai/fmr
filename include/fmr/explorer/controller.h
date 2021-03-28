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

#ifndef FMR_EXPLORER_CONTROLLER
#define FMR_EXPLORER_CONTROLLER

#include <fmr/bitmap/inc.h>
#include <fmr/file_handler/factory.h>
#include <fmr/loader/fwd.h>
#include <wx/event.h>
#include <wx/gdicmn.h>

#include <unordered_map>

class wxWindow;

namespace fmr {

namespace window {
class GridWindow;
}

namespace explorer {

using ReadStream = file_handler::ReadStream;
class OpenCellEvent : public wxEvent {
  std::unique_ptr<ReadStream> stream_;

 public:
  OpenCellEvent(int id, wxEventType event_type, const ReadStream &stream)
      : wxEvent(id, event_type), stream_(stream.Clone()) {}
  OpenCellEvent(const OpenCellEvent &oth)
      : wxEvent(oth), stream_(oth.stream_->Clone()) {}

  std::unique_ptr<ReadStream> &GetStream() { return stream_; }

  OpenCellEvent *Clone() const override { return new OpenCellEvent(*this); }
};

wxDECLARE_EVENT(kEventOpenCell, OpenCellEvent);

enum ExplorerId { kLoaderId = wxID_HIGHEST + 130 };
class ImageCell;

class Controller : public wxEvtHandler {
  wxSize border_size_{30, 30};
  int column_ = 5;
  window::GridWindow *window_ = nullptr;
  std::unordered_map<const ReadStream *, ImageCell *> stream_to_cell_;
  std::vector<std::unique_ptr<ReadStream>> stream_vec_;
  std::unique_ptr<file_handler::Handler> handler_;
  std::unique_ptr<bitmap::Rescaler> rescaler_;
  std::unique_ptr<loader::Rescale> loader_;

  const file_handler::Factory *factory_ = file_handler::Factory::GetGlobal();

 public:
  Controller();
  virtual ~Controller();

  bool CreateWindow(wxWindow *parent, int id, const wxPoint &pos,
                    const wxSize &size, long style = 0,
                    const std::string &name = wxPanelNameStr);

  window::GridWindow *GetWindow() { return window_; }

  bool Open(std::unique_ptr<file_handler::Handler> handler);
  bool Open(const std::string &path);
  bool OpenCell(size_t index);
  bool OpenParent(const std::string &name);
  bool OpenParent();

  bool Select(std::string name);

  void AdjustCell();

  void Clear();

 private:
  void OnKeyDown(wxKeyEvent &event);
  void OnOpenCell(OpenCellEvent &event);
  void OnImageLoaded(loader::LoadEvent &event);

  void PushCell(const ReadStream &stream);
};

}  // namespace explorer

}  // namespace fmr

#endif /* end of include guard: FMR_EXPLORER_CONTROLLER */
