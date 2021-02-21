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
#include <fmr/handler/abstract_openable_handler.h>
#include <wx/gdicmn.h>

#include <unordered_map>
class wxWindow;

namespace fmr {

namespace window {
class GridWindow;
}

namespace explorer {

wxDECLARE_EVENT(kEventOpenFile, wxCommandEvent);

enum ExplorerId { kLoaderId = wxID_HIGHEST + 130 };
class ImageCell;

class Controller : public wxEvtHandler {
  wxSize border_size_{30, 30};
  int column_ = 5;
  window::GridWindow *window_ = nullptr;
  std::unordered_map<const SStream *, ImageCell *> stream_to_cell_;
  std::vector<std::unique_ptr<SStream>> stream_vec_;
  std::unique_ptr<AbstractOpenableHandler> handler_;
  std::unique_ptr<bitmap::Rescaler> rescaler_;
  std::unique_ptr<bitmap::RescaleLoader> loader_;

 public:
  Controller();
  virtual ~Controller();

  bool CreateWindow(wxWindow *parent, int id, const wxPoint &pos,
                    const wxSize &size, long style = 0,
                    const std::string &name = wxPanelNameStr);

  window::GridWindow *GetWindow() { return window_; }

  bool Open(std::unique_ptr<AbstractOpenableHandler> handler);
  bool Open(const std::string &path);
  bool OpenCell(size_t index);
  bool OpenParent(const std::string &name);
  bool OpenParent();

  void PushCell(const SStream &stream);

  void Select(std::string name);

  void AdjustCell();

  void Clear();

 private:
  void OnKeyDown(wxKeyEvent &event);
  void OnImageLoaded(bitmap::ImageLoadEvent &event);
};

}  // namespace explorer

}  // namespace fmr

#endif /* end of include guard: FMR_EXPLORER_CONTROLLER */
