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

#include "fmr/queue/load_image.h"

#include <wx/log.h>

#include "fmr/bitmap/image_util.h"
#include "fmr/handler/handler_factory.h"
#include "fmr/handler/stream_util.h"
#include "fmr/nowide/string.h"

namespace fmr {

namespace queue {

LoadReturn LoadImage::Load(LoadImage::value_type &item) {
#define TEST_DELETED() \
  if (IsBeingStopped()) return kLoadBeingStopped;

  auto &stream = item.GetLoadedStream();

  TEST_DELETED();

  auto input_stream = stream.GetStream();
  if (!wxImage::CanRead(*input_stream)) return kLoadCannotReadStream;

  TEST_DELETED();

  wxLogMessage("Loading image in %s/%s", stream.GetHandlerPath(),
               stream.GetName());

  image_util::Load(item.GetImage(), stream);

  if (rescaler_) rescaler_->DoRescale(item.GetImage());

  SendItem(std::move(item));
  return kLoadSuccess;
}

bool LoadImage::ProcessTask(LoadImage::value_type &item) {
#define TEST_RETURN() \
  if (IsBeingStopped()) return false;

  auto &stream = item.GetLoadedStream();
  std::shared_ptr<wxInputStream> input_stream = stream.GetStream();

  TEST_RETURN();

  if (!stream.IsOk() || !wxImage::CanRead(*input_stream)) {
    TEST_RETURN();
    wxLogMessage("Loading Stream in %s/%s", stream.GetHandlerPath(),
                 stream.GetName());

    stream_util::LoadStream(stream);
  }

  TEST_RETURN();

  return Load(item) != kLoadBeingStopped;
}

}  // namespace queue

}  // namespace fmr
