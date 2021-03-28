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

#ifndef H_FMR_FILE_HANDLER_VIRTUAL_WRITE_MAP_H
#define H_FMR_FILE_HANDLER_VIRTUAL_WRITE_MAP_H

#include <fmr/file_handler/virtual_write/stream.h>

#include <map>
#include <queue>

namespace fmr {

namespace file_handler {

namespace virtual_write {

class Map {
  std::multimap<std::string, Map> path_;
  bool is_dir_ = false;
  char separator_ = '\0';
  virtual_write::Stream stream_;

  using StringVector = std::queue<std::string>;

 public:
  Map(char separator) : is_dir_(true), separator_(separator) {}

  /**
   * @brief: Check if given name exist within map or child's
   * @param: name name to check
   * @return: Check if given name exist within this map or child's
   */
  bool Exist(const std::string &name) const;

  template <typename... U>
  bool AddEmplace(U &&...u) {
    return Add(virtual_write::Stream(std::forward<U>(u)...));
  }

  /**
   * @brief: Add Stream to map
   *
   * the given stream will only added if the parent path exist or if WriteType
   * is overwrite and IsDirectory is equal from existing stream
   *
   * @return: true if the stream is added
   */
  bool Add(const Stream &stream);

  bool IsDirectory() const { return is_dir_; }

  /**
   * @brief: Check if current map is empty
   */
  bool Empty() const { return path_.empty(); }

  /**
   * @brief: Search for given stream's Map
   * @param: stream stream to search for
   * @return: Map from given name, nullptr if map is not found
   */
  const Map *Search(const Stream &stream) const;
  /**
   * @brief: Search for given name's map
   * @param:
   * @return: Map from given name, nullptr if nap is not found
   */
  const Map *Search(const std::string &name) const;
  char GetSeparator() const { return separator_; }

  const virtual_write::Stream &GetVirtualStream() const { return stream_; }

  enum ExtractType {
    kExtractReadStream = 0x01,
    kExtractWriteStream = 0x02,
    kExtractAll = kExtractReadStream | kExtractWriteStream
  };

  /**
   * @brief: Extract virtual Stream from map to vector
   * @param: extract_type Extract mode
   * @return: Vector to virtual_write::Stream
   */
  std::vector<virtual_write::Stream> ToVector(ExtractType extract_type) const;

 protected:
  Map(char separator, const virtual_write::Stream &stream)
      : is_dir_(stream.IsDirectory()), separator_(separator), stream_(stream){};

  StringVector Separate(const std::string &name) const;

  bool SetVirtualStream(const virtual_write::Stream &stream);

  bool Add(StringVector &name, virtual_write::Stream stream);

  const Map *Search(StringVector &vec) const;

  const Map *Find(const std::string &name) const;
  Map *Find(const std::string &name);

  void ToVector(std::vector<virtual_write::Stream> &vec,
                ExtractType extract_type) const;
};

}  // namespace virtual_write

}  // namespace file_handler

}  // namespace fmr

#endif /* end of include guard: H_FMR_FILE_HANDLER_VIRTUAL_WRITE_MAP_H */
