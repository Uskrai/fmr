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

#ifndef FMR_FILE_HANDLER_STREAM
#define FMR_FILE_HANDLER_STREAM

#include <fmr/compare/sortable.h>

#include <memory>

namespace fmr {

namespace file_handler {

class Stream : public compare::Sortable {
 public:
  virtual ~Stream() {}
  /**
   * @brief: Getting Stream Filename
   * @return: stream's Filename
   */
  virtual std::string GetName() const = 0;
  /**
   * @brief: Get Handler's path
   * @return: Get Handler's path
   */
  virtual std::string GetHandlerPath() const = 0;
  /**
   * @brief: Get Internal Buffer
   * @return: pointer to internal Buffer
   */
  virtual const void *GetBuffer() const = 0;

  virtual const void *GetBufferEnd() const = 0;

  virtual size_t Size() const = 0;

  /**
   * @brief: Check if the buffer is currently shared with other Stream
   * @return: true if shared
   */
  virtual bool IsShared() const = 0;

  /**
   * @brief: Check if the stream is a directory
   * @return: true if stream is directory
   */
  virtual bool IsDirectory() const = 0;

  std::unique_ptr<Stream> Clone() const {
    return std::unique_ptr<Stream>(DoClone());
  }

 protected:
  /**
   * @brief: Clone current Stream object
   */
  virtual Stream *DoClone() const = 0;
};

}  // namespace file_handler

}  // namespace fmr

#endif /* end of include guard: FMR_FILE_HANDLER_STREAM */
