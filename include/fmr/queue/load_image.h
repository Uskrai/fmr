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

#include <fmr/handler/struct_stream.h>
#include <fmr/thread/thread.h>
#include <wx/event.h>

#include <queue>

#include "fmr/bitmap/bmp.h"
#include "fmr/queue/base.h"

namespace fmr {

namespace queue {

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

class LoadImage : public Base<SStream *> {
 public:
  LoadImage(ThreadController *parent, int id = wxID_ANY) : Base(parent, id){};

  void PopTask();
  void Load(SStream *stream);
};

};  // namespace queue

};  // namespace fmr

#endif
