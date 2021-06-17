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
#include <fmr/fs/context.h>
#include <fmr/iterator/input.h>
#include <fmr/nowide/fs.h>

#include <functional>

namespace fmr {

namespace fs {

class Find : public find::Find<nwd::fs::path> {
  using ChildContainer = std::vector<std::unique_ptr<find::Find<>>>;
  ChildContainer child_;

  bool recursive_{false};

  Context *context_{nullptr};

  std::vector<nwd::fs::path> founds_;

  using ptm = void (Find::*)();
  ptm next_ = &Find::Start;
  nwd::fs::path path_;

  std::unique_ptr<iterator::InputContainer<nwd::fs::path>> container_;

  ChildContainer::iterator child_it_;
  std::vector<nwd::fs::path>::iterator found_it_;

 public:
  Find(std::unique_ptr<iterator::InputContainer<nwd::fs::path>> container,
       Context *context);

  Find(Find &&o);

  static bool CanFind(nwd::fs::path path);
  static bool CanFind(std::string path);

  bool IsRecursive() const override { return recursive_; }
  void SetRecursive(bool recursive) override { recursive_ = recursive; }
  bool CanRecursive() const override { return true; }

  void Next() override { (this->*next_)(); }
  virtual bool HasNext() const override { return next_ != &Find::Done; }

  Context *GetContext() override { return context_; }

 protected:
  void Done();

 private:
  void Start();

  void IterateDirectory();
  void SortFound();
  void SendFound();
  void IterateChild();

  void SetNext(ptm next) { next_ = next; };
};

}  // namespace fs

}  // namespace fmr

#endif  // INCLUDE_FMR_FS_FIND_H_
