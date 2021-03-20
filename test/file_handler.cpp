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

#include <fmr/common/path.h>
#include <fmr/file_handler/filesystem/handler.h>
#include <fmr/file_handler/wx_archive/handler.h>
#include <gtest/gtest.h>

#include "fmr/file_handler/memory_stream.h"

namespace test_path {
using namespace fmr::file_handler;
const char *GetDirName(Handler &handler) { return "dir_test"; }
const char *GetDirName(wx_archive::Handler &handler) { return "zip_test.zip"; }

std::string GetRelativePath(Handler &handler) {
  return fmr::Path::MakeRelative(".", handler.GetPath());
}

std::string GetAbsolutePath(Handler &handler) {
  return fmr::Path::MakeAbsolute(handler.GetPath());
}

}  // namespace test_path

template <typename HandlerType>
class HandlerFixture : public ::testing::TestWithParam<HandlerType> {
 protected:
  HandlerType handler_;
  const char *path = test_path::GetDirName(handler_);

  void SettingUp(fmr::file_handler::Handler &handler) {
    //
  }
  void SetUp() override {
    SettingUp(handler_);
    fmr::file_handler::Handler &handler = handler_;
    handler.Open(path);
    // handler.Write().Create();
  }

  void TearDown() override {
    //
  }
  //
};

namespace type {
using namespace fmr::file_handler;
using HandlerFixtureType = testing::Types<filesystem::Handler>;
}  // namespace type

void test_open(fmr::file_handler::Handler &handler, std::string path) {
  ASSERT_FALSE(handler.IsOk());
  handler.Open("");
  ASSERT_FALSE(handler.IsOk());
  handler.Open(path);
  ASSERT_EQ(handler.GetPath(), path);

  handler.Open(test_path::GetAbsolutePath(handler));
  ASSERT_EQ(handler.GetPath(), test_path::GetAbsolutePath(handler));

  handler.Open(test_path::GetRelativePath(handler));
  ASSERT_EQ(handler.GetPath(), test_path::GetRelativePath(handler));
}

void create_dumies(fmr::file_handler::Handler &handler,
                   fmr::file_handler::Stream *stream,
                   fmr::file_handler::WriteType type, size_t start,
                   size_t count) {
  for (; start < count; ++start) {
    handler.Write()->CreateFile(std::to_string(start), stream, type);
  }
  handler.Write()->CommitWrite();
}

void test_write(fmr::file_handler::Handler &handler, std::string path) {
  ASSERT_FALSE(handler.IsExist(path))
      << path << " is going to be used as test directory";
  ASSERT_FALSE(handler.IsOk()) << "Should be false when pat is not exist";

  handler.Open(path);
  ASSERT_EQ(handler.GetPath(), path);
  handler.Write()->Create();

  constexpr size_t buff_size = 1000;
  char ch[buff_size];
  fmr::file_handler::MemoryStream stream;
  stream.Write(&ch, 1000);

  for (int i = 1; i < 100; ++i) {
    handler.Write()->CreateFile(std::to_string(i), &stream,
                                fmr::file_handler::kWriteNone);
    handler.Write()->CommitWrite();

    handler.Read()->Traverse(false);
    ASSERT_EQ(handler.Read()->Size(), i);

    for (const auto &it : handler.Read()->GetChild()) {
      ASSERT_EQ(it->Size(), 0);
      it->Load();
      ASSERT_EQ(it->Size(), buff_size);
    }
  }

  handler.Write()->Delete();

  ASSERT_FALSE(handler.IsExist(path));
}

void test_overwrite(fmr::file_handler::Handler &handler, std::string path) {
  handler.Open(path);
  ASSERT_EQ(handler.GetPath(), path);
  ASSERT_FALSE(handler.IsExist(path))
      << path << " is going to be used as test directory";
  ASSERT_FALSE(handler.IsOk()) << "Should be false when path is not exist";

  handler.Write()->Create();

  constexpr size_t initial_size = 1000;
  constexpr size_t overwriten_size = 3000;

  for (size_t i = 0; i < 100; ++i) {
    using namespace fmr::file_handler;

    char ch[initial_size];
    MemoryStream stream;
    stream.Write(&ch, initial_size);

    handler.Write()->CreateFile(std::to_string(i), &stream, kWriteNone);
    handler.Write()->CommitWrite();
  }

  for (size_t i = 0; i < 50; ++i) {
    using namespace fmr::file_handler;
    char ch[overwriten_size];
    MemoryStream stream;
    stream.Write(&ch, overwriten_size);

    handler.Write()->CreateFile(std::to_string(i), &stream, kWriteOverwrite);
    handler.Write()->CommitWrite();
  }

  handler.Read()->Traverse(false);
  auto vec = handler.Read()->GetChild();

  std::sort(vec.begin(), vec.end(),
            [](const fmr::file_handler::Stream *s1,
               const fmr::file_handler::Stream *s2) {
              return std::stoi(s1->GetName()) < std::stoi(s2->GetName());
            });

  size_t i = 0;
  while (i < 50) {
    ASSERT_EQ(vec[i]->Size(), 0);
    vec[i]->Load();
    ASSERT_EQ(vec[i]->Size(), overwriten_size);
    ++i;
  }

  while (i < 100) {
    ASSERT_EQ(vec[i]->Size(), 0);
    vec[i]->Load();
    ASSERT_EQ(vec[i]->Size(), initial_size);
    ++i;
  }

  handler.Write()->Delete();
}

void test_parent(fmr::file_handler::Handler &handler, std::string path) {
  // handler.GetParent()->GetPath();
}

TYPED_TEST_SUITE(HandlerFixture, type::HandlerFixtureType);

TYPED_TEST(HandlerFixture, TestOpen) { test_open(this->handler_, this->path); }

TYPED_TEST(HandlerFixture, TestWrite) {
  test_write(this->handler_, this->path);
}

TYPED_TEST(HandlerFixture, TestOverwrite) {
  test_overwrite(this->handler_, this->path);
}

TEST(StreamTest, TestWrite) {
  fmr::file_handler::MemoryStream stream;

  constexpr size_t buff_size = 1000;
  char ch[buff_size];
  stream.Write(&ch, buff_size);

  auto stream_2 = stream;

  ASSERT_EQ(stream.Size(), stream_2.Size());

  stream_2.Write(&ch, buff_size);

  ASSERT_NE(stream.Size(), stream_2.Size());

  ASSERT_EQ(stream.Size(), buff_size);

  ASSERT_EQ(stream_2.Size(), buff_size * 2);
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}

// TEST_P(HandlerFixture, FirstTest) {}

// TEST_P(HandlerFixture, Testing) {
//
// }
