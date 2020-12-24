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

#include "explorer/controller_explorer.h"

namespace fmr
{

namespace explorer
{

Controller::~Controller()
{
    DeleteThread( kLoadThreadID, g_sLock );
}

void Controller::DoSetNull( int id )
{
    switch ( id )
    {
        case kLoadThreadID:
            load_thread_ = NULL;
            break;
    };
}

wxThread *Controller::GetThread( int id )
{
    switch ( id )
    {
        case kLoadThreadID:
            return load_thread_;
    }
    return NULL;
}

void Controller::SetParameter( std::vector<StreamBitmap> &list_stream )
{
    list_stream_ = list_stream;
}

void Controller::Load()
{
    if( find_thread_ )
        DeleteThread( kFindThreadID, g_sLock );

    find_thread_ = new FindThread( this, wxTHREAD_DETACHED, kFindThreadID );
    find_thread_->SetParameter( list_stream_ );
    find_thread_->Run();
}

};

}; // namespace fmr