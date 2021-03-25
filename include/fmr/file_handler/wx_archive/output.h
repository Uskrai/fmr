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

#ifndef FMR_FILE_HANDLER_WX_ARCHIVE_OUTPUT
#define FMR_FILE_HANDLER_WX_ARCHIVE_OUTPUT

#include <fmr/file_handler/factory.h>
#include <fmr/file_handler/output.h>
#include <fmr/file_handler/read_stream.h>
#include <fmr/file_handler/wx_archive/write_stream.h>
#include <wx/archive.h>

namespace fmr {

namespace file_handler {

namespace wx_archive {

class Input;
class Handler;

class Output : public file_handler::Output {
  Input *input_ = nullptr;
  Handler *handler_ = nullptr;
  file_handler::Handler *handler_parent_ = nullptr;
  const wxArchiveClassFactory *archive_factory_ = nullptr;
  const Factory *handler_factory_ = nullptr;

  std::vector<WriteStream> vec_;
  std::shared_ptr<file_handler::ReadStream> stream_;

 public:
  Output(Handler *handler, Input *input);
  void Open(std::shared_ptr<file_handler::ReadStream> stream,
            const wxArchiveClassFactory *arch_factory,
            file_handler::Handler *handler_parent_);

  bool IsOk() const;

  void Create();

  void CreateDirectory(const std::string &name);
  WriteStream *CreateFile(const std::string &name, Stream *stream,
                          WriteType type);

  void DeleteDirectory(const std::string &name);
  void DeleteFile(const std::string &name);

  void CommitWrite();

  void Delete();

 protected:
  void UpdateStream(const Stream &stream);
};

}  // namespace wx_archive

}  // namespace file_handler

}  // namespace fmr

#endif /* end of include guard: FMR_FILE_HANDLER_WX_ARCHIVE_OUTPUT */
