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

namespace fmr {

namespace bitmap {

enum ThreadLoaderID {
  kFindImageHandlerThreadID = wxID_HIGHEST + 3000,
  kLoadImageThreadID
};

class Loader : public ThreadController {
 private:
  thread::FindHandler *find_thread_ = nullptr;
  thread::FindHandlerFlags find_flags_ = thread::kFindHandlerDefault;
  thread::LoadImage *load_image_thread_ = nullptr;
  Rescaler *rescaler_ = nullptr;

  wxEvtHandler *parent_ = nullptr;

  thread::FindHandlerController find_controller_;
  thread::LoadImageController load_controller_;

 public:
  Loader(wxEvtHandler *parent);
  virtual ~Loader() { Clear(); }
  virtual bool Open(const std::string &name);
  virtual void PushFind(const SStream *stream) {
    find_controller_.Push(stream);
  };

  wxEvtHandler *GetParent() { return parent_; }
  wxThread *GetThread(int id);
  void DoSetNull(int id);
  bool Run();
  void Clear();

  void SetRescaler(Rescaler *rescaler) { rescaler_ = rescaler; }
  void SetFindFlags(const thread::FindHandlerFlags &flags) {
    find_controller_.SetFlags(flags);
  };

 private:
  void OnStreamFound(thread::FoundEvent &event);
  void OnImageLoaded(thread::LoadImageEvent &event);
};

}  // namespace bitmap

}  // namespace fmr

#endif /* end of include guard: FMR_BITMAP_LOADER */
