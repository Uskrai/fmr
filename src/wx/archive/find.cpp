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

#include <fmr/wx/archive/find.h>
#include <wx/archive.h>
#include <wx/stream.h>

namespace fmr {

namespace wx {

namespace archive {

using BaseFind = find::Find<wxArchiveEntry>;

std::string ToStdString(wxString string) {
  return std::string(string.utf8_str());
}

Find::Find(Provider *provider, Archive archive)
    : BaseFind(provider), archive_(std::move(archive)), provider_(provider) {}

Find::Find(Find &parent, Archive archive)
    : BaseFind(parent),
      provider_(parent.provider_),
      archive_(std::move(archive)) {}

Find::Find(find::Find<wxArchiveEntry> &parent, Provider *provider,
           Archive archive)
    : BaseFind(parent), archive_(std::move(archive)), provider_(provider) {}

void Find::DoResume() { (this->*next_)(); }

void Find::Done() {
  task::Task::Done(true);
  next_ = &Find::Done;
}

void Find::Start() {
  auto stream = archive_.GetInputStream();

  if (!stream) {
    return Done();
  }

  if (stream->IsSeekable()) {
    SetNext(&Find::SeekableIterateChild);
  } else {
    SetNext(&Find::NonSeekableIterateChild);
  }

  DoResume();
}

void Find::NonSeekableIterateChild() {
  auto stream = archive_.GetInputStream();

  auto next_entry = [&stream]() {
    return std::unique_ptr<wxArchiveEntry>(stream->GetNextEntry());
  };

  while (auto entry = next_entry()) {
    if (provider_->Check(*entry)) {
      provider_->ConsumeNonSeekable(*entry, *stream);
    }

    if (IsPaused()) return;
  }

  Done();
}

void Find::SeekableIterateChild() {
  auto stream = archive_.GetInputStream();

  auto next_entry = [&stream]() {
    return std::unique_ptr<wxArchiveEntry>(stream->GetNextEntry());
  };

  while (auto entry = next_entry()) {
    auto name = entry->GetName();

    if (Check(*entry)) {
      child_.push_back(std::move(entry));
    }

    if (IsPaused()) return;
  }

  SetNextAndResume(&Find::SortChild);
}

void Find::SortChild() {
  if (comparer_) {
    auto sorter = [this](const auto &t1, const auto &t2) {
      return comparer_->operator()(*t1, *t2);
    };

    std::sort(child_.begin(), child_.end(), sorter);
  }
  child_it_ = child_.begin();

  if (IsPaused()) return;

  SetNext(&Find::SendChild);
  DoResume();
}

void Find::SendChild() {
  auto &it = child_it_;
  auto stream = archive_.GetInputStream();

  while (it != child_.end()) {
    auto &entry = **it;
    if (stream->OpenEntry(entry)) {
      stream->OpenEntry(entry);
      provider_->ConsumeSeekable(entry, *stream);
    }
  }
}

}  // namespace archive

}  // namespace wx

}  // namespace fmr
