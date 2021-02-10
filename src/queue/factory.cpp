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

#include "fmr/queue/factory.h"

#include "fmr/queue/find_handler.h"
#include "fmr/queue/load_image.h"
#include "fmr/queue/rescale.h"

namespace fmr {

namespace queue {

namespace factory {

std::unique_ptr<queue::LoadImage> LoadImage(wxEvtHandler *parent, int id) {
  return std::make_unique<queue::LoadImage>(parent, id);
}
std::unique_ptr<queue::FindHandler> FindHandler(wxEvtHandler *parent, int id) {
  return std::make_unique<queue::FindHandler>(parent, id);
}

}  // namespace factory

}  // namespace queue

}  // namespace fmr
