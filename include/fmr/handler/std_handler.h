/*
 *  Copyright (c) 2020-2021 Uskrai
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

#ifndef FMR_HANDLER_STD_HANDLER
#define FMR_HANDLER_STD_HANDLER

#include <fmr/common/vector.h>
#include <fmr/handler/abstract_openable_handler.h>
#include <fmr/handler/struct_stream.h>
#include <fmr/nowide/filesystem.h>
#include <wx/dir.h>
#include <wx/filename.h>
#include <wx/mstream.h>

#include <filesystem>
#include <memory>

namespace fmr {

class STDHandler : public AbstractOpenableHandler {
 public:
  STDHandler(){};
  STDHandler(STDHandler &&move);
  STDHandler(const std::string &path);
  ~STDHandler() {}

  const std::shared_ptr<AbstractHandler> GetParent() const;
  std::shared_ptr<AbstractHandler> GetParent();

  const std::vector<struct SStream> &GetChild() const;
  std::vector<struct SStream> &GetChild();

  const std::string &GetName() const;

  std::string GetFromCurrent(int step) const;
  std::string GetNext() const;
  std::string GetPrev() const;

  void Open(const std::string &path);
  bool IsOpened() const;

  void Traverse(bool GetStream = false, DirGetFlags flags = kDirDefault);

  bool IsExist(size_t idx) const;

  std::string GetItemPath(const SStream &stream) const;
  std::string GetItemPath(size_t idx) const;
  size_t Index(const std::string &name) const;
  const struct SStream &Item(size_t idx) const;
  struct SStream &Item(size_t idx);

  size_t Size() const;
  void Clear();
  void Reset();
  void Close();

  static bool CanHandle(const std::string &path);
  static bool StreamOpenable();
  bool IsStreamOpenable() const;

  bool GetStream(SStream &stream);
  bool GetFirst(SStream &stream, DirGetFlags flags = kDirDefault,
                bool is_get_stream = false);
  bool GetNextStream(SStream &stream, bool is_get_stream = false);

  bool CreateDirectory(const std::string directory_name,
                       bool overwrite = false);
  bool CreateFiles(SStream stream, const std::string &filename,
                   bool overwrite = false);
  bool CreateDirectories();
  bool CreateDirectories(const std::string &path);
  bool RemoveAll();
  bool RemoveAll(const std::string &path);
  bool Remove(const std::string &name, bool recursive);
  bool CommitWrite();

 private:
  void TraverseStream();

  std::unique_ptr<nwd::fs::directory_iterator> iterator_item_;
  DirGetFlags iterator_flags_;
  nwd::fs::directory_iterator iterator_;

  std::string name_, filename_;
  bool is_opened_ = false;

  std::vector<struct SStream> list_stream_, list_write_stream_;
  std::shared_ptr<STDHandler> parent_ = NULL;

  bool OpenStream(const std::string &name, SStream &stream,
                  bool is_get_stream = false);
};

};  // namespace fmr

#endif
