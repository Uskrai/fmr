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

#include <fmr/bitmap/image_util.h>
#include <fmr/explorer/load_explorer.h>
#include <fmr/handler/handler_factory.h>

namespace fmr {

namespace explorer {

wxDEFINE_EVENT(EVT_BITMAP_LOADED, StreamBitmapEvent);

void LoadThread::SetSize(const wxSize &size) { image_size_ = size; }

void LoadThread::SetImageQuality(wxImageResizeQuality quality) {
  image_quality_ = quality;
}

void LoadThread::Push(StreamBitmap &stream_bitmap) {
  load_queue_.push(stream_bitmap);
}

void LoadThread::Clear() { load_queue_ = std::queue<StreamBitmap>(); }

void LoadThread::DeleteOnEmptyQueue(bool condition) {
  is_delete_on_empty_ = condition;
}

void LoadThread::Load(StreamBitmap &item) {
#define TEST_RETURN() \
  if (TestDestroy()) return;

  wxImage image;
  auto stream = item.stream->GetStream();
  if (!wxImage::CanRead(*stream)) return;

  TEST_RETURN();

  image_util::Load(image, *stream);

  TEST_RETURN();

  if (image.IsOk()) image_util::Rescale(image, image_size_, image_quality_);

  TEST_RETURN();

  if (image.IsOk()) item.bitmap->SetBitmap(image);
}

void LoadThread::Update(StreamBitmap &item) {
  TEST_RETURN();
  std::unique_ptr<StreamBitmapEvent> event(
      new StreamBitmapEvent(EVT_BITMAP_LOADED, m_id));

  TEST_RETURN();
  event->SetStreamBitmap(item);

  TEST_RETURN();
  QueueEventParent(event.release());
  BaseThread::Update();
}

wxThread::ExitCode LoadThread::Entry() {
#define TEST_BREAK() \
  if (TestDestroy()) break

  while (!TestDestroy() && !(is_delete_on_empty_ && load_queue_.empty())) {
    if (load_queue_.size() > 0 && !TestDestroy()) {
      TEST_BREAK();
      StreamBitmap &item = load_queue_.front();
      std::shared_ptr<wxInputStream> input_stream = item.stream->GetStream();

      TEST_BREAK();

      if (!item.stream->IsOk() || !wxImage::CanRead(*input_stream)) {
        TEST_BREAK();
        std::unique_ptr<AbstractHandler> handler(
            HandlerFactory::NewHandler(item.stream->GetHandlerPath()));
        TEST_BREAK();
        handler->GetStream(*item.stream);
      }

      TEST_BREAK();

      Load(item);

      Update(item);
      load_queue_.pop();
    }

    TEST_BREAK();
  }

  Completed();

  return (wxThread::ExitCode)0;
}

}  // namespace explorer

};  // namespace fmr
