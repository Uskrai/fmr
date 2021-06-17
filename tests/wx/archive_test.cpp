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

#include "fmr/wx/archive/archive.h"

#include <fmr/wx/archive/find.h>
#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <wx/mstream.h>
#include <wx/zipstrm.h>

#include <iostream>

namespace fmr {

namespace wx {

namespace archive {

class MockContext : public Context {
 public:
  MOCK_METHOD(bool, Check, (const Entry &));

  MOCK_METHOD(void, Consume, (Entry &));

  MOCK_METHOD(bool, Compare, (const Entry &t1, const Entry &t2));
};

struct Seekable {};
struct NonSeekable {};

template <typename Type>
class ArchiveTest : public testing::Test {
 protected:
  MockContext mock_;
  wxMemoryOutputStream out_stream_;
  wxZipOutputStream arc_stream_{out_stream_};
  std::vector<wxString> content_;

  Type type_;

  void SetUp() override {
    PutNextDirEntry("a", {"aa", "ab"});
    PutNextDirEntry("b", {"ba", "bb"});
    PutNextEntry("c");
  }

  void PutNextDirEntry(wxString name, std::vector<wxString> child) {
    PutNextEntry(name + "/");
    for (const auto &it : child) {
      PutNextEntry(name + "/" + it);
      char a;
      arc_stream_.Write(&a, 1);
    }
  }

  void PutNextEntry(wxString name) {
    name = wxFileName(name).GetFullPath(wxPATH_UNIX);
    arc_stream_.PutNextEntry(name);
    content_.push_back(name);
  }
};

using NonSeekableArchiveTest = ArchiveTest<NonSeekable>;
using SeekableArchiveTest = ArchiveTest<Seekable>;

using ArchiveTestTypes = ::testing::Types<Seekable, NonSeekable>;

TYPED_TEST_SUITE(ArchiveTest, ArchiveTestTypes);

TYPED_TEST(ArchiveTest, Testing) {
  wxArchiveOutputStream &arc_stream = this->arc_stream_;
  MockContext &mock = this->mock_;
  arc_stream.Close();

  wxMemoryInputStream stream(this->out_stream_);

  {
    std::vector<wxString> &content = this->content_;
    for (auto it : content) {
      EXPECT_CALL(mock, Check(testing::Truly([it](const Entry &entry) {
                    return entry.entry->GetName(wxPATH_UNIX) == it;
                  })))
          .WillOnce(testing::Return(true));

      EXPECT_CALL(mock, Consume(testing::Truly([it](Entry &entry) {
                    return entry.entry->GetName(wxPATH_UNIX) == it;
                  })));
    }
  }

  Find find(Archive("a.zip", stream), &mock);

  while (find.HasNext()) find.Next();
}

}  // namespace archive

}  // namespace wx

}  // namespace fmr
