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

#include <fmr/bitmap/bmp.h>
#include <fmr/handler/struct_stream.h>
#include <wx/event.h>

namespace fmr {

namespace loader {

class LoadEvent : public wxEvent {
  const SStream *source_stream_ = nullptr, *found_stream_ = nullptr;
  SBitmap bitmap_;

 public:
  LoadEvent(int id, wxEventType type) : wxEvent(id, type) {}
  LoadEvent(const LoadEvent &event) = default;

  LoadEvent *Clone() const override { return new LoadEvent(*this); }

  void SetStream(const SStream *source_stream, const SStream *found_stream) {
    source_stream_ = source_stream;
    found_stream_ = found_stream;
  }

  void SetBitmap(const SBitmap &bitmap) { bitmap_ = bitmap; }

  const SStream *GetSourceStream() const { return source_stream_; }
  const SStream *GetFoundStream() const { return found_stream_; }

  SBitmap &GetBitmap() { return bitmap_; }
  const SBitmap &GetBitmap() const { return bitmap_; }
};

}  // namespace loader

}  // namespace fmr
