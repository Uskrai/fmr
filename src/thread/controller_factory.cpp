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

#include "fmr/thread/controller_factory.h"

#include "fmr/thread/find_handler_controller.h"
#include "fmr/thread/load_image_controller.h"
#include "fmr/thread/rescale_controller.h"

namespace fmr {

namespace thread {

namespace controller_factory {

std::unique_ptr<FindHandlerController> NewFindHandler(wxEvtHandler *parent,
                                                      int id) {
  return std::make_unique<FindHandlerController>(parent, id);
}

std::unique_ptr<LoadImageController> NewLoadImage(wxEvtHandler *parent,
                                                  int id) {
  return std::make_unique<LoadImageController>(parent, id);
}
std::unique_ptr<RescaleController> NewRescale(wxEvtHandler *parent, int id) {
  return std::make_unique<RescaleController>(parent, id);
}

}  // namespace controller_factory

}  // namespace thread

}  // namespace fmr
