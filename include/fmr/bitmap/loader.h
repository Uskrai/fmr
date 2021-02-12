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
#include <wx/event.h>

#include <memory>
#include <queue>
#include <string>
#include <unordered_map>

#include "fmr/handler/struct_stream.h"
#include "fmr/queue/factory.h"
#include "fmr/thread/controller_factory.h"

namespace fmr {

namespace bitmap {

enum ThreadLoaderID {
  kFindImageHandlerThreadID = wxID_HIGHEST + 3000,
  kLoadImageThreadID,
  kRescaleImageThreadID
};

class ImageChecker;

class Loader : public wxEvtHandler {
 private:
  std::queue<wxImage> queue_in_rescale_;
  std::unordered_map<wxImage *, const SStream *> map_loaded_to_source_;

  wxEvtHandler *parent_ = nullptr;
  int event_id_;

  std::unique_ptr<ImageChecker> checker_;
  std::unique_ptr<thread::FindHandlerController> find_controller_;
  std::unique_ptr<thread::LoadImageController> load_controller_;

 public:
  Loader(wxEvtHandler *parent, int id);
  virtual ~Loader();
  virtual bool Open(const std::string &name);
  virtual void PushFind(const SStream *stream);

  void SendImageToParent(const SStream *stream, const SBitmap &bitmap);

  virtual void SetControllerId(int find_controller_id, int load_controller);

  int GetEventId() { return event_id_; }
  void SetEventId(int id) { event_id_ = id; }

  const SStream *GetSourceStream(const SStream *found_stream);

  wxEvtHandler *GetParent() { return parent_; }
  bool Run();

  virtual void ClearThread();
  virtual void Clear();

  thread::FindHandlerController *GetFindController();

  thread::LoadImageController *GetLoadImageController();

 private:
  void OnStreamFound(queue::FoundEvent &event);
  void OnImageLoaded(queue::LoadImageEvent &event);

  void OnThreadCompleted(wxThreadEvent &event);
};

}  // namespace bitmap

}  // namespace fmr

#endif /* end of include guard: FMR_BITMAP_LOADER */
