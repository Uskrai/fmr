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

#ifndef FMR_HANDLER_STREAM_UTIL
#define FMR_HANDLER_STREAM_UTIL

#include <memory>

namespace fmr {

class SStream;
class AbstractHandler;
class AbstractOpenableHandler;

namespace stream_util {

std::unique_ptr<AbstractHandler> MakeParentHandler(const SStream &stream);
std::unique_ptr<AbstractOpenableHandler> MakeParentOpenableHandler(
    const SStream &stream);

std::string GetPath(const SStream &stream);

bool LoadStream(SStream &stream);

}  // namespace stream_util

}  // namespace fmr

#endif /* end of include guard: FMR_HANDLER_STREAM_UTIL */
