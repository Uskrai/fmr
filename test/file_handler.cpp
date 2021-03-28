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

#include "fmr/compare/natural.h"
#include "fmr/file_handler/memory_stream.h"
#include "fmr/file_handler/write_type.h"

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

namespace var {

constexpr size_t kBufferSize = 10;
constexpr size_t kSecondBufferSize = kBufferSize * 2;
constexpr size_t kLoopCount = 25;
constexpr size_t kSecondLoopCount = kLoopCount * 2;

}  // namespace var

template <typename HandlerType>
class HandlerFixture : public ::testing::TestWithParam<HandlerType> {
 protected:
  HandlerType handler_;
  const char *path = test_path::GetDirName(handler_);

 private:
  bool should_delete_ = false;

 protected:
  void SetUp() override {
    fmr::file_handler::Handler &handler = handler_;
    should_delete_ = false;

    ASSERT_FALSE(handler.IsOk());
    handler.Open(path);
    ASSERT_FALSE(handler.IsExist())
        << this->path << " Is going to be used as testing directory";
    ASSERT_FALSE(handler.IsOk());

    handler.Write()->Create();
    ASSERT_TRUE(handler.IsExist());
    ASSERT_TRUE(handler.IsOk());

    should_delete_ = true;
  }

  void TearDown() override {
    if (should_delete_) {
      fmr::file_handler::Handler &handler = handler_;

      handler.Open(path);
      handler.Write()->Delete();

      ASSERT_FALSE(handler.IsExist())
          << path << " still exist after Write()->Delete()";
    }
  }
};

namespace type {
using namespace fmr::file_handler;
using HandlerFixtureType =
    testing::Types<filesystem::Handler, wx_archive::Handler>;
}  // namespace type

void test_open(fmr::file_handler::Handler &handler, std::string path) {
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
  constexpr size_t buff_size = var::kBufferSize;
  char ch[buff_size];
  fmr::file_handler::MemoryStream stream;
  stream.Write(&ch, buff_size);

  for (size_t i = 1; i < var::kLoopCount; ++i) {
    handler.Write()->CreateFile(std::to_string(i), &stream,
                                fmr::file_handler::kWriteNone);

    handler.Write()->CommitWrite();
    handler.Read()->Traverse(false);
    ASSERT_EQ(handler.Read()->Size(), i);
  }

  for (auto &it : *handler.Read()) {
    ASSERT_EQ(it.Size(), 0);
    it.Load();
    ASSERT_EQ(it.Size(), buff_size);
  }
}

void test_overwrite(fmr::file_handler::Handler &handler, std::string path) {
  constexpr size_t initial_size = var::kBufferSize;
  constexpr size_t overwriten_size = var::kSecondBufferSize;

  for (size_t i = 0; i < var::kSecondLoopCount; ++i) {
    using namespace fmr::file_handler;

    char ch[initial_size];
    MemoryStream stream;
    stream.Write(&ch, initial_size);

    handler.Write()->CreateFile(std::to_string(i), &stream, kWriteNone);
    handler.Write()->CommitWrite();
  }

  for (size_t i = 0; i < var::kLoopCount; ++i) {
    using namespace fmr::file_handler;
    char ch[overwriten_size];
    MemoryStream stream;
    stream.Write(&ch, overwriten_size);

    handler.Write()->CreateFile(std::to_string(i), &stream, kWriteOverwrite);
    handler.Write()->CommitWrite();
  }

  handler.Read()->Traverse(false);
  handler.Read()->Sort(fmr::compare::Natural());

  ASSERT_EQ(handler.Read()->Size(), var::kSecondLoopCount);

  size_t i = 0;
  while (i < var::kLoopCount) {
    ASSERT_EQ(handler.Read()->At(i)->Size(), 0);
    handler.Read()->At(i)->Load();
    ASSERT_EQ(handler.Read()->At(i)->Size(), overwriten_size);
    ++i;
  }

  while (i < var::kSecondLoopCount) {
    ASSERT_EQ(handler.Read()->At(i)->Size(), 0);
    handler.Read()->At(i)->Load();
    ASSERT_EQ(handler.Read()->At(i)->Size(), initial_size);
    ++i;
  }
}

TYPED_TEST_SUITE(HandlerFixture, type::HandlerFixtureType, );

TYPED_TEST(HandlerFixture, TestOpen) { test_open(this->handler_, this->path); }

TYPED_TEST(HandlerFixture, TestWrite) {
  test_write(this->handler_, this->path);
}

TYPED_TEST(HandlerFixture, TestOverwrite) {
  test_overwrite(this->handler_, this->path);
}

TEST(StreamTest, TestWrite) {
  fmr::file_handler::MemoryStream stream;

  constexpr size_t buff_size = var::kBufferSize;
  char ch[buff_size];
  stream.Write(&ch, buff_size);

  auto stream_2 = stream;

  ASSERT_EQ(stream.Size(), stream_2.Size());

  stream_2.Write(&ch, buff_size);

  ASSERT_NE(stream.Size(), stream_2.Size());

  ASSERT_EQ(stream.Size(), buff_size);

  ASSERT_EQ(stream_2.Size(), buff_size * 2);
}

TYPED_TEST(HandlerFixture, TestDelete) {
  fmr::file_handler::Handler &handler = this->handler_;
  fmr::file_handler::MemoryStream stream;
  constexpr size_t buff_size = var::kBufferSize;
  char ch[buff_size];
  stream.Write(&ch, buff_size);

  handler.Write()->CreateFile("1", &stream, fmr::file_handler::kWriteNone);
  handler.Write()->DeleteDirectory("1");
  handler.Write()->CommitWrite();
  handler.Read()->Traverse(false);

  ASSERT_EQ(handler.Read()->Size(), 1);
  handler.Write()->DeleteFile("1");
  handler.Write()->CommitWrite();
  handler.Read()->Traverse(false);
  ASSERT_EQ(handler.Read()->Size(), 0) << " file not deleted";

  handler.Write()->CreateDirectory("test");
  for (size_t i = 0; i < var::kLoopCount; ++i) {
    handler.Write()->CreateFile("test/" + std::to_string(i), &stream,
                                fmr::file_handler::kWriteNone);
  }
  handler.Write()->CommitWrite();

  handler.Write()->DeleteDirectory("test");
  handler.Read()->Traverse(false);

  handler.Write()->CommitWrite();

  handler.Read()->Traverse(false);
  handler.Read()->Sort(fmr::compare::Natural());

  {
    bool found = false;
    for (const auto &it : *handler.Read()) {
      if (it.GetName() == "test" && it.IsDirectory()) found = true;
    }

    ASSERT_TRUE(found) << "Directory should'nt be deleted when it is not empty";
  }
}

void iterator_test(fmr::file_handler::Handler &handler) {
  for (const auto &it : *handler.Read()) it.GetName();
}

void iterator_test(fmr::file_handler::local::Handler &handler) {
  for (const auto &it : *handler.Read()) it.GetName();
  iterator_test(static_cast<fmr::file_handler::Handler &>(handler));
}

TYPED_TEST(HandlerFixture, TestIterator) {
  for (const auto &it : *this->handler_.Read()) it.GetName();
  for (auto &it : *this->handler_.Read()) it.GetName();
  iterator_test(this->handler_);
}

int main(int argc, char **argv) {
  auto factory = std::make_unique<fmr::file_handler::Factory>();
  fmr::file_handler::InitDefaultFactory(*factory);
  fmr::file_handler::Factory::SetGlobal(factory.get());
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
