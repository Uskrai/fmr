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

#ifndef FMR_COMMON_VECTOR
#define FMR_COMMON_VECTOR

#include <vector>

#include "stddef.h"

namespace fmr {

namespace Vector {
template <typename T>
constexpr bool IsExist(const T& vec, size_t idx) {
  return idx < vec.size();
}

template <typename T>
void Push(const T& source, T& destination) {
  for (const auto& it : source) destination.push_back(it);
}

template <typename T>
auto ConvertToPtr(std::vector<T>& vec) {
  std::vector<T*> ret;
  for (auto& it : vec) {
    ret.push_back(&it);
  }
  return ret;
}

template <typename T>
auto ConvertToPtr(const std::vector<T>& vec) {
  std::vector<const T*> ret;
  for (auto& it : vec) {
    ret.push_back(&it);
  }
  return ret;
}

}  // namespace Vector

}  // namespace fmr

#endif
