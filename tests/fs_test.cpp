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

class Checker : public find::Checker<nwd::fs::path> {
  std::function<bool(const nwd::fs::path &)> checker_ = [](auto a) {
    return false;
  };

 public:
  MOCK_METHOD1(Check, bool(const nwd::fs::path &));

  void SetCheck(std::function<bool(const nwd::fs::path &)> checker) {
    checker_ = checker;
  }
};

template <template <class MockProvider> class Mock>
class FSFindFixtureTemplate : public testing::Test {
 protected:
  Mock<MockProvider> mock_;
  Find find_{nwd::fs::path{"/"}, &mock_};

  void SetUp() override {
    ON_CALL(mock_, Open(testing::_))
        .WillByDefault(testing::Invoke(&mock_, &MockProvider::DefaultOpen));

    ON_CALL(mock_, Check(testing::_))
        .WillByDefault(testing::Invoke(&mock_, &MockProvider::DefaultCheck));

    ON_CALL(mock_, Find(testing::_, testing::_))
        .WillByDefault(testing::WithArgs<0, 1>(
            testing::Invoke(&mock_, &MockProvider::DefaultFind)));

    ON_CALL(mock_, Consume(testing::_))
        .WillByDefault(testing::Invoke(&mock_, &MockProvider::DefaultConsume));
  }
};

using FSFindFixture = FSFindFixtureTemplate<testing::NaggyMock>;
using NiceFSFindFixture = FSFindFixtureTemplate<testing::NiceMock>;
using StrictFSFindFixture = FSFindFixtureTemplate<testing::StrictMock>;

TEST_F(NiceFSFindFixture, Testing) {
  auto &mock = mock_;
  auto &find = find_;

  mock.Map("/", {"a", "b", "c"});
  mock.Map("/a", {"aa", "ab", "ac"});

  EXPECT_CALL(mock, Open(testing::_)).Times(5);

  EXPECT_CALL(mock, Open(nwd::fs::path("/"))).Times(1);
  EXPECT_CALL(mock, Open(nwd::fs::path("/a"))).Times(1);

  mock.SetConsumer([](auto a) {});
  find.SetRecursive(true);
  find.Resume();
}

TEST_F(NiceFSFindFixture, Checker) {
  using nwd::fs::path;
  auto &mock = mock_;
  auto &find = find_;

  mock.Map("/", {"a", "b", "c"});

  EXPECT_CALL(mock, Open(testing::_)).Times(3);
  EXPECT_CALL(mock, Open(path("/a"))).Times(1);

  EXPECT_CALL(mock, Check(testing::_)).WillRepeatedly(testing::Return(false));
  EXPECT_CALL(mock, Check(path("/a")))
      .Times(2)
      .WillRepeatedly(testing::Return(true));

  mock.SetConsumer([&mock](auto a) { mock.Check(a); });
  find.SetRecursive(true);
  find.Resume();
}

TEST_F(NiceFSFindFixture, NonRecursive) {
  using nwd::fs::path;

  mock_.Map("/", {"a", "b", "c"});
  mock_.Map("/a", {"aa", "ab", "ac"});

  EXPECT_CALL(mock_, Open(path("/"))).Times(1);

  mock_.SetConsumer([](auto a) {});
  find_.SetRecursive(false);
  find_.Resume();
}

}  // namespace fs

}  // namespace fmr
