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
#include <fmr/common/string.h>
#include <fmr/handler/handler_factory.h>
#include <fmr/thread/load_image.h>
#include <wx/log.h>
#include <wx/stopwatch.h>

namespace fmr {

namespace thread {

wxDEFINE_EVENT(kEventImageLoaded, LoadImageEvent);

void LoadImage::SetSize(const wxSize &size) { image_size_ = size; }

void LoadImage::SetImageQuality(wxImageResizeQuality quality) {
  image_quality_ = quality;
}

void LoadImage::Push(SStream *stream) { load_queue_.push(stream); }

void LoadImage::Clear() { load_queue_ = std::queue<SStream *>(); }

void LoadImage::DeleteOnEmptyQueue(bool condition) {
  is_delete_on_empty_ = condition;
}

void LoadImage::Load(SStream *stream) {
#define TEST_RETURN() \
  if (TestDestroy()) return;

  auto event =
      std::make_unique<LoadImageEvent>(kEventImageLoaded, GetEventId());
  event->SetStream(stream);

  TEST_RETURN();

  auto input_stream = stream->GetStream();
  if (!wxImage::CanRead(*input_stream)) return;

  TEST_RETURN();

  wxLogMessage("Loading image in %s/%s",
               String::FromString<wxString>(stream->GetHandlerPath()),
               String::FromString<wxString>(stream->GetName()));
  image_util::Load(event->GetImage(), *stream);

  TEST_RETURN();
  wxLogMessage("Sending Image Loaded Event to %p", GetParent());
  wxQueueEvent(GetParent(), event.release());
}

wxThread::ExitCode LoadImage::Entry() {
#define TEST_BREAK() \
  if (TestDestroy()) break

  wxLogMessage("Starting load thread");

  while (!TestDestroy() && !(is_delete_on_empty_ && load_queue_.empty())) {
    if (load_queue_.size() > 0 && !TestDestroy()) {
      TEST_BREAK();
      SStream *stream = load_queue_.front();
      std::shared_ptr<wxInputStream> input_stream = stream->GetStream();

      TEST_BREAK();

      if (!stream->IsOk() || !wxImage::CanRead(*input_stream)) {
        TEST_BREAK();
        std::unique_ptr<AbstractHandler> handler(
            HandlerFactory::NewHandler(stream->GetHandlerPath()));
        TEST_BREAK();
        wxLogMessage("Loading Stream in %s/%s",
                     String::FromString<wxString>(stream->GetHandlerPath())),
            String::FromString<wxString>(stream->GetName());
        TEST_BREAK();
        handler->GetStream(*stream);
      }

      TEST_BREAK();
      Load(stream);
      load_queue_.pop();
    }

    TEST_BREAK();
  }

  wxLogMessage("ID:%d Load thread completed", GetEventId());
  Completed();

  return (wxThread::ExitCode)0;
}

}  // namespace thread

};  // namespace fmr
