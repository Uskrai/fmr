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

#ifndef FMR_BITMAP_LOADER_RESCALE_CONTAINER
#define FMR_BITMAP_LOADER_RESCALE_CONTAINER

#include <fmr/bitmap/bmp.h>
#include <fmr/bitmap/loader/container.h>

namespace fmr {

namespace bitmap {

namespace loader {

class RescaleContainer : public Container {
  std::unordered_map<const SStream *, wxImage> stream_to_img_;
  std::unordered_map<const wxImage *, const SStream *> img_to_stream_;

 public:
  wxImage *AddImage(const SStream *found_stream, const wxImage &img);

  wxImage *GetImage(const SStream *found_stream);
  const wxImage *GetImage(const SStream *found_stream) const;

  using Container::GetFoundStream;
  const SStream *GetFoundStream(const wxImage *bmp) const;

  virtual void Clear() override;
};

}  // namespace loader

}  // namespace bitmap

}  // namespace fmr

#endif /* end of include guard: FMR_BITMAP_LOADER_RESCALE_CONTAINER */
