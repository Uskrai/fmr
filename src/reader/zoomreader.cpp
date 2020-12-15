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

#include "reader/zoomreader.h"

#include "window/scrolledwindow.h"
#include "handler/handler.h"
#include "bitmap/bitmap.h"
#include "wx/log.h"

void ZoomThread::SetParameter( std::shared_ptr<Handler> handler, std::shared_ptr<Bitmap> bitmap, float scale )
{
    m_handler = handler;
    m_bitmap = bitmap;
    m_scale = scale;
}

ZoomThread::~ZoomThread()
{
    wxCriticalSectionLocker locker(g_sLock);
    m_parent->DoSetNull( m_id );
}

void ZoomThread::Zoom( SBitmap *bmp, wxImage &img, float  scaleadd, std::function<bool()> f )
{
    if ( img.IsOk() && f())
    {
        wxCriticalSectionLocker locker( g_sLock );
        wxSize size = img.GetSize();
        float scale = bmp->GetScale() + scaleadd;
        size.Scale( scale , scale );
        if ( img.IsOk() && img.GetSize() != size && f())
        {
            img.Rescale( size.GetWidth(), size.GetHeight() );
            if ( img.IsOk() && img.GetSize() != bmp->GetSize() & f() )
            {
                bmp->SetBitmap( wxBitmap(img) );
                bmp->SetScale( scale );
            }
        }
    }
}

wxThread::ExitCode ZoomThread::Entry()
{
    if ( m_handler && m_bitmap )
    {
        auto TestNotDestroy = [this]() -> bool 
        {
            return !TestDestroy();
        };
        auto bitmap = m_bitmap->Get();
        for ( auto &it : bitmap )
        {
            float scale = it->GetScale() + m_scale;
            if ( m_handler->IsExist( it->GetIndex() ) && !TestDestroy() )
            {
                wxLogNull nuller;
                auto stream = m_handler->Item( it->GetIndex() );
                wxImage img( *stream );

                Zoom( it, img, m_scale, TestNotDestroy);

                if ( !TestDestroy() )
                {
                    wxSize sz = m_bitmap->GetSize( m_parent->GetClientSize() );
                    m_bitmap->RefreshPosition( sz );

                    wxQueueEvent( 
                        m_parent,
                        new wxThreadEvent( EVT_COMMAND_THREAD_UPDATE, m_id )
                    );
                }
            }
            it->SetScale( scale );
        }
    } // if m_handler and m_bitmap not NULL
    wxQueueEvent(
        m_parent,
        new wxThreadEvent( EVT_COMMAND_THREAD_COMPLETED, m_id )
    );
    return (wxThread::ExitCode)0;
}