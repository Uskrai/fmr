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

#ifndef FMR_BITMAP_LOADER
#define FMR_BITMAP_LOADER

#include <fmr/bitmap/bmp.h>
#include <fmr/thread/find_handler.h>
#include <fmr/thread/load_image.h>
#include <wx/event.h>

#include <memory>
#include <queue>
#include <string>
#include <unordered_map>

#include "fmr/thread/find_handler_controller.h"
#include "fmr/thread/load_image_controller.h"
#include "fmr/thread/rescale_controller.h"

namespace fmr {

namespace bitmap {

enum ThreadLoaderID {
  kFindImageHandlerThreadID = wxID_HIGHEST + 3000,
  kLoadImageThreadID,
  kRescaleImageThreadID
};

class Loader : public ThreadController {
 private:
  thread::FindHandler *find_thread_ = nullptr;
  thread::FindHandlerFlags find_flags_ = thread::kFindHandlerDefault;
  thread::LoadImage *load_image_thread_ = nullptr;
  std::queue<wxImage> queue_in_rescale_;
  std::unordered_map<wxImage *, const SStream *> map_loaded_to_source_;

  Rescaler *rescaler_ = nullptr;

  wxEvtHandler *parent_ = nullptr;

  thread::FindHandlerController find_controller_;
  thread::LoadImageController load_controller_;

 public:
  Loader(wxEvtHandler *parent);
  virtual ~Loader() { Clear(); }
  virtual bool Open(const std::string &name);
  virtual void PushFind(const SStream *stream) {
    GetFindController()->Push(stream);
  };

  virtual void SetControllerId(int find_controller_id, int load_controller);

  const SStream *GetSourceStream(const SStream *found_stream);

  wxEvtHandler *GetParent() { return parent_; }
  BaseThread *GetThread(int id) { return nullptr; };
  void DoSetNull(int id){};
  bool Run();
  void Clear();

  void SetFindFlags(const thread::FindHandlerFlags &flags) {
    GetFindController()->SetFlags(flags);
  };

  thread::FindHandlerController *GetFindController() {
    return &find_controller_;
  }

  thread::LoadImageController *GetLoadImageController() {
    return &load_controller_;
  }

 private:
  void OnStreamFound(thread::FoundEvent &event);
  void OnImageLoaded(thread::LoadImageEvent &event);

  void OnThreadCompleted(wxThreadEvent &event);
};

}  // namespace bitmap

}  // namespace fmr

#endif /* end of include guard: FMR_BITMAP_LOADER */
