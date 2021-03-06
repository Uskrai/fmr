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

#ifndef INCLUDE_FMR_MACRO_FLAGS_H_
#define INCLUDE_FMR_MACRO_FLAGS_H_

#include <type_traits>

#define FMR_MACRO_DEFINE_ENUM_FLAG_OPERATORS(T)                            \
  constexpr inline T operator~(T a) {                                      \
    return static_cast<T>(~static_cast<std::underlying_type<T>::type>(a)); \
  }                                                                        \
                                                                           \
  constexpr inline T operator|(T a, T b) {                                 \
    return static_cast<T>(static_cast<std::underlying_type<T>::type>(a) |  \
                          static_cast<std::underlying_type<T>::type>(b));  \
  }                                                                        \
                                                                           \
  constexpr inline T operator&(T a, T b) {                                 \
    return static_cast<T>(static_cast<std::underlying_type<T>::type>(a) &  \
                          static_cast<std::underlying_type<T>::type>(b));  \
  }                                                                        \
                                                                           \
  constexpr inline T operator^(T a, T b) {                                 \
    return static_cast<T>(static_cast<std::underlying_type<T>::type>(a) ^  \
                          static_cast<std::underlying_type<T>::type>(b));  \
  }                                                                        \
                                                                           \
  inline T& operator|=(T& a, T b) {                                        \
    return reinterpret_cast<T&>(                                           \
        reinterpret_cast<std::underlying_type<T>::type&>(a) |=             \
        static_cast<std::underlying_type<T>::type>(b));                    \
  }                                                                        \
                                                                           \
  inline T& operator&=(T& a, T b) {                                        \
    return reinterpret_cast<T&>(                                           \
        reinterpret_cast<std::underlying_type<T>::type&>(a) &=             \
        static_cast<std::underlying_type<T>::type>(b));                    \
  }                                                                        \
                                                                           \
  inline T& operator^=(T& a, T b) {                                        \
    return reinterpret_cast<T&>(                                           \
        reinterpret_cast<std::underlying_type<T>::type&>(a) ^=             \
        static_cast<std::underlying_type<T>::type>(b));                    \
  }

#endif  // INCLUDE_FMR_MACRO_FLAGS_H_
