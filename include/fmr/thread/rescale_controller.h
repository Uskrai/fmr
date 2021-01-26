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

#ifndef FMR_THREAD_RESCALE_CONTROLLER
#define FMR_THREAD_RESCALE_CONTROLLER

#include "fmr/thread/rescale.h"

namespace fmr {

namespace thread {

class RescaleController : public ThreadController {
 protected:
  Rescale *thread_ = nullptr;
  int thread_id_;
  bitmap::Rescaler *rescaler_ = nullptr;
  wxEvtHandler *parent_;
  wxCriticalSection lock_;

 public:
  RescaleController(wxEvtHandler *parent, int id) : ThreadController() {
    thread_id_ = id;
  }

  virtual ~RescaleController() { Clear(); }

  void SetRescaller(bitmap::Rescaler *rescaler) { rescaler_ = rescaler; }

  void SetThreadId(int id) {
    Unbind(kEventImageRescaled, &RescaleController::OnImageRescalled, this,
           thread_id_);
    thread_id_ = id;
    Bind(kEventImageRescaled, &RescaleController::OnImageRescalled, this,
         thread_id_);
  }

  void OnImageRescalled(RescaledEvent &event) {
    wxPostEvent(GetParent(), event);
  }

  void Push(wxImage *image) {
    if (!thread_) Run();
    thread_->Push(image);
  };

  bool Run() {
    thread_ = new Rescale(this, wxTHREAD_DETACHED, thread_id_);
    thread_->SetRescaler(rescaler_);
    return thread_->Run() == wxTHREAD_NO_ERROR;
  }

  int GetThreadId() { return thread_id_; }

  void DoSetNull(int id) {
    if (id == GetThreadId()) thread_ = nullptr;
  }

  BaseThread *GetThread(int id) {
    if (id == GetThreadId()) return thread_;
    return nullptr;
  }

  wxEvtHandler *GetParent() { return parent_; }

  void Clear() { DeleteThread(GetThreadId(), lock_); }
};

}  // namespace thread

}  // namespace fmr

#endif /* end of include guard: FMR_THREAD_RESCALE_CONTROLLER */
