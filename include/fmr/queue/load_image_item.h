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

#include <fmr/handler/struct_stream.h>
#include <wx/image.h>

namespace fmr {

namespace queue {

class LoadImageItem {
  wxImage image_;
  const SStream *source_ = nullptr;
  SStream stream_;

 public:
  LoadImageItem() = default;
  LoadImageItem(const SStream *source) {
    source_ = source;
    stream_ = *source_;
  }
  LoadImageItem(LoadImageItem &&move) = default;
  LoadImageItem(const LoadImageItem &other) = default;

  virtual ~LoadImageItem() = default;

  LoadImageItem &operator=(const LoadImageItem &other) = default;
  LoadImageItem &operator=(LoadImageItem &&other) = default;

  void SetImage(const wxImage &image) { image_ = image; }
  wxImage &GetImage() { return image_; }

  void SetStream(const SStream *stream) { source_ = stream; }
  const SStream *GetStream() { return source_; }

  SStream &GetLoadedStream() { return stream_; }
  const SStream &GetLoadedStream() const { return stream_; }

  bool operator==(const LoadImageItem &item) const {
    return item.source_ == source_;
  }
};

}  // namespace queue

}  // namespace fmr

#endif /* end of include guard: FMR_QUEUE_LOAD_IMAGE_ITEM */
