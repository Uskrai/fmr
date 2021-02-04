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

void LoadImage::Load(SStream *stream) {
#define TEST_RETURN() \
  if (IsBeingDeleted()) return;

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
  wxImage image;
  image_util::Load(image, *stream);

  event->GetBitmap().SetImage(image);

  TEST_RETURN();

  wxLogMessage("Sending Image Loaded Event to %p", GetParent());
  SendEventToParent(event.release());
}

void LoadImage::PopTask() {
  wxLogMessage("Starting load thread");

  if (IsEmpty()) return;

  SStream *&stream = Front();
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
  Load(stream);

  Pop();
}

}  // namespace queue
};  // namespace fmr
