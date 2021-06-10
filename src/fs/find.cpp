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

Find::Find(nwd::fs::path path, Provider *provider)
    : find::Find<nwd::fs::path>(provider), path_(path), provider_(provider) {}

Find::Find(Find &find, nwd::fs::path path)
    : find::Find<nwd::fs::path>(find), path_(path), provider_(find.provider_) {}

void Find::Done() {
  find::Find<nwd::fs::path>::Done(true);
  SetNext(&Find::DonePmt);
}

void Find::Start() {
  container_ = provider_->Open(path_);

  if (container_)
    SetNext(&Find::IterateDirectory);
  else
    Done();
  Resume();
}

void Find::IterateDirectory() {
  auto &it = container_->iterator();

  for (; it != container_->end(); ++it) {
    if (IsPaused()) return;

    auto &path = *it;

    if (Check(path)) {
      founds_.push_back(path);
    }

    auto find = provider_->Find(this, path);
    child_.push_back(std::move(find));
  }

  SetNext(&Find::SortFound);
  if (IsPaused()) return;
  Resume();
}

void Find::SortFound() {
  if (GetComparer()) GetComparer()->Sort(founds_.begin(), founds_.end());
  found_it_ = founds_.begin();

  SetNext(&Find::SendFound);
  if (IsPaused()) return;
  Resume();
}

void Find::SendFound() {
  auto &it = found_it_;

  for (; it != founds_.end(); ++it) {
    if (IsPaused()) return;

    provider_->Consume(*it);
  }

  if (IsRecursive()) {
    child_it_ = child_.begin();
    SetNext(&Find::IterateChild);
    Resume();
  } else {
    Done();
  }
}

void Find::IterateChild() {
  auto &it = child_it_;

  while (it != child_.end()) {
    auto &find = *it;

    if (IsPaused()) return;
    find->Resume();
    if (find->IsDone()) ++it;
  }

  Done();
}

bool Find::CanFind(nwd::fs::path path) { return nwd::fs::is_directory(path); }

bool Find::CanFind(std::string path) { return CanFind(nwd::fs::path(path)); }

}  // namespace fs

}  // namespace fmr
