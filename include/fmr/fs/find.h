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

#ifndef INCLUDE_FMR_FS_FIND_H_
#define INCLUDE_FMR_FS_FIND_H_

#include <fmr/find/find.h>
#include <fmr/fs/provider.h>
#include <fmr/nowide/fs.h>

#include <functional>

namespace fmr {

namespace fs {

class Find : public find::Find<nwd::fs::path> {
  using ChildContainer =
      std::vector<std::unique_ptr<find::Find<nwd::fs::path>>>;

  ChildContainer child_;

  std::vector<nwd::fs::path> founds_;

  using ptm = void (Find::*)();
  ptm next_ = &Find::Start;
  nwd::fs::path path_;

  Provider *provider_ = nullptr;
  std::unique_ptr<InputContainer<nwd::fs::path>> container_;

  ChildContainer::iterator child_it_;
  std::vector<nwd::fs::path>::iterator found_it_;

 public:
  Find(nwd::fs::path path, Provider *provider);
  Find(std::string path, Provider *provider)
      : Find(nwd::fs::path(path), provider){};
  Find(Find &find, nwd::fs::path path);

  Find(Find &&o) = default;

  static bool CanFind(nwd::fs::path path);
  static bool CanFind(std::string path);

 protected:
  void Done();

 private:
  void Start();
  void IterateDirectory();
  void SortFound();
  void SendFound();
  void IterateChild();

  void DonePmt() {}

  void DoResume() override { (this->*next_)(); }

  void SetNext(ptm next) { next_ = next; };
  void SetNextAndResume(ptm next) {
    SetNext(next);
    if (!IsPaused()) return DoResume();
  }
};

}  // namespace fs

}  // namespace fmr

#endif  // INCLUDE_FMR_FS_FIND_H_
