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

#include <fmr/reader/load_reader.h>
#include <fmr/bitmap/image_util.h>

#include <wx/log.h>

namespace fmr
{

namespace reader
{

void LoadThread::SetParameter( std::shared_ptr<AbstractHandler> handler, std::shared_ptr<Bitmap> bitmap, size_t start )
{
    SetBitmap( bitmap );
    SetHandler( handler );
    SetStart( start );
}

void LoadThread::SetHandler( std::shared_ptr<AbstractHandler> handler )
{
    file_handler_ = handler;
}

void LoadThread::SetBitmap( std::shared_ptr<Bitmap> bitmap )
{
    bitmap_ = bitmap;
}

void LoadThread::SetStart( size_t start )
{
    load_start_ = start;
}

void LoadThread::SetLimit( size_t prev, size_t next )
{
}

wxThreadError LoadThread::Run()
{
    // prevent thread from running if
    // m_bitmap or m_fHandler is not set
    if ( GetBitmap() == NULL ||  GetHandler() == NULL )
        return wxTHREAD_MISC_ERROR;

    return wxThread::Run();
}

void LoadThread::LoadImage( size_t& idx, int step )
{
    LoadImage( GetBitmap(), GetHandler(), idx, g_sLock );
    Update();
    idx += step;
}

void LoadThread::LoadImage( std::shared_ptr<Bitmap> bmp, std::shared_ptr<AbstractHandler> handler, size_t idx, wxCriticalSection &lock )
{
    if ( !bmp && !handler )
        return;

    if ( ! Vector::IsExist( bmp->GetAll(), idx  ) )
        return;

    if ( bmp->GetAll().at( idx ).IsLoaded() )
        return;

    wxImage img;
    if ( ! image_util::Load( img, *handler, idx ) )
        return bmp->MarkLoaded( idx );

    wxCriticalSectionLocker locker(lock);
    float scale = bmp->Prepare( img );
    bmp->Add( img, idx, scale );
}

wxThread::ExitCode LoadThread::Entry()
{
    size_t next = load_start_, prev = next - 1;
    while( !TestDestroy() && ( GetHandler()->IsExist(next) ||  GetHandler()->IsExist(prev)  ) )
    {
        LoadImage( next, 1 );
        LoadImage( prev, -1 );
        if ( TestDestroy() )
            break;
    }

    if ( !TestDestroy())
        Completed();
    return (wxThread::ExitCode)0;
}

}; // namespace reader

}; // namespace fmr
