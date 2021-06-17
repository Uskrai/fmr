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

#include <fmr/fs/filesystem.h>
#include <fmr/fs/find.h>
#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <iostream>
#include <map>

#include "testing/mock_fs.cpp"

namespace fmr {

namespace fs {

template <template <class MockProvider> class Mock>
class FSFindFixtureTemplate : public testing::Test {
 protected:
  MockProvider provider_;
  Mock<MockContext> mock_;

  void SetUp() override {
    ON_CALL(mock_, Find(testing::_, testing::_))
        .WillByDefault(testing::Invoke(&provider_, &MockProvider::Find));
  }

  Find Open(nwd::fs::path path) { return Find(provider_.Open(path), &mock_); }

  void Map(nwd::fs::path path, std::vector<nwd::fs::path> child) {
    return provider_.Map(path, child);
  }
};

using FSFindFixture = FSFindFixtureTemplate<testing::NaggyMock>;
using NiceFSFindFixture = FSFindFixtureTemplate<testing::NiceMock>;
using StrictFSFindFixture = FSFindFixtureTemplate<testing::StrictMock>;

TEST_F(NiceFSFindFixture, Testing) {
  auto &mock = mock_;

  Map("/", {"a", "b", "c"});
  Map("/a", {"aa", "ab", "ac"});

  EXPECT_CALL(mock, Find(testing::_, testing::_)).Times(5);

  EXPECT_CALL(mock, Find(testing::_, nwd::fs::path("/a"))).Times(1);

  auto find = Open("/");
  find.SetRecursive(true);

  while (find.HasNext()) find.Next();
}

TEST_F(NiceFSFindFixture, Checker) {
  using nwd::fs::path;
  auto &mock = mock_;

  Map("/", {"a", "b", "c"});

  EXPECT_CALL(mock, Find(testing::_, testing::_)).Times(2);
  EXPECT_CALL(mock, Find(testing::_, path("/a"))).Times(1);

  EXPECT_CALL(mock, Check(testing::_)).WillRepeatedly(testing::Return(false));
  EXPECT_CALL(mock, Check(path("/a")))
      .Times(2)
      .WillRepeatedly(testing::Return(true));

  EXPECT_CALL(
      mock,
      Consume(testing::Truly([](const auto &it) { return it == path("/a"); })))
      .WillOnce(testing::Invoke(&mock, &MockContext::Check));

  auto find = Open("/");
  find.SetRecursive(true);

  while (find.HasNext()) find.Next();
}

TEST_F(NiceFSFindFixture, NonRecursive) {
  using nwd::fs::path;

  Map("/", {"a", "b", "c"});
  Map("/a", {"aa", "ab", "ac"});

  auto find = Open("/");

  EXPECT_CALL(mock_, Find(testing::_, testing::_)).Times(0);

  EXPECT_CALL(mock_, Find(&find, testing::_)).Times(3);

  {
    auto deny = {"aa", "ab", "ac"};
    for (auto &it : deny) {
      EXPECT_CALL(mock_, Find(testing::_, path("/a/" + std::string(it))))
          .Times(0);
    }
  }

  find.SetRecursive(false);

  while (find.HasNext()) find.Next();
}

}  // namespace fs

}  // namespace fmr
