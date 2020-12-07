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

ZoomThread::ZoomThread( ScrolledWindow *parent, const wxThreadKind type, int id )
    : wxThread(type)
{
    m_parent = parent;
    m_id = id;
}

void ZoomThread::SetParameter( Handler *handler, Bitmap *bitmap, float scale )
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
        printf("zoom locked\n");
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
        printf("zoom unlocked\n");
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
                auto stream = m_handler->Item( it->GetIndex() );
                wxImage img( *stream );

                Zoom( it, img, m_scale, TestNotDestroy);

                if ( !TestDestroy() )
                {
                    m_bitmap->RefreshSize();
                    m_bitmap->RefreshPosition();

                    wxQueueEvent( 
                        m_parent,
                        new wxThreadEvent( EVT_COMMAND_THREAD_UPDATE )
                    );
                }
            }
            it->SetScale( scale );
        }
    } // if m_handler and m_bitmap not NULL
    return (wxThread::ExitCode)0;
}