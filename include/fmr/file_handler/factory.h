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

#ifndef FMR_FILE_HANDLER_FACTORY
#define FMR_FILE_HANDLER_FACTORY

#include <fmr/file_handler/handler.h>
#include <fmr/file_handler/local/handler.h>

namespace fmr {

namespace file_handler {

class Factory {
  inline static Factory *g_factory_ = nullptr;
  std::vector<std::unique_ptr<Handler>> handler_vec_;
  std::vector<std::unique_ptr<local::Handler>> local_handler_vec_;

 public:
  using UniquePtrHandler = std::unique_ptr<Handler>;
  virtual UniquePtrHandler NewHandler(const std::string &path) const {
    return UniquePtrHandler(DoNewHandler(path));
  }

  virtual UniquePtrHandler NewHandler(const ReadStream &stream) const {
    return UniquePtrHandler(DoNewHandler(stream));
  }

  using UniquePtrLocalHandler = std::unique_ptr<local::Handler>;
  virtual UniquePtrLocalHandler NewLocalHandler(const std::string &path) const {
    return UniquePtrLocalHandler(DoNewLocalHandler(path));
  }

  virtual UniquePtrLocalHandler NewLocalHandler(
      const ReadStream &stream) const {
    return UniquePtrLocalHandler(DoNewLocalHandler(stream));
  }

  void RegisterHandler(UniquePtrHandler handler) {
    handler_vec_.push_back(std::move(handler));
  }

  void RegisterHandler(UniquePtrLocalHandler handler) {
    local_handler_vec_.push_back(std::move(handler));
  }

  static void SetGlobal(Factory *factory) { g_factory_ = factory; };
  static const Factory *GetGlobal() { return g_factory_; }

 protected:
  template <typename Type>
  UniquePtrHandler DoNewHandler(const Type &t) const {
    UniquePtrHandler handler;
    for (const auto &it : handler_vec_)
      if (it->IsHandleable(t)) {
        handler = it->CreateNew();
        break;
      }

    if (handler) {
      handler->Open(t);
    } else {
      return UniquePtrHandler(DoNewLocalHandler(t));
    }

    return handler;
  }

  template <typename Type>
  UniquePtrLocalHandler DoNewLocalHandler(const Type &t) const {
    UniquePtrLocalHandler handler;
    for (const auto &it : local_handler_vec_)
      if (it->IsHandleable(t)) {
        handler = it->CreateNew();
        break;
      }

    if (handler) handler->Open(t);
    return handler;
  }
};

void InitDefaultFactory(Factory &factory);

}  // namespace file_handler

}  // namespace fmr

#endif /* end of include guard: FMR_FILE_HANDLER_FACTORY */
