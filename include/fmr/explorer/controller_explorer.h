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

#ifndef FMR_EXPLORER_CONTROLLER_EXPLORER
#define FMR_EXPLORER_CONTROLLER_EXPLORER

#include <fmr/bitmap/rescaler.h>
#include <fmr/explorer/common.h>
#include <fmr/handler/struct_stream.h>
#include <fmr/thread/find_handler.h>
#include <fmr/thread/load_image.h>
#include <fmr/thread/thread.h>
#include <fmr/window/scrolledwindow.h>

#include <memory>
#include <unordered_map>

namespace fmr {

namespace explorer {

enum ThreadID { kFindThreadID = wxID_HIGHEST + 40, kLoadThreadID };

class Controller : public ThreadController {
 protected:
  std::unordered_map<const SStream *, SBitmap *> map_find_, map_loading_;
  std::vector<std::unique_ptr<SStream>> list_found_stream_;
  bitmap::Rescaler rescaler_;

 public:
  Controller(wxWindow *parent);
  ~Controller();
  void SetParameter(std::vector<StreamBitmap> &list_stream);
  void SetThumbSize(const wxSize &size);

  void Load();

  wxEvtHandler *GetParent() { return parent_; }
  wxThread *GetThread(int id);
  void DoSetNull(int id);

  void Clear();

 private:
  wxWindow *parent_;
  std::vector<StreamBitmap> list_stream_;
  thread::FindHandler *find_thread_ = NULL;
  thread::LoadImage *load_thread_ = NULL;
  wxSize thumb_size_;

  void OnFound(thread::FoundEvent &event);
  void OnImageLoaded(thread::LoadImageEvent &event);
  void OnLoaded(StreamBitmapEvent &event);
  void OnUpdate(wxThreadEvent &event);
  void OnFindCompleted(wxThreadEvent &event);
  void OnLoadCompleted(wxThreadEvent &event);
};

};  // namespace explorer

};  // namespace fmr

#endif
