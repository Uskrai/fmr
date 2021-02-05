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

#include "fmr/thread/load_image_controller.h"

namespace fmr {
namespace thread {

LoadImageController::LoadImageController(wxEvtHandler *parent, int id)
    : ThreadController() {
  SetParent(parent);
  SetThreadId(id);
}

void LoadImageController::SetThreadId(int id) {
  Unbind(queue::kEventImageLoaded, &LoadImageController::OnImageLoaded, this,
         GetThreadId());
  thread_id_ = id;
  Bind(queue::kEventImageLoaded, &LoadImageController::OnImageLoaded, this,
       GetThreadId());
}

void LoadImageController::OnImageLoaded(queue::LoadImageEvent &event) {
  //
}

void LoadImageController::Push(SStream *stream) {
  if (!thread_) Run();
  thread_->GetQueue().Push(stream);
}

bool LoadImageController::Run() {
  auto thread_temp =
      new Queue<queue::LoadImage>(this, wxTHREAD_DETACHED, GetThreadId());
  bool ret = thread_temp->Run() == wxTHREAD_NO_ERROR;
  thread_ = thread_temp;

  return ret;
}

void LoadImageController::Clear() { DeleteThread(thread_, lock_); }

void LoadImageController::DisableOnEmptyQueue(bool disable) {
  if (thread_) thread_->DisableOnEmptyQueue(disable);
}

void LoadImageController::DoSetNull(BaseThread *thread) {
  if (thread == thread_) thread_ = nullptr;
}

}  // namespace thread
}  // namespace fmr
