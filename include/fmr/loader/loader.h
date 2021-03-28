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
#include <fmr/loader/container.h>
#include <fmr/loader/find_event.h>
#include <fmr/loader/fwd.h>
#include <fmr/loader/load_event.h>
#include <fmr/thread/inc.h>

#include <unordered_map>
#include <unordered_set>

namespace fmr {

namespace loader {

wxDECLARE_EVENT(kEventImageLoaded, LoadEvent);
wxDECLARE_EVENT(kEventOnFindItemPush, FindEvent);

class Container;

using ReadStream = file_handler::ReadStream;

class Loader : public wxEvtHandler {
  std::unordered_map<const wxImage *, const ReadStream *> map_loaded_to_source_;

  using FindQueueData =
      thread::QueueThreadData<queue::FindItem, queue::FindHandler,
                              ImageFindReceiverEvent>;

  using LoadQueueData =
      thread::QueueThreadData<queue::LoadImageItem, queue::LoadImage,
                              ImageLoadReceiverEvent>;

  using FindQueueCtrl = thread::QueueThreadCtrl<queue::FindItem>;
  using LoadQueueCtrl = thread::QueueThreadCtrl<queue::LoadImageItem>;

  const file_handler::Factory *handler_factory_ =
      file_handler::Factory::GetGlobal();

  std::unique_ptr<FindQueueData> find_data_;
  std::unique_ptr<LoadQueueData> load_data_;

  std::unique_ptr<bitmap::ImageChecker> image_checker_;

  std::unique_ptr<Container> container_;

  int event_id_ = -1;

  bool lazy_load_ = false;

 public:
  Loader(int event_id);
  virtual ~Loader();

  void PushFind(const ReadStream *stream);
  void PushFrontFind(const ReadStream *stream);
  bool MakeFrontFind(const ReadStream *stream);

  void PushLoad(ReadStream *found_stream);
  void PushFrontLoad(ReadStream *found_stream);
  bool MakeFrontLoad(ReadStream *found_stream);

  void LoadSourceStream(const ReadStream *source_stream);
  void LoadFoundStream(const ReadStream *found_stream);

  void SetFindFlags(queue::FindHandlerFlags flags);

  void SendImage(const ReadStream *source_stream,
                 const ReadStream *found_stream, const wxImage &image);

  int GetEventId() const { return event_id_; }

  bool IsInFindQueue(const ReadStream *stream);
  bool IsInLoadQueue(const ReadStream *found_stream);

  virtual Container *GetContainer();
  virtual const Container *GetContainer() const;

  const file_handler::Factory *GetHandlerFactory() const {
    return handler_factory_;
  }

  void SetLazyLoad(bool lazy) { lazy_load_ = lazy; }
  bool IsLazyLoad() { return lazy_load_; }

  bool IsFound(const ReadStream *found_stream) const;
  bool IsSourceFound(const ReadStream *source_stream) const;
  const ReadStream *GetSourceStream(const ReadStream *found_stream) const;
  std::vector<ReadStream *> GetFoundStream(
      const ReadStream *source_stream) const;

  void Clear();

 protected:
  virtual void OnFindItemPush(FindEvent &event);
  virtual void OnImageLoaded(ImageLoadEvent &event);

  virtual void SetContainer(std::unique_ptr<Container> container);

  LoadQueueCtrl *GetLoadController();
  const LoadQueueCtrl *GetLoadController() const;
  FindQueueCtrl *GetFindController();
  const FindQueueCtrl *GetFindController() const;

 private:
  void OnFindItem(ImageFindEvent &event);
  void OnImageLoad(ImageLoadEvent &event);
};

}  // namespace loader

}  // namespace fmr

#endif /* end of include guard: FMR_BITMAP_LOADER_BASE */
