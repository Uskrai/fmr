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

#include <fmr/handler/default_handler.h>
#include <fmr/handler/handler_factory.h>
#include <fmr/handler/std_handler.h>
#include <fmr/handler/wxarchive_handler.h>

namespace fmr {

HandlerType HandlerFactory::GetType() { return m_type; }

bool HandlerFactory::Is(const std::string& path1, const std::string& path2) {
  HandlerType type1, type2;
  Find(path1, type1);
  Find(path2, type2);
  return type1 == type2;
}

bool HandlerFactory::Find(const std::string& path) {
  HandlerType type;
  return Find(path, type);
}

bool HandlerFactory::Find(const std::string& path, HandlerType& type) {
  type = kHandlerNotFound;
  if (WxArchiveHandler::CanHandle(path)) {
    type = kHandlerWxArchive;
    return true;
  }

  return FindOpenable(path, type);
}

bool HandlerFactory::FindOpenable(const std::string& path) {
  HandlerType type;
  return FindOpenable(path, type);
}

bool HandlerFactory::FindOpenable(const std::string& path, HandlerType& type) {
  if (STDHandler::CanHandle(path)) {
    type = kHandlerStd;
    return true;
  } else if (DefaultHandler::CanHandle(path)) {
    type = kHandlerDefault;
    return true;
  }
  type = kHandlerNotFound;
  return false;
}

std::unique_ptr<AbstractHandler> HandlerFactory::NewHandler(
    const std::string& path) {
  std::unique_ptr<AbstractHandler> handler = NULL;
  HandlerType type;
  if (Find(path, type)) {
    handler = NewHandler(type);
    if (handler) handler->Open(path);
  }
  return handler;
}

std::unique_ptr<AbstractHandler> HandlerFactory::NewHandler(
    const HandlerType& type) {
  switch (type) {
    case kHandlerWxArchive:
      return std::make_unique<WxArchiveHandler>();
    default:
      return NewOpenableHandler(type);
  }
}

std::unique_ptr<AbstractOpenableHandler> HandlerFactory::NewOpenableHandler(
    const std::string& path) {
  HandlerType type;
  std::unique_ptr<AbstractOpenableHandler> handler;
  if (Find(path, type)) {
    handler = NewOpenableHandler(type);
    if (handler) handler->Open(path);
  }

  return handler;
}

std::unique_ptr<AbstractOpenableHandler> HandlerFactory::NewOpenableHandler(
    const HandlerType& type) {
  switch (type) {
    case kHandlerStd:
      return std::make_unique<STDHandler>();
    case kHandlerDefault:
      return std::make_unique<DefaultHandler>();
    default:
      return nullptr;
  }
  return nullptr;
}

std::unique_ptr<AbstractHandler> HandlerFactory::NewHandler() {
  return NewHandler(m_type);
}

bool HandlerFactory::IsOpenable(const std::string& path) {
  HandlerType type, openable_type;
  Find(path, type);
  FindOpenable(path, openable_type);

  return type == openable_type;
}

};  // namespace fmr
