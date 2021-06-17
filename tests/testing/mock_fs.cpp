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
class MockIterator : public iterator::BaseInput<nwd::fs::path> {
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

  bool equal(const iterator::BaseInput<nwd::fs::path> &o) const override {
    auto &oc = (MockIterator &)(o);
    return it_ == oc.it_;
  }
};

class MockContainer : public iterator::InputContainer<nwd::fs::path> {
  nwd::fs::path parent_;

  std::vector<nwd::fs::path> child_;
  iterator::Input<nwd::fs::path> it_;
  iterator::Input<nwd::fs::path> end_;

 public:
  MockContainer(std::pair<nwd::fs::path, std::vector<nwd::fs::path>> it) {
    parent_ = it.first;
    child_ = it.second;

    it_ = iterator::Input<nwd::fs::path>(
        std::make_unique<MockIterator>(parent_, child_.begin()));
    end_ = iterator::Input<nwd::fs::path>(
        std::make_unique<MockIterator>(parent_, child_.end()));
  }

  iterator::Input<nwd::fs::path> &iterator() override { return it_; }

  iterator::Input<nwd::fs::path> &end() override { return end_; }
};

class MockContext : public Context {
 public:
  MOCK_METHOD(bool, Check, (const nwd::fs::path &));
  MOCK_METHOD(bool, Compare, (const nwd::fs::path &, const nwd::fs::path &));

  MOCK_METHOD(void, Consume, (nwd::fs::path &));

  MOCK_METHOD(std::unique_ptr<find::Find<>>, Find,
              (fmr::fs::Find *, const nwd::fs::path &));

  void Map(std::string name, std::vector<std::string> child) {}
};

class MockProvider {
  std::map<nwd::fs::path, std::vector<nwd::fs::path>> directory_;

  using UniqueInputContainer =
      std::unique_ptr<iterator::InputContainer<nwd::fs::path>>;

  using UniqueFind = std::unique_ptr<find::Find<nwd::fs::path>>;

  Context *context_;

 public:
  MockProvider() {}

  UniqueInputContainer Open(const nwd::fs::path &path) {
    auto it = directory_.find(path);

    if (it == directory_.end()) return nullptr;

    return std::make_unique<MockContainer>(*it);
  }

  UniqueFind Find(fs::Find *find, const nwd::fs::path &path) {
    return std::make_unique<fs::Find>(Open(path), find->GetContext());
  }

  void Map(nwd::fs::path name, std::vector<nwd::fs::path> child) {
    directory_.insert(std::make_pair(name, child));
  }
};

}  // namespace fs

}  // namespace fmr

#endif  // TESTS_TESTING_FS_MOCK_CPP_
