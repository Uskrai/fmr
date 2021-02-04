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

#ifndef FMR_BITMAP_RESCALE_LOADER
#define FMR_BITMAP_RESCALE_LOADER

#include "fmr/bitmap/loader.h"
#include "fmr/thread/rescale_controller.h"

namespace fmr {

namespace bitmap {

class RescaleLoader : public Loader {
  thread::RescaleController rescale_controller_;
  std::unordered_map<const SStream *, SBitmap> stream_bmp_;
  std::unordered_map<const wxImage *, const SStream *> img_stream_;

 public:
  RescaleLoader(wxEvtHandler *parent, int id);

  using Loader::SetControllerId;
  void SetControllerId(int find_id, int load_id, int rescale_id);

  thread::RescaleController *GetRescaleController() {
    return &rescale_controller_;
  }

 private:
  void OnImageLoaded(queue::LoadImageEvent &event);
  void OnImageRescaled(queue::RescaledEvent &event);
};

}  // namespace bitmap

}  // namespace fmr

#endif /* end of include guard: FMR_BITMAP_RESCALE_LOADER */
