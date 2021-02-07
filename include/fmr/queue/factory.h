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

#ifndef FMR_QUEUE_FACTORY
#define FMR_QUEUE_FACTORY

#include <memory>

class wxEvtHandler;

namespace fmr {

namespace queue {

class FoundEvent;
class LoadImageEvent;
class RescaleEvent;
class FindHandler;
class LoadImage;
class Rescale;

namespace factory {

std::unique_ptr<FindHandler> FindHandler(wxEvtHandler *parent, int id);
std::unique_ptr<LoadImage> LoadImage(wxEvtHandler *parent, int id);
std::unique_ptr<Rescale> Rescale(wxEvtHandler *parent, int id);

}  // namespace factory

}  // namespace queue

}  // namespace fmr

#endif /* end of include guard: FMR_QUEUE_FACTORY */
