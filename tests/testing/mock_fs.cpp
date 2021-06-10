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

#ifndef TESTS_TESTING_FS_MOCK_CPP_
#define TESTS_TESTING_FS_MOCK_CPP_

#include <fmr/fs/filesystem.h>
#include <fmr/fs/find.h>
#include <gmock/gmock.h>

namespace fmr {

namespace fs {

/**
 * Mock Iteartor used by MockProvider
 */
class MockIterator : public BaseInputIterator<nwd::fs::path> {
  nwd::fs::path parent_;
  std::vector<nwd::fs::path>::iterator it_;

  mutable nwd::fs::path temp_;

 public:
  MockIterator(nwd::fs::path parent, std::vector<nwd::fs::path>::iterator it) {
    parent_ = parent;
    it_ = it;
  }

  MockIterator &operator++() override {
    ++it_;
    return *this;
  }

  const nwd::fs::path &operator*() const override {
    temp_ = (parent_ / *it_);
    return temp_;
  }

  bool equal(const BaseInputIterator<nwd::fs::path> &o) const override {
    auto &oc = (MockIterator &)(o);
    return it_ == oc.it_;
  }
};

class MockContainer : public InputContainer<nwd::fs::path> {
  nwd::fs::path parent_;

  std::vector<nwd::fs::path> child_;
  InputIterator<nwd::fs::path> it_;
  InputIterator<nwd::fs::path> end_;

 public:
  MockContainer(std::pair<nwd::fs::path, std::vector<nwd::fs::path>> it) {
    parent_ = it.first;
    child_ = it.second;

    it_ = InputIterator<nwd::fs::path>(
        std::make_unique<MockIterator>(parent_, child_.begin()));
    end_ = InputIterator<nwd::fs::path>(
        std::make_unique<MockIterator>(parent_, child_.end()));
  }

  InputIterator<nwd::fs::path> &iterator() override { return it_; }

  InputIterator<nwd::fs::path> &end() override { return end_; }
};

class MockProvider : public Provider {
  std::map<nwd::fs::path, std::vector<nwd::fs::path>> directory_;

  std::function<bool(const nwd::fs::path &)> checker_ =
      [](const nwd::fs::path &) { return false; };

  std::function<void(const nwd::fs::path &)> consumer_ = [](const path &) {};

 public:
  MOCK_METHOD(UniqueInputContainer, Open, (const nwd::fs::path &));

  MOCK_METHOD(UniqueFind, Find, (fs::Find *, const nwd::fs::path &));

  MOCK_METHOD(bool, Check, (const nwd::fs::path &));

  MOCK_METHOD(void, Consume, (const nwd::fs::path &));

  void Map(const nwd::fs::path &path, std::vector<nwd::fs::path> child) {
    directory_.emplace(std::make_pair(path, child));
  }

  UniqueInputContainer DefaultOpen(const nwd::fs::path &path) {
    auto it = directory_.find(path);

    if (it == directory_.end()) return nullptr;

    return std::make_unique<MockContainer>(*it);
  }

  void SetChecker(std::function<bool(const nwd::fs::path &)> checker) {
    checker_ = checker;
  }

  bool DefaultCheck(const nwd::fs::path &path) { return checker_(path); }

  UniqueFind DefaultFind(fs::Find *find, const nwd::fs::path &path) {
    return std::make_unique<fs::Find>(*find, path);
  }

  void DefaultConsume(const nwd::fs::path &path) { consumer_(path); }

  void SetConsumer(std::function<void(const nwd::fs::path &)> consumer) {
    consumer_ = consumer;
  }
};

}  // namespace fs

}  // namespace fmr

#endif  // TESTS_TESTING_FS_MOCK_CPP_
