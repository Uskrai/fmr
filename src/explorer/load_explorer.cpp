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

#include "explorer/load_explorer.h"
#include <wx/image.h>
#include <wx/filename.h>

namespace fmr
{

namespace explorer
{

void LoadThread::SetParameter( std::vector<SStream> &list_stream )
{
    for ( auto &it : list_stream )
        list_stream_.push_back( &it );
}

wxThread::ExitCode LoadThread::Entry()
{
    for ( auto &it : list_stream_  )
    {
        if ( wxFileName::FileExists( it->GetName() ) && wxImage::CanRead(it->GetName()) )
            it->Open( it->GetName() );
        else
            Find( it, it->GetName() );
        
        Update();
    }
    Completed();
    return (wxThread::ExitCode)0;
}

bool LoadThread::Find( SStream *target_stream, const wxString &folder )
{
    AbstractHandler *handler = HandlerFactory::NewHandler( folder );
    handler->Traverse( true );
    return Find( target_stream, handler->GetChild() );
}

bool LoadThread::Find( SStream *target_stream, std::vector<SStream> &list_stream )
{
    for ( auto &it : list_stream )
    {
        if ( it.IsOk() )
            target_stream->Open( it.GetOutputStream() );

        else if ( wxFileName::Exists( it.GetName() ) )
        {
            if ( wxFileName::DirExists( it.GetName() ) )
                return Find( target_stream, it.GetName() );
        }    
    }
    return false;
}


}; // namespace explorer

}; // namespace fmr