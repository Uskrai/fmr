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
#include <fmr/queue/load_image.h>
#include <wx/log.h>

namespace fmr {

namespace queue {

wxDEFINE_EVENT(kEventImageLoaded, LoadImageEvent);

LoadReturn LoadImage::Load(SStream *stream) {
#define TEST_DELETED() \
  if (IsBeingDeleted()) return kLoadBeingDeleted;

  auto event =
      std::make_unique<LoadImageEvent>(kEventImageLoaded, GetEventId());
  event->SetStream(stream);

  TEST_DELETED();

  auto input_stream = stream->GetStream();
  if (!wxImage::CanRead(*input_stream)) return kLoadCannotReadStream;

  TEST_DELETED();

  wxLogMessage("Loading image in %s/%s",
               String::FromString<wxString>(stream->GetHandlerPath()),
               String::FromString<wxString>(stream->GetName()));
  wxImage image;
  image_util::Load(image, *stream);

  if (rescaler_) rescaler_->DoRescale(image);

  event->GetBitmap().SetImage(image);

  wxLogMessage("Sending Image Loaded Event to %p", GetParent());
  SendEventToParent(event.release());
  return kLoadSuccess;
}

bool LoadImage::ProcessTask(LoadImage::value_type &stream) {
#define TEST_RETURN() \
  if (IsBeingDeleted()) return false;

  std::shared_ptr<wxInputStream> input_stream = stream->GetStream();

  TEST_RETURN();

  if (!stream->IsOk() || !wxImage::CanRead(*input_stream)) {
    TEST_RETURN();
    std::unique_ptr<AbstractHandler> handler(
        HandlerFactory::NewHandler(stream->GetHandlerPath()));
    TEST_RETURN();
    wxLogMessage("Loading Stream in %s/%s",
                 String::FromString<wxString>(stream->GetHandlerPath())),
        String::FromString<wxString>(stream->GetName());
    TEST_RETURN();
    handler->GetStream(*stream);
  }

  TEST_RETURN();
  if (Load(stream) == kLoadBeingDeleted) {
    return false;
  }

  return true;
}

}  // namespace queue
};  // namespace fmr
