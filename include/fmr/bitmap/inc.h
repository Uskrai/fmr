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

#include <fmr/queue/factory.h>

namespace fmr {

namespace bitmap {

// class Loader;
// typedef queue::ItemEvent<queue::FindItem> ImageFindEvent;
// typedef queue::ItemEvent<queue::LoadImageItem> ImageLoadEvent;
// typedef queue::ItemReceiverEvent<queue::LoadImageItem>
// ImageLoadReceiverEvent; typedef queue::ItemReceiverEvent<queue::FindItem>
// ImageFindReceiverEvent;
//
// class RescaleLoader;
// typedef queue::ItemEvent<queue::RescaleItem> RescaleEvent;
// typedef queue::ItemReceiverEvent<queue::RescaleItem> RescaleReceiverEvent;
//
// namespace loader {
//
// class Base;
// class Page;
// class Rescale;
//
// class LoadEvent;
// class FindEvent;
//
// }  // namespace loader

class PageLoader;
class Rescaler;

class BitmapCtrl;
class BitmapPageCtrl;
class BitmapVectorEvent;
class BitmapVector;

}  // namespace bitmap

}  // namespace fmr
