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

#ifndef FMR_BITMAP_LOADER_CONTAINER
#define FMR_BITMAP_LOADER_CONTAINER

#include <fmr/file_handler/read_stream.h>

#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace fmr {

namespace loader {

template <typename KeyT, typename ValueT>
std::enable_if_t<std::is_pointer_v<ValueT>, ValueT> Find(
    std::unordered_map<KeyT, ValueT> &map, const KeyT &t) {
  auto item = map.find(t);
  if (item != map.end()) return item->second;
  return nullptr;
}

template <typename KeyT, typename ValueT>
std::enable_if_t<!std::is_pointer_v<ValueT>, ValueT> Find(
    std::unordered_map<KeyT, ValueT> &map, const KeyT &t) {
  auto item = map.find(t);
  if (item != map.end()) return item->second;
  return ValueT();
}

template <typename KeyT, typename ValueT>
std::enable_if_t<!std::is_pointer_v<ValueT>, const ValueT> Find(
    const std::unordered_map<KeyT, ValueT> &map, const KeyT &t) {
  auto item = map.find(t);
  if (item != map.end()) return item->second;
  return ValueT();
}

template <typename KeyT, typename ValueT>
std::enable_if_t<std::is_pointer_v<ValueT>, const ValueT> Find(
    const std::unordered_map<KeyT, ValueT> &map, const KeyT &t) {
  auto item = map.find(t);
  if (item != map.end()) return item->second;
  return nullptr;
}

template <typename T>
class Set {
  std::unordered_set<T> set_;

 public:
  using value_type = T;
  bool Exist(const value_type &t) const { return set_.find(t) != set_.end(); }
  void Remove(const value_type &t) {
    while (Exist(t)) set_.erase(t);
  }
  void Insert(const value_type &t) { set_.insert(t); }

  void Clear() { set_.clear(); }
};

template <typename Key, typename Value>
class Map {
  std::unordered_map<Key, Value> key_to_value_;
  std::unordered_map<Value, Key> value_to_key_;

 public:
  Key GetKey(const Value &t) { return Find(value_to_key_, t); }

  Value GetValue(const Key &t) { return Find(key_to_value_, t); }
};

template <typename Key, typename Value>
class MapVector {
  std::unordered_map<Key, std::vector<Value>> key_to_value_;
  std::unordered_map<Value, Key> value_to_key_;

 public:
  void Insert(const Key &key, const Value &value) {
    if (!IsKeyExist(key)) {
      std::pair<Key, std::vector<Value>> pair;
      pair.first = key;
      key_to_value_.insert(pair);
    }
    key_to_value_.find(key)->second.push_back(value);

    value_to_key_.insert(std::make_pair(value, key));
  }

  bool IsKeyExist(const Key &key) const {
    return key_to_value_.find(key) != key_to_value_.end();
  }

  bool IsValueExist(const Value &value) const {
    return value_to_key_.find(value) != value_to_key_.end();
  }

  Key GetKey(const Value &value) { return Find(value_to_key_, value); }
  Key GetKey(const Value &value) const { return Find(value_to_key_, value); }

  std::vector<Value> GetValue(const Key &key) {
    return Find(key_to_value_, key);
  }
  std::vector<Value> GetValue(const Key &key) const {
    return Find(key_to_value_, key);
  }

  void Clear() {
    key_to_value_.clear();
    value_to_key_.clear();
  }
};

using ReadStream = file_handler::ReadStream;

class Container {
  MapVector<const ReadStream *, ReadStream *> source_found_map_;
  Set<const ReadStream *> find_queue_set_, load_queue_set_;
  std::vector<std::unique_ptr<ReadStream>> loaded_stream_;

 public:
  virtual ~Container() = default;

  ReadStream *AddFoundStream(const ReadStream *source_stream,
                             std::unique_ptr<ReadStream> found_stream);

  void InsertFind(const ReadStream *source_stream);
  virtual bool IsInFindQueue(const ReadStream *source_stream) const;
  void RemoveFind(const ReadStream *source_stream);

  void InsertLoad(const ReadStream *found_stream);
  virtual bool IsInLoadQueue(const ReadStream *found_stream) const;
  void RemoveLoad(const ReadStream *found_stream);

  bool IsSourceFound(const ReadStream *source_stream) const;
  bool IsFound(const ReadStream *found_stream) const;
  const ReadStream *GetSourceStream(const ReadStream *found_stream) const;
  std::vector<ReadStream *> GetFoundStream(
      const ReadStream *source_stream) const;

  virtual void Clear();
};

}  // namespace loader

}  // namespace fmr

#endif /* end of include guard: FMR_BITMAP_LOADER_CONTAINER */
