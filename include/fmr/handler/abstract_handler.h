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
#ifndef FMR_HANDLER_ABSTRACT_HANDLER
#define FMR_HANDLER_ABSTRACT_HANDLER

#include <fmr/common/bitmask.h>
#include <fmr/handler/struct_stream.h>
#include <wx/stream.h>

#include <memory>
#include <vector>

namespace fmr {

enum HandlerSortFirst { kSortFileFirst, kSortDirFirst, kSortAll };

enum DirGetFlags {
  kDirNone = 0x0,
  kDirFile = 0x1,
  kDirDirectory = 0x2,
  kDirSkipDenied = 0x4,
  kDirFollowSymLink = 0x8,
  kDirDefault = kDirFile | kDirDirectory | kDirSkipDenied | kDirFollowSymLink
};

DEFINE_BITMASK_TYPE(DirGetFlags)

class AbstractHandler {
 public:
  AbstractHandler(){};
  AbstractHandler(const std::string &path){};
  virtual ~AbstractHandler(){};

  // return handler of Parent's Directory
  virtual const std::shared_ptr<AbstractHandler> GetParent() const = 0;
  virtual std::shared_ptr<AbstractHandler> GetParent() = 0;

  // return all directory and files in current dir
  virtual const std::vector<struct SStream> &GetChild() const = 0;
  virtual std::vector<struct SStream> &GetChild() = 0;

  // return opened path
  virtual const std::string &GetName() const = 0;
  virtual std::string GetFromCurrent(int step) const = 0;
  // return next or prev folder / file
  virtual std::string GetNext() const = 0;
  virtual std::string GetPrev() const = 0;

  virtual void Open(const std::string &path) = 0;

  virtual bool GetFirst(SStream &stream, DirGetFlags flags = kDirDefault,
                        bool is_get_stream = false) = 0;
  virtual bool GetNextStream(SStream &stream, bool is_get_stream = false) = 0;

  /**
   * @brief this method will try to get stream that is created by this object
   * this method might reset the state of the object
   * @param stream stream that have been initialized
   * by instance of this object
   * @return true if the stream exist
   * @return false if the stream doesn't exist in handler's current path
   */
  virtual bool GetStream(SStream &stream) = 0;

  // enumerate current opened file or dir
  virtual void Traverse(bool GetStream = false,
                        DirGetFlags flags = kDirDefault) = 0;

  virtual bool IsExist(size_t index) const = 0;
  virtual bool IsDirectory(const SStream &stream) const = 0;

  // search from filename
  virtual size_t Index(const std::string &name) const = 0;
  // return files stream;
  virtual const struct SStream &Item(size_t index) const = 0;
  virtual struct SStream &Item(size_t index) = 0;

  virtual size_t Size() const = 0;
  virtual void Clear() = 0;

  // virtual bool GetFirst( SStream &stream, DirGetFlags flags = kDirDefault,
  // bool is_get_stream = false ) = 0;
};

}  // namespace fmr

#endif
