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

#include <fmr/handler/struct_stream.h>
#include <wx/event.h>

namespace fmr {

namespace bitmap {

namespace loader {

class FindEvent : public wxEvent {
  const SStream *source_stream_ = nullptr;
  SStream *found_stream_ = nullptr;

 public:
  FindEvent(int id, wxEventType type) : wxEvent(id, type) {}

  FindEvent *Clone() const override { return new FindEvent(*this); }

  void SetStream(const SStream *source_stream, SStream *found_stream) {
    source_stream_ = source_stream;
    found_stream_ = found_stream;
  }

  const SStream *GetSourceStream() const { return source_stream_; }
  const SStream *GetFoundStream() const { return found_stream_; }
  SStream *GetFoundStream() { return found_stream_; }
};

}  // namespace loader

}  // namespace bitmap

}  // namespace fmr
