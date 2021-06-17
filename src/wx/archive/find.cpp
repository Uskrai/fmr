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

Find::Find(Archive archive, Context *context)
    : archive_(std::move(archive)), context_(context) {}

void Find::Next() { (this->*next_)(); }

void Find::Done() { next_ = &Find::Done; }

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
}

void Find::NonSeekableIterateChild() {
  auto stream = archive_.GetInputStream();

  auto next_entry = [&stream]() {
    return std::unique_ptr<wxArchiveEntry>(stream->GetNextEntry());
  };

  auto entry = next_entry();

  if (entry) {
    Entry ent{entry.get(), archive_.GetInputStream()};
    if (GetContext()->Check(ent)) {
      GetContext()->Consume(ent);
    }
  } else {
    Done();
  }
}

void Find::SeekableIterateChild() {
  auto stream = archive_.GetInputStream();

  auto next_entry = [&stream]() {
    return std::unique_ptr<wxArchiveEntry>(stream->GetNextEntry());
  };

  auto entry = next_entry();

  if (entry) {
    Entry ent{entry.get(), archive_.GetInputStream()};
    if (GetContext()->Check(ent)) {
      child_.push_back(std::move(entry));
    }
  } else {
    SetNext(&Find::SortChild);
  }
}

void Find::SortChild() {
  auto compare = [this](const auto &t1, const auto &t2) {
    return GetContext()->Compare(t1, t2);
  };

  child_it_ = child_.begin();

  SetNext(&Find::SendChild);
}

void Find::SendChild() {
  auto &it = child_it_;
  auto stream = archive_.GetInputStream();

  auto &entry = **it;

  if (stream->OpenEntry(entry)) {
    Entry ent{&entry, stream};
    GetContext()->Consume(ent);
  }

  ++it;

  if (it == child_.end()) Done();
}

}  // namespace archive

}  // namespace wx

}  // namespace fmr
