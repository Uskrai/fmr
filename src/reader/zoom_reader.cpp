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

#include <fmr/reader/zoom_reader.h>
#include <fmr/bitmap/image_util.h>

#include "wx/log.h"

namespace fmr
{

namespace reader
{

void ZoomThread::SetParameter( std::shared_ptr<AbstractHandler> handler, std::shared_ptr<Bitmap> bitmap, float scale )
{
    m_handler = handler;
    m_bitmap = bitmap;
    m_scale = scale;
}

wxThread::ExitCode ZoomThread::Entry()
{
    if ( m_handler && m_bitmap )
    {
        auto bitmap = m_bitmap->Get();
        for ( auto &it : bitmap )
        {
            float scale = it->GetScale() + m_scale;
            if ( m_handler->IsExist( it->GetIndex() ) && !TestDestroy() )
            {
                wxLogNull nuller;
                auto stream = m_handler->Item( it->GetIndex() );

                wxImage img;
                image_util::Load( img, stream );
                image_util::Rescale( *it, img, it->GetScale() );
                Update();
            }
            it->SetScale( scale );
        }
    } // if m_handler and m_bitmap not NULL

    if ( !TestDestroy() )
        Completed();

    return (wxThread::ExitCode)0;
}

}; // namespace fmr

}; // namespace fmr
