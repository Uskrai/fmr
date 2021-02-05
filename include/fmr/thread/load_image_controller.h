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

#ifndef FMR_THREAD_LOAD_IMAGE_CONTROLLER
#define FMR_THREAD_LOAD_IMAGE_CONTROLLER

#include "fmr/handler/struct_stream.h"
#include "fmr/queue/load_image.h"
#include "fmr/thread/queue_ctrl.h"

namespace fmr {

namespace thread {

class LoadImageController : public QueueThreadCtrl<queue::LoadImage> {
  using QueueThreadCtrl::QueueThreadCtrl;
};

}  // namespace thread

}  // namespace fmr

#endif /* end of include guard: FMR_THREAD_LOAD_IMAGE_CONTROLLER */
