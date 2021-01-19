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

#include <fmr/handler/abstract_handler.h>
#include <fmr/handler/struct_stream.h>
#include <wx/filename.h>
#include <wx/mstream.h>
#include <wx/wfstream.h>
#include <wx/wxcrtvararg.h>

namespace fmr {

char DUMMY_BUFFER;

void SStream::Open(void *data, size_t length) {
  m_stream = std::shared_ptr<wxMemoryOutputStream>(
      new wxMemoryOutputStream(data, length));
}

SStream::SStream(void *data, size_t length) { Open(data, length); }

SStream::SStream(const wxString &name) : SStream() { Open(name); }

SStream::SStream(wxInputStream *stream) : SStream() { Open(stream); }

SStream::SStream(const SStream &copy) {
  Open(copy.m_stream);
  SetName(copy.m_name);
  SetType(copy.stream_flags_);
  SetHandlerPath(copy.handler_path_);
  SetDir(copy.is_dir_);
}

SStream::SStream(SStream &&move) {
  m_stream = std::move(move.m_stream);
  m_name = std::move(move.m_name);
  stream_flags_ = std::move(move.stream_flags_);
  handler_path_ = std::move(move.handler_path_);
  is_dir_ = std::move(move.is_dir_);
}

SStream &SStream::operator=(const SStream &copy) {
  Open(copy.m_stream);
  SetName(copy.m_name);
  SetType(copy.stream_flags_);
  SetHandlerPath(copy.handler_path_);
  SetDir(copy.is_dir_);
  return *this;
}

void SStream::Open(const wxString &name) {
  Open();

  if (wxFileName::FileExists(name)) {
    wxFileInputStream stream(name);
    m_stream->Write(stream);
  }
}

void SStream::Open(wxInputStream *stream) {
  Open();

  if (stream) m_stream->Write(*stream);
}

void SStream::Open(const wxMemoryOutputStream &stream) {
  auto buffer = std::shared_ptr<char[]>(new char[stream.GetSize()]);
  size_t length;
  length = stream.CopyTo(buffer.get(), stream.GetSize());

  Open();
  m_stream->Write(buffer.get(), length);
}

void SStream::Open(std::shared_ptr<wxMemoryOutputStream> stream) {
  if (!stream) return;
  Open(*stream);
}

void SStream::Open(wxMemoryOutputStream *stream) {
  if (stream) Open(*stream);
}

void SStream::SetName(const wxString &name) { m_name = name; }

void SStream::SetName(const std::wstring &name) { SetName(wxString(name)); }

void SStream::SetName(const std::string &name) { m_name = name; }

void SStream::SetHandlerPath(const wxString &path) { handler_path_ = path; }

void SStream::SetDir(bool is_dir) { is_dir_ = is_dir; }

void SStream::SetType(StreamActionType flags) { stream_flags_ = flags; }

bool SStream::IsOk() const {
  return m_stream && m_stream->IsOk() && m_stream->GetSize() != 0;
}

bool SStream::IsDir() const { return is_dir_; }

size_t SStream::GetSize() const { return m_stream->GetSize(); }

const StreamActionType &SStream::GetType() const { return stream_flags_; }

std::shared_ptr<AbstractHandler> SStream::GetHandler() { return m_handler; }

std::shared_ptr<wxMemoryInputStream> SStream::GetStream() const {
  if (IsOk()) {
    return std::shared_ptr<wxMemoryInputStream>(
        new wxMemoryInputStream(*m_stream));
  } else {
    return std::shared_ptr<wxMemoryInputStream>(
        new wxMemoryInputStream(NULL, 0));
  }
}

std::shared_ptr<wxMemoryOutputStream> SStream::GetOutputStream() {
  return m_stream;
}

const std::shared_ptr<wxMemoryOutputStream> SStream::GetOutputStream() const {
  return m_stream;
}

size_t SStream::CopyTo(void *buffer, size_t length) const {
  return GetOutputStream()->CopyTo(buffer, length);
}

StreamEvent::StreamEvent(const StreamEvent &other)
    : wxCommandEvent(other.GetEventType(), other.GetId()) {
  SetStream(other.stream_);
  SetIndex(other.index_);
}

StreamEvent::StreamEvent(StreamEvent &&other)
    : wxCommandEvent(other.GetEventType(), other.GetId()) {
  SetStream(other.stream_);
  index_ = std::move(other.index_);
}

void StreamEvent::SetStream(std::shared_ptr<SStream> stream) {
  stream_ = stream;
}

void StreamEvent::SetIndex(size_t index) { index_ = index; }

const std::shared_ptr<SStream> StreamEvent::GetStream() const {
  return stream_;
}

std::shared_ptr<SStream> StreamEvent::GetStream() { return stream_; }

size_t StreamEvent::GetIndex() { return index_; }

};  // namespace fmr
