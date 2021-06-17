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
#include <fmr/iterator/input.h>
#include <fmr/nowide/fs.h>

namespace fmr {

namespace fs {

class DirectoryIterator : public iterator::BaseInput<nwd::fs::path> {
  nwd::fs::directory_iterator iterator_;

 public:
  DirectoryIterator() : iterator_(nwd::fs::end(iterator_)) {}
  DirectoryIterator(nwd::fs::path path) : iterator_(path){};

  DirectoryIterator &operator++() override {
    ++iterator_;
    return *this;
  }

  const nwd::fs::path &operator*() const override { return iterator_->path(); }

 protected:
  bool equal(const iterator::BaseInput<nwd::fs::path> &o) const override {
    auto &it = (DirectoryIterator &)(o);
    return it.iterator_ == iterator_;
  }
};

FilesystemInputContainer::FilesystemInputContainer(nwd::fs::path path)
    : path_(path) {
  it_ =
      iterator::Input<nwd::fs::path>(std::make_unique<DirectoryIterator>(path));
  end_ = iterator::Input<nwd::fs::path>(std::make_unique<DirectoryIterator>());
}

iterator::Input<nwd::fs::path> &FilesystemInputContainer::iterator() {
  return it_;
}

iterator::Input<nwd::fs::path> &FilesystemInputContainer::end() { return end_; }

}  // namespace fs

}  // namespace fmr
