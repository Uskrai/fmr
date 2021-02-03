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

#ifndef FMR_EXPLORER_LOAD_EXPLORER
#define FMR_EXPLORER_LOAD_EXPLORER

#include <fmr/bitmap/rescaler.h>
#include <fmr/handler/struct_stream.h>
#include <fmr/thread/thread.h>
#include <wx/event.h>

#include <queue>

#include "fmr/bitmap/bmp.h"
#include "fmr/thread/queue.h"

namespace fmr {

namespace thread {

class LoadImageEvent : public wxCommandEvent {
 protected:
  SBitmap bitmap_;
  const SStream *stream_;

 public:
  LoadImageEvent(wxEventType type, int id) : wxCommandEvent(type, id){};
  LoadImageEvent(const LoadImageEvent &event) : wxCommandEvent(event) {
    bitmap_ = event.bitmap_;
    stream_ = event.stream_;
  }

  void SetBitmap(const SBitmap &bitmap) { bitmap_ = bitmap; }
  void SetStream(const SStream *stream) { stream_ = stream; }

  SBitmap &GetBitmap() { return bitmap_; }
  const SStream *GetStream() { return stream_; }
};

wxDECLARE_EVENT(kEventImageLoaded, LoadImageEvent);

class LoadImage : public Queue<SStream *> {
 public:
  LoadImage(ThreadController *parent, wxThreadKind kind = wxTHREAD_DETACHED,
            int id = wxID_ANY)
      : Queue(parent, kind, id){};

  ExitCode Entry();
  void Load(SStream *stream);

  void SetSize(const wxSize &size);
  void SetImageQuality(wxImageResizeQuality quality);
  void Clear();

  void DeleteOnEmptyQueue(bool condition = true);

 protected:
  std::queue<SStream *> load_queue_;

  bool is_delete_on_empty_ = false;

  bitmap::Rescaler image_rescaler_;
  wxSize image_size_;
  wxImageResizeQuality image_quality_ = wxIMAGE_QUALITY_NORMAL;
};

};  // namespace thread
};  // namespace fmr

#endif
