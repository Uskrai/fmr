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

#ifndef FMR_QUEUE_LOAD_IMAGE_ITEM
#define FMR_QUEUE_LOAD_IMAGE_ITEM

#include <fmr/file_handler/read_stream.h>
#include <wx/image.h>

namespace fmr {

namespace queue {

class LoadImageItem {
  using ReadStream = file_handler::ReadStream;
  wxImage image_;
  const ReadStream *source_ = nullptr;
  std::unique_ptr<ReadStream> stream_;

 public:
  LoadImageItem() = default;
  LoadImageItem(const ReadStream *source) {
    source_ = source;
    stream_ = source_->Clone();
  }
  LoadImageItem(LoadImageItem &&move) = default;
  LoadImageItem(const LoadImageItem &other) {
    source_ = other.source_;
    stream_ = other.stream_->Clone();
  }

  virtual ~LoadImageItem() = default;

  LoadImageItem &operator=(const LoadImageItem &other) {
    if (&other != this) {
      source_ = other.source_;
      stream_ = other.stream_->Clone();
    }
    return *this;
  }

  LoadImageItem &operator=(LoadImageItem &&other) = default;

  void SetImage(const wxImage &image) { image_ = image; }
  wxImage &GetImage() { return image_; }

  void SetStream(const ReadStream *stream) { source_ = stream; }
  const ReadStream *GetStream() { return source_; }

  std::unique_ptr<ReadStream> &GetLoadedStream() { return stream_; }
  const std::unique_ptr<ReadStream> &GetLoadedStream() const { return stream_; }

  bool operator==(const LoadImageItem &item) const {
    return item.source_ == source_;
  }
};

}  // namespace queue

}  // namespace fmr

#endif /* end of include guard: FMR_QUEUE_LOAD_IMAGE_ITEM */
