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

#include <fmr/fs/find.h>

#include <cassert>

namespace fmr {

namespace fs {

Find::Find(std::unique_ptr<iterator::InputContainer<nwd::fs::path>> container,
           Context *context)
    : container_(std::move(container)), context_(context) {}

void Find::Done() { SetNext(&Find::Done); }

void Find::Start() {
  if (container_) {
    SetNext(&Find::IterateDirectory);
  } else
    Done();
}

void Find::IterateDirectory() {
  auto &it = container_->iterator();

  if (it != container_->end()) {
    if (GetContext()->Check(*it)) founds_.push_back(*it);

    auto find = GetContext()->Find(this, *it);
    assert(find && "GetContext()->Find() should not return null");
    if (find) child_.push_back(std::move(find));

    ++it;
  } else {
    SetNext(&Find::SortFound);
  }
}

void Find::SortFound() {
  auto compare = [this](const auto &t1, const auto &t2) {
    return GetContext()->Compare(t1, t2);
  };

  std::sort(founds_.begin(), founds_.end(), compare);

  found_it_ = founds_.begin();

  SetNext(&Find::SendFound);
}

void Find::SendFound() {
  auto &it = found_it_;

  if (it != founds_.end()) {
    context_->Consume(*it);
    ++it;
  } else {
    if (IsRecursive()) {
      child_it_ = child_.begin();
      SetNext(&Find::IterateChild);
    } else {
      Done();
    }
  }
}

void Find::IterateChild() {
  auto &it = child_it_;

  if (it != child_.end()) {
    auto &find = *it;
    find->Next();
    if (!find->HasNext()) ++it;
  } else {
    Done();
  }
}

bool Find::CanFind(nwd::fs::path path) { return nwd::fs::is_directory(path); }

bool Find::CanFind(std::string path) { return CanFind(nwd::fs::path(path)); }

}  // namespace fs

}  // namespace fmr
