/*
 *  Copyright (c) 2020 Uskrai
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

#include <fmr/compare/comparer.h>
#include <gtest/gtest.h>

#include <algorithm>
#include <iostream>
#include <random>

#include "fmr/compare/natural.h"

namespace fmr {

class TestSortable : public compare::Sortable {
  std::string string_;

 public:
  TestSortable(const std::string &str) : string_(str) {}

  const std::string &GetString() const { return string_; }
};

TestSortable GetStream(std::string name) {
  TestSortable stream(name);
  return stream;
}

std::wstring MakeName(size_t size) {
  std::wstring path;
  std::random_device dev;
  std::mt19937 rng(dev());
  std::uniform_int_distribution<> lower_char(97, 122), upper_char(65, 90);

  size_t i = 0;
  std::wstring string;
  while (i < size) {
    if (!(i & 2))
      string += (wchar_t)lower_char(rng);
    else
      string += (wchar_t)upper_char(rng);
    i++;
  }
  return string;
}

TEST(SortTest, NaturalTest) {
  std::vector<TestSortable> vec_string;

  for (size_t i = 0; i < 100; i++) {
    TestSortable test("testing");
    vec_string.push_back(test);
  }

  std::sort(vec_string.begin(), vec_string.end(), compare::Natural());
}

}  // namespace fmr

int main(int argc, char **argv) {
  testing::InitGoogleTest(&argc, argv);
  std::setlocale(LC_ALL, "");
  return RUN_ALL_TESTS();
}
