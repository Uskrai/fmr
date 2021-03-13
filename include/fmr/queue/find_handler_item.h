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
#include <fmr/queue/find_handler_flags.h>

namespace fmr {

namespace queue {

class FindItem {
  SStream found_stream_;
  const SStream *source_stream_;
  FindStatus status_ = kFindNotFound;

 public:
  FindItem(const SStream *source_stream, SStream found_stream) {
    SetFoundStream(std::move(found_stream));
    SetSourceStream(source_stream);
  };
  FindItem(const SStream *source_stream) {
    SetFoundStream(*source_stream);
    SetSourceStream(source_stream);
  }
  FindItem(const FindItem &item) = default;
  FindItem(FindItem &&item) = default;

  virtual ~FindItem() = default;

  SStream &GetFoundStream() { return found_stream_; }
  const SStream *GetSourceStream() { return source_stream_; }
  FindStatus GetStatus() { return status_; }

  void SetFoundStream(SStream stream) { found_stream_ = std::move(stream); }
  void SetSourceStream(const SStream *stream) { source_stream_ = stream; }
  void SetStatus(FindStatus status) { status_ = status; }

  bool operator==(const FindItem &other) const {
    return other.source_stream_ == source_stream_;
  }
};

}  // namespace queue

}  // namespace fmr
