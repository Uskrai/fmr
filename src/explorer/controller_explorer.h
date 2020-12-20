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

#include "thread/thread.h"
#include "explorer/load_explorer.h"
#include "handler/abstract_handler.h"
#include <wx/window.h>
#include <memory>

namespace fmr
{

namespace explorer
{

enum ThreadID
{
    kLoadThreadID = wxID_HIGHEST + 40
};


class Controller
    : ThreadController
{
    public:
        Controller( wxWindow *parent )
        { parent_ = parent; }
        ~Controller();
        void SetHandler( std::shared_ptr<AbstractHandler> handler );

        void Load();

        wxEvtHandler *GetParent() { return parent_; }
        wxThread *GetThread( int id );
        void DoSetNull( int id );
    
    private:
        wxWindow *parent_;
        std::shared_ptr<AbstractHandler> handler_;
        LoadThread *load_thread_;
};

}; // namespace explorer

}; // namespace fmr

#endif