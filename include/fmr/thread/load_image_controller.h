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

#ifndef FMR_THREAD_LOAD_IMAGE_CONTROLLER
#define FMR_THREAD_LOAD_IMAGE_CONTROLLER

#include "fmr/handler/struct_stream.h"
#include "fmr/thread/load_image.h"
#include "fmr/thread/thread.h"

namespace fmr {
namespace thread {

constexpr int kLoadImageControllerIdDefault = wxID_HIGHEST + 4000;

class LoadImageController : public ThreadController {
 protected:
  LoadImage *thread_ = nullptr;

  int thread_id_ = kLoadImageControllerIdDefault;
  wxEvtHandler *parent_ = nullptr;
  wxCriticalSection lock_;

 public:
  LoadImageController(wxEvtHandler *parent,
                      int thread_id = kLoadImageControllerIdDefault);
  void Push(SStream *stream);
  bool Run();

  virtual void SetParent(wxEvtHandler *parent) { parent_ = parent; }
  virtual wxEvtHandler *GetParent() { return parent_; }

  void DoSetNull(int id);
  BaseThread *GetThread(int id);

  void SetThreadId(int id);
  int GetThreadId() const { return thread_id_; }

  void Clear();

 private:
  void OnImageLoaded(LoadImageEvent &event);
};

}  // namespace thread
}  // namespace fmr

#endif /* end of include guard: FMR_THREAD_LOAD_IMAGE_CONTROLLER */
