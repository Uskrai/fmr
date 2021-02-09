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

#include "fmr/handler/stream_util.h"

#include "fmr/common/string.h"
#include "fmr/handler/handler_factory.h"

namespace fmr {

namespace stream_util {

std::unique_ptr<AbstractHandler> MakeParentHandler(const SStream &stream) {
  return HandlerFactory::NewHandler(stream.GetHandlerPath());
}

std::unique_ptr<AbstractOpenableHandler> MakeParentOpenableHandler(
    const SStream &stream) {
  return HandlerFactory::NewOpenableHandler(stream.GetHandlerPath());
}

std::string GetPath(const SStream &stream) {
  auto parent = MakeParentOpenableHandler(stream);
  if (parent) return parent->GetItemPath(stream);
  return std::string();
}

std::unique_ptr<AbstractHandler> MakeHandler(const SStream &stream) {
  auto path = GetPath(stream);
  if (path.empty()) return nullptr;
  return HandlerFactory::NewHandler(path);
}

bool LoadStream(SStream &stream) {
  auto handler = MakeParentHandler(stream);
  return handler && handler->GetStream(stream);
};

}  // namespace stream_util

}  // namespace fmr
