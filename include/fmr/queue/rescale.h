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

#ifndef FMR_THREAD_RESCALE
#define FMR_THREAD_RESCALE

#include <queue>

#include "fmr/bitmap/rescaler.h"
#include "fmr/queue/base.h"

namespace fmr {

namespace queue {

class RescaledEvent : public wxCommandEvent {
 protected:
  wxImage *image_ = nullptr;

 public:
  RescaledEvent(wxEventType type, int id, wxImage *image = nullptr)
      : wxCommandEvent(type, id) {
    SetImage(image);
  }

  void SetImage(wxImage *image) { image_ = image; };

  wxImage *GetImage() { return image_; }
};

wxDECLARE_EVENT(kEventImageRescaled, RescaledEvent);

class Rescale : public Base<wxImage *> {
 private:
  bitmap::Rescaler *rescaler_ = nullptr;

 public:
  Rescale(wxEvtHandler *parent, int id = wxID_ANY) : Base(parent, id){};

  void SetRescaler(bitmap::Rescaler *rescaler) { rescaler_ = rescaler; }

  bool ProcessTask(value_type &item);

 private:
  void SendEvent(wxImage *image);
};

}  // namespace queue

}  // namespace fmr

#endif /* end of include guard: FMR_THREAD_RESCALE */
