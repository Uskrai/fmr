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

#ifndef FMR_QUEUE_FIND_HANDLER_ITEM
#define FMR_QUEUE_FIND_HANDLER_ITEM

#include <fmr/file_handler/read_stream.h>
#include <fmr/queue/find_handler_flags.h>

namespace fmr {

namespace queue {

class FindItem {
  std::unique_ptr<file_handler::ReadStream> found_stream_;
  const file_handler::ReadStream *source_stream_;
  FindStatus status_ = kFindNotFound;

 public:
  FindItem(const file_handler::ReadStream *source_stream,
           std::unique_ptr<file_handler::ReadStream> found_stream) {
    SetFoundStream(std::move(found_stream));
    SetSourceStream(source_stream);
  };

  FindItem(const file_handler::ReadStream *source_stream) {
    SetFoundStream(source_stream->Clone());
    SetSourceStream(source_stream);
  }

  FindItem(const FindItem &item) {
    SetFoundStream(item.found_stream_->Clone());
    SetSourceStream(item.source_stream_);
  }

  FindItem(FindItem &&item) = default;

  virtual ~FindItem() = default;

  std::unique_ptr<file_handler::ReadStream> &GetFoundStream() {
    return found_stream_;
  }

  const file_handler::ReadStream *GetSourceStream() { return source_stream_; }

  FindStatus GetStatus() { return status_; }

  void SetFoundStream(std::unique_ptr<file_handler::ReadStream> stream) {
    found_stream_ = std::move(stream);
  }
  void SetSourceStream(const file_handler::ReadStream *stream) {
    source_stream_ = stream;
  }
  void SetStatus(FindStatus status) { status_ = status; }

  bool operator==(const FindItem &other) const {
    return other.source_stream_ == source_stream_;
  }
};

}  // namespace queue

}  // namespace fmr

#endif /* end of include guard: FMR_QUEUE_FIND_HANDLER_ITEM */
