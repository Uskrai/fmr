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

#ifndef FMR_HANDLER_STRUCT_STREAM
#define FMR_HANDLER_STRUCT_STREAM

#include <fmr/common/bitmask.h>
#include <fmr/common/compare.h>
#include <fmr/common/path.h>
#include <wx/event.h>
#include <wx/mstream.h>

#include <memory>
#include <string>

class wxInputStream;
class wxOutputStream;

namespace fmr {

class AbstractHandler;

enum StreamActionType {
  kStreamNone = 0x00,
  kStreamRead = 0x01,
  kStreamWrite = 0x02,
  kStreamOverwrite = 0x04,
  kStreamRemove = 0x08,
  kStreamRecursive = 0x10,
};

DEFINE_BITMASK_TYPE(StreamActionType)

struct SStream : public Compare::Sortable {
  SStream();
  SStream(void *data, size_t length);
  SStream(const wxString &name) = delete;
  SStream(const char *name) { Open(std::string(name)); }
  SStream(const std::string &name) { Open(name); }
  SStream(wxInputStream *stream);
  SStream(const wxOutputStream &stream);
  SStream(wxOutputStream *stream);
  SStream(const SStream &copy);
  SStream(SStream &&move);

  SStream &operator=(const SStream &copy);

  void Open(void *data = NULL, size_t length = 0);
  void Open(const wxString &name) = delete;
  void Open(const std::string &name);
  void Open(wxInputStream *stream);
  void Open(const wxMemoryOutputStream &stream);
  void Open(wxMemoryOutputStream *stream);
  void Open(std::shared_ptr<wxMemoryOutputStream> stream);

  void SetName(const wxString &name) = delete;
  void SetName(const std::wstring &name) = delete;
  void SetName(const std::string &name);

  void SetHandlerPath(wxString) = delete;
  void SetHandlerPath(const std::string &path);
  void SetDir(bool is_dir = true);
  void SetType(StreamActionType flags);

  bool IsOk() const;
  bool IsDir() const;

  std::string GetString() const { return m_name; }
  const std::string &GetName() const { return m_name; };
  const std::string &GetHandlerPath() const { return handler_path_; }
  const StreamActionType &GetType() const;
  size_t GetSize() const;
  std::shared_ptr<AbstractHandler> GetHandler();
  std::shared_ptr<wxMemoryInputStream> GetStream() const;
  std::shared_ptr<wxMemoryOutputStream> GetOutputStream();
  const std::shared_ptr<wxMemoryOutputStream> GetOutputStream() const;

  size_t CopyTo(void *buffer, size_t length) const;

  std::shared_ptr<wxMemoryOutputStream> m_stream = NULL;
  std::shared_ptr<AbstractHandler> m_handler;
  std::string m_name, handler_path_;
  StreamActionType stream_flags_ = kStreamNone;

  bool is_dir_ = false;
};

class StreamEvent : public wxCommandEvent {
 public:
  StreamEvent(wxEventType comand_type, int id = 0)
      : wxCommandEvent(comand_type, id){};
  StreamEvent(const StreamEvent &other);
  StreamEvent(StreamEvent &&other);
  using wxEvent::Clone;
  wxEvent *Clone() { return new StreamEvent(*this); }

  void SetStream(std::shared_ptr<SStream> stream);
  void SetIndex(size_t index);

  const std::shared_ptr<SStream> GetStream() const;
  std::shared_ptr<SStream> GetStream();
  size_t GetIndex();

 private:
  std::shared_ptr<SStream> stream_;
  size_t index_;
};

typedef void (wxEvtHandler::*StreamEventFunction)(StreamEvent &);

#define StreamEventHandler(func) wxEVENT_HANDLER_CAST(StreamEventFunction, func)

#define EVT_STREAM(id, type, func) \
  wx__DECLARE_EVT1(type, id, StreamEventHandler(func))

};  // namespace fmr

#endif
