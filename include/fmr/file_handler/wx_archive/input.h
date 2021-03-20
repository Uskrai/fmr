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

#ifndef FMR_FILE_HANDLER_WX_ARCHIVE_INPUT
#define FMR_FILE_HANDLER_WX_ARCHIVE_INPUT

#include <fmr/file_handler/input.h>
#include <fmr/file_handler/wx_archive/read_stream.h>
#include <wx/archive.h>

namespace fmr {

namespace file_handler {

namespace wx_archive {

using ReadStreamBase = file_handler::ReadStream;
class Input : public file_handler::Input {
  std::string path_;
  std::weak_ptr<file_handler::ReadStream> stream_;
  const wxArchiveClassFactory *factory_ = nullptr;

  std::unique_ptr<wxArchiveInputStream> arch_stream_;

  std::vector<ReadStream> vec_;

 public:
  void Open(std::weak_ptr<file_handler::ReadStream> stream);

  virtual std::string GetPath() const override { return path_; }

  virtual bool IsOpened() const override;

  virtual ReadStream *GetFirst(bool load_buffer) override;
  virtual ReadStream *GetNext(bool load_buffer) override;

  virtual bool IsEmpty() const override;

  virtual size_t Size() const override;

  virtual void GetChild(std::vector<ReadStreamBase *> &vec) override;
  virtual void GetChild(
      std::vector<const ReadStreamBase *> &vec) const override;

  void GetChild(std::vector<ReadStream *> &vec);
  void GetChild(std::vector<const ReadStream *> &vec) const;

  virtual void Clear() override;
};

}  // namespace wx_archive

}  // namespace file_handler

}  // namespace fmr

#endif /* end of include guard: FMR_FILE_HANDLER_WX_ARCHIVE_INPUT */
