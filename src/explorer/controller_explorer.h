/*
 *  Copyright (c) 2020 Uskrai
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

#ifndef FMR_EXPLORER_CONTROLLER_EXPLORER
#define FMR_EXPLORER_CONTROLLER_EXPLORER

#include "explorer/common.h"
#include "thread/thread.h"
#include "explorer/find_explorer.h"
#include "explorer/load_explorer.h"
#include "handler/struct_stream.h"
#include <wx/window.h>
#include <memory>

namespace fmr
{

namespace explorer
{

enum ThreadID
{
    kFindThreadID = wxID_HIGHEST + 40,
    kLoadThreadID
};


class Controller
    : public ThreadController
{
    public:
        Controller( wxWindow *parent );
        ~Controller();
        void SetParameter( std::vector<StreamBitmap> &list_stream );
        void SetThumbSize( const wxSize &size );

        void Load();

        wxEvtHandler *GetParent() { return parent_; }
        wxThread *GetThread( int id );
        void DoSetNull( int id );

        void Clear();

    private:
        wxWindow *parent_;
        std::vector<StreamBitmap> list_stream_;
        FindThread *find_thread_ = NULL;
        LoadThread *load_thread_ = NULL;
        wxSize thumb_size_;

        void OnFound( StreamBitmapEvent &event );
        void OnUpdate( wxCommandEvent &event );
        void OnFindCompleted( wxCommandEvent &event );
        void OnLoadCompleted( wxCommandEvent &event );
        wxDECLARE_EVENT_TABLE();
};

}; // namespace explorer

}; // namespace fmr

#endif