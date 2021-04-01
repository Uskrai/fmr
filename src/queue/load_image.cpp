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

#include <filesystem>

#include "fmr/bitmap/image_util.h"
#include "fmr/common/path.h"
#include "fmr/file_handler/wx/input_stream.h"
#include "fmr/log/stopwatch.h"
#include "fmr/nowide/filesystem.h"
#include "fmr/nowide/string.h"

namespace fmr {

namespace queue {

LoadReturn LoadImage::Load(LoadImage::value_type &item) {
#define TEST_DELETED() \
  if (IsBeingStopped()) return kLoadBeingStopped;

  auto &stream = item.GetLoadedStream();

  TEST_DELETED();

  auto input_stream = file_handler::wx::InputStream(*stream);
  if (!wxImage::CanRead(input_stream)) return kLoadCannotReadStream;

  TEST_DELETED();

  GetLogger().Trace("Loading image in {}/{}", stream->GetHandlerPath(),
                    stream->GetName());

  item.GetImage() = wxImage(input_stream);

  if (rescaler_) rescaler_->DoRescale(item.GetImage());

  SendItem(std::move(item));
  return kLoadSuccess;
}

bool LoadImage::ProcessItem(LoadImage::value_type &item) {
#define TEST_RETURN() \
  if (IsBeingStopped()) return false;

  auto &stream = item.GetLoadedStream();
  auto input_stream = file_handler::wx::InputStream(*stream);

  log::StopWatchMilli sw;
  auto id = stream->GetHandlerPath() + "/" + stream->GetName();
  GetLogger().Info("Start Loading {} ", id);

  TEST_RETURN();

  if (!stream->IsLoaded() || !wxImage::CanRead(input_stream)) {
    TEST_RETURN();
    GetLogger().Trace("Loading Stream for {}/{}", stream->GetHandlerPath(),
                      stream->GetName());
    stream->Load();
  }

  TEST_RETURN();

  auto ret = Load(item) != kLoadBeingStopped;

  GetLogger().Info("Load completed in {} for {}", sw, id);

  return ret;
}

}  // namespace queue

}  // namespace fmr
