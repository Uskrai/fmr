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

#include <wx/window.h>
#include "image/image.h"
#include "bitmap/bitmap.h"
#include "reader/threadreader.h"

void LoadImage( Bitmap *bmp, wxInputStream &stream, size_t idx  )
{
    if ( wxImage::CanRead(stream) )
    {
        wxImage img(stream);
        wxCriticalSectionLocker locker( Reader::LoadThreadLock );
        bmp->Add(img,idx);
    }
}

bool IsExist( Handler *handler, size_t idx )
{
    return handler->IsExist(idx);
}

namespace Reader
{

LoadThread::LoadThread( wxWindow *parent, const wxThreadKind &type ) 
    : wxThread( type )
{
    m_parent = parent;
}

LoadThread::~LoadThread()
{
}

void LoadThread::SetParameter( Bitmap *bitmap, Handler *handler, size_t start )
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

#define CheckAndLoadImage( idx, step )                  \
    if ( IsExist(m_fHandler,idx) )                      \
    {                                                   \
        wxInputStream &stream = *m_fHandler->Item(idx); \
        LoadImage(m_bitmap,stream,idx);                 \
        idx += step;                                    \
        wxQueueEvent( m_parent,                         \
            new wxThreadEvent(                          \
                EVT_COMMAND_LOADTHREAD_UPDATE           \
            ));                                         \
    }

wxThread::ExitCode LoadThread::Entry()
{
    size_t next = m_start, prev = next - 1;
    while( !TestDestroy() && ( IsExist( m_fHandler, next ) || IsExist( m_fHandler, prev ) ) )
    {
        CheckAndLoadImage( next, 1 );
        CheckAndLoadImage( prev, -1 );
    }

    return (wxThread::ExitCode)0;
}

} // namespace reader