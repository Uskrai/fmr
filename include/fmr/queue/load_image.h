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

#ifndef FMR_EXPLORER_LOAD_EXPLORER
#define FMR_EXPLORER_LOAD_EXPLORER

#include <fmr/handler/struct_stream.h>
#include <fmr/queue/load_image_item.h>
#include <fmr/queue/task.h>
#include <fmr/thread/thread.h>
#include <wx/event.h>

#include <queue>

#include "fmr/bitmap/bmp.h"
#include "fmr/bitmap/rescaler.h"
#include "fmr/queue/base.h"

namespace fmr {

namespace queue {

enum LoadReturn { kLoadBeingStopped, kLoadSuccess, kLoadCannotReadStream };

enum LoadImageStatus { kItemLoaded, kCannotLoadItem };

class LoadImage : public Task<LoadImageItem> {
  bitmap::Rescaler *rescaler_ = nullptr;

 public:
  LoadImage() {}
  LoadImage(receiver_type *receiver) : Task(receiver){};

  bool ProcessItem(value_type &item) override;
  LoadReturn Load(value_type &stream);

  /**
   * @brief: Rescaller to use after load image physically ( if any )
   * @param: rescaler bitmap Rescaler
   */
  void SetRescaler(bitmap::Rescaler *rescaler) { rescaler_ = rescaler; }
};

}  // namespace queue
}  // namespace fmr
#endif
