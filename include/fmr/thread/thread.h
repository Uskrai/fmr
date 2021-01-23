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

#ifndef FMR_THREAD_THREAD
#define FMR_THREAD_THREAD

#include <wx/event.h>
#include <wx/thread.h>

inline wxCriticalSection g_sLock;

namespace fmr {

wxDECLARE_EVENT(EVT_COMMAND_THREAD_UPDATE, wxThreadEvent);
wxDECLARE_EVENT(EVT_COMMAND_THREAD_COMPLETED, wxThreadEvent);

class ThreadController : public wxEvtHandler {
 public:
  ThreadController();
  ~ThreadController() {}
  virtual wxEvtHandler *GetParent() = 0;
  virtual void DoSetNull(int id) = 0;
  virtual wxThread *GetThread(int id) = 0;

  bool Delete(int thread_id, wxCriticalSection &lock);
  bool Wait(int thread_id, wxCriticalSection &lock);
  bool DeleteThread(int thread_id, wxCriticalSection &lock);

  void Update(int id);
  void Completed(int id);

  void OnUpdate(wxThreadEvent &event);
  void OnCompleted(wxThreadEvent &event);

 protected:
  void BindEvent();
};

class BaseThread : public wxThread, public wxObject {
 public:
  BaseThread(ThreadController *parent,
             const wxThreadKind type = wxTHREAD_DETACHED, int id = -1);
  ~BaseThread();

  [[deprecated("Replaced by SetEventId")]] void SetId(int id);
  void SetEventId(int id);
  int GetEventId() { return m_id; }

  ThreadController *GetParent() { return parent_; }
  void QueueEventParent(wxEvent *event);

  /**
   * @brief disable send event when TestDestroy() return true
   * this method will disable any called by QueueEventParent from thread
   * managed by this object ( so if QueueEventParent called by different thread,
   * it still will be send )
   * @param disable whether to disable the event or not
   */
  void DisableEventOnDestroy(bool disable = true);

 protected:
  void Update();
  void Completed();
  ThreadController *parent_ = NULL;
  bool disable_event_on_destroy_ = false;
  int m_id;
};

};  // namespace fmr

#endif
