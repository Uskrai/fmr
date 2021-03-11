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

#ifndef FMR_BITMAP_LOADER_BASE
#define FMR_BITMAP_LOADER_BASE

#include <fmr/bitmap/image_checker.h>
#include <fmr/bitmap/inc.h>
#include <fmr/bitmap/loader/container.h>
#include <fmr/bitmap/loader/find_event.h>
#include <fmr/bitmap/loader/load_event.h>
#include <fmr/queue/find_handler.h>
#include <fmr/thread/controller_factory.h>

#include <unordered_map>
#include <unordered_set>

namespace fmr {

namespace bitmap {

namespace loader {

wxDECLARE_EVENT(kEventImageLoaded, LoadEvent);
wxDECLARE_EVENT(kEventOnFindItemPush, FindEvent);

class Container;

class Base : public wxEvtHandler {
  std::unordered_map<const wxImage *, const SStream *> map_loaded_to_source_;

  std::unique_ptr<ImageFindReceiverEvent> find_receiver_;
  std::unique_ptr<thread::FindHandlerController> find_controller_;

  std::unique_ptr<ImageLoadReceiverEvent> load_receiver_;
  std::unique_ptr<thread::LoadImageController> load_controller_;

  std::unique_ptr<ImageChecker> image_checker_;

  std::unique_ptr<Container> container_;

  int event_id_ = -1;

  bool lazy_load_ = false;

 public:
  Base(int event_id);
  virtual ~Base();

  void PushFind(const SStream *stream);
  void PushFrontFind(const SStream *stream);
  bool MakeFrontFind(const SStream *stream);

  void PushLoad(SStream *found_stream);
  void PushFrontLoad(SStream *found_stream);
  bool MakeFrontLoad(SStream *found_stream);

  void LoadSourceStream(const SStream *source_stream);
  void LoadFoundStream(const SStream *found_stream);

  void SetFindFlags(queue::FindHandlerFlags flags);

  void SendImage(const SStream *source_stream, const SStream *found_stream,
                 const wxImage &image);

  int GetEventId() const { return event_id_; }

  bool IsInFindQueue(const SStream *stream);
  bool IsInLoadQueue(const SStream *found_stream);

  virtual Container *GetContainer();
  virtual const Container *GetContainer() const;

  void SetLazyLoad(bool lazy) { lazy_load_ = lazy; }
  bool IsLazyLoad() { return lazy_load_; }

  bool IsFound(const SStream *found_stream) const;
  bool IsSourceFound(const SStream *source_stream) const;
  const SStream *GetSourceStream(const SStream *found_stream) const;
  std::vector<SStream *> GetFoundStream(const SStream *source_stream) const;

  void Clear();

 protected:
  virtual void OnFindItemPush(FindEvent &event);
  virtual void OnImageLoaded(ImageLoadEvent &event);

  virtual void SetContainer(std::unique_ptr<Container> container);

  thread::LoadImageController *GetLoadController();
  const thread::LoadImageController *GetLoadController() const;
  thread::FindHandlerController *GetFindController();
  const thread::FindHandlerController *GetFindController() const;

 private:
  void OnFindItem(ImageFindEvent &event);
  void OnImageLoad(ImageLoadEvent &event);
};

}  // namespace loader

}  // namespace bitmap

}  // namespace fmr

#endif /* end of include guard: FMR_BITMAP_LOADER_BASE */
