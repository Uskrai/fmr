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

#ifndef INCLUDE_FMR_WX_ARCHIVE_FIND_H_
#define INCLUDE_FMR_WX_ARCHIVE_FIND_H_

#include <fmr/find/find.h>
#include <fmr/wx/archive/archive.h>
#include <fmr/wx/archive/context.h>
#include <fmr/wx/archive/entry.h>
#include <wx/archive.h>
#include <wx/stream.h>

namespace fmr {

namespace wx {

namespace archive {

class Find : public find::Find<Entry> {
  Archive archive_;
  Context *context_;

  using pmt = void (Find::*)();
  pmt next_ = &Find::Start;

  using ChildContainer = std::vector<std::unique_ptr<wxArchiveEntry>>;
  ChildContainer child_;
  ChildContainer::iterator child_it_;

 public:
  Find(Archive archive, Context *context);

  Context *GetContext() override { return context_; }

  void Next() override;

  bool HasNext() const override { return next_ != &Find::Done; }

  void SetRecursive(bool recursive) override {}
  bool IsRecursive() const override { return false; }
  bool CanRecursive() const override { return false; }

 protected:
  void SetNext(pmt next) { next_ = next; }
  void Done();

  void Start();
  void NonSeekableIterateChild();

  void SeekableIterateChild();
  void SortChild();
  void SendChild();
};

}  // namespace archive

}  // namespace wx

}  // namespace fmr

#endif  // INCLUDE_FMR_WX_ARCHIVE_FIND_H_
