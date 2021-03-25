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
#include <fmr/file_handler/input_implementer_helper.h>
#include <fmr/file_handler/wx_archive/read_stream.h>
#include <wx/archive.h>

namespace fmr {

namespace file_handler {

namespace wx_archive {

class Output;
class Handler;

using InputBase = InputImplementHelper<ReadStream, file_handler::Input>;
class Input : public InputBase {
  std::weak_ptr<file_handler::ReadStream> stream_;
  const wxArchiveClassFactory *factory_ = nullptr;
  Handler *handler_ = nullptr;

  std::shared_ptr<file_handler::ReadStream> arch_memory_stream_;
  std::unique_ptr<wxArchiveInputStream> arch_input_stream_;

 public:
  Input(Handler *handler) { handler_ = handler; }
  void Open(std::weak_ptr<file_handler::ReadStream> stream);

  virtual bool IsOpened() const override;

  virtual ReadStream *GetFirst(bool load_buffer) override;
  virtual ReadStream *GetNext(bool load_buffer) override;

  virtual bool IsEmpty() const override;

  virtual size_t Index(const std::string &path) const override;

  virtual void Clear() override;

  void UpdateStream(std::weak_ptr<file_handler::ReadStream> stream);

 protected:
};

}  // namespace wx_archive

}  // namespace file_handler

}  // namespace fmr

#endif /* end of include guard: FMR_FILE_HANDLER_WX_ARCHIVE_INPUT */
