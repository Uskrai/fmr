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

#ifndef FMR_THREAD_RESCALE
#define FMR_THREAD_RESCALE

#include <fmr/queue/task.h>

#include <queue>

#include "fmr/bitmap/rescaler.h"

namespace fmr {

namespace queue {

enum RescaleStatus { kRescaled, kCannotRescale };

class RescaleItem {
  wxImage *image_ = nullptr;
  RescaleStatus status_;

 public:
  RescaleItem(wxImage *image) { image_ = image; }
  RescaleItem(RescaleStatus status, wxImage *image) {
    status_ = status;
    image_ = image;
  }
  RescaleItem(const RescaleItem &other) = default;
  RescaleItem(RescaleItem &&other) = default;

  wxImage *GetImage() { return image_; }
  void SetImage(wxImage *image) { image_ = image; }

  RescaleStatus GetStatus() { return status_; }
  void SetStatus(RescaleStatus status) { status_ = status; }

  bool operator==(const RescaleItem &oth) const { return oth.image_ == image_; }
};

class Rescale : public Task<RescaleItem> {
 private:
  bitmap::Rescaler *rescaler_ = nullptr;

 public:
  Rescale() {}
  Rescale(receiver_type *receiver) : Task(receiver){};

  void SetRescaler(bitmap::Rescaler *rescaler) { rescaler_ = rescaler; }

  bool ProcessItem(value_type &item) override;
};

}  // namespace queue

}  // namespace fmr

#endif /* end of include guard: FMR_THREAD_RESCALE */
