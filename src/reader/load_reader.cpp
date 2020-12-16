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

#include "reader/load_reader.h"

#include <wx/log.h>

namespace fmr
{

namespace reader
{

void LoadThread::SetParameter( std::shared_ptr<AbstractHandler> handler, std::shared_ptr<Bitmap> bitmap, size_t start )
{
    m_bitmap = bitmap;
    m_fHandler = handler;
    m_start = start;
}

void LoadThread::SetLimit( size_t prev, size_t next )
{
}

wxThreadError LoadThread::Run()
{
    // prevent thread from running if
    // m_bitmap or m_fHandler is not set
    if ( m_bitmap == NULL ||  m_fHandler == NULL )
        return wxTHREAD_MISC_ERROR;

    return wxThread::Run();
}

void LoadThread::CheckAndLoadImage( size_t& idx, int step )
{
    if ( m_fHandler->IsExist(idx) )
    {
        SStream stream;
        stream = m_fHandler->Item(idx);
        if ( !TestDestroy() )
            LoadImage( m_bitmap, stream, idx );
        idx += step;
        if ( !TestDestroy() )
        {
            wxQueueEvent( 
                m_parent,
                new wxThreadEvent(
                    EVT_COMMAND_THREAD_UPDATE,
                    m_id
                )
            );
        }
    }
}

void LoadThread::LoadImage( std::shared_ptr<Bitmap> bmp, wxInputStream &stream, size_t idx  )
{
    if ( Vector::IsExist( bmp->GetAll(), idx) )
    {
        if ( bmp->GetAll().at(idx).IsLoaded() )
            return;

        if ( wxImage::CanRead(stream) )
        {
            wxLogNull nuller;
            wxImage img(stream);
            float scale = bmp->Prepare( img );

            // wxCriticalSectionLocker locker( g_sLock );
            bmp->Add(img,idx,scale);
        }

    }
}

void LoadThread::LoadImage( std::shared_ptr<Bitmap> bmp, SStream &stream, size_t idx )
{
    if ( stream.IsOk() )
    {
        auto instream = stream.GetStream();
        LoadImage( bmp, *instream, idx );
    }
    else bmp->MarkLoaded(idx);
}

wxThread::ExitCode LoadThread::Entry()
{
    size_t next = m_start, prev = next - 1;
    while( !TestDestroy() && ( m_fHandler->IsExist(next) ||  m_fHandler->IsExist(prev)  ) )
    {
        CheckAndLoadImage( next, 1 );
        CheckAndLoadImage( prev, -1 );
    }
    if ( !TestDestroy() )
        wxQueueEvent( m_parent, new wxThreadEvent( EVT_COMMAND_THREAD_COMPLETED, m_id ) );
    return (wxThread::ExitCode)0;
}

}; // namespace reader

}; // namespace fmr