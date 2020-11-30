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

#include "bitmap/bitmapvertical.h"

#include <wx/scrolwin.h>

BitmapVertical::BitmapVertical( wxScrolledWindow* parent )
{
    this->m_parent = parent;
    m_maxWidth = parent->GetClientSize().GetWidth();
}

BitmapVertical::~BitmapVertical()
{

}

void BitmapVertical::Prepare( const wxImage& image, int pos, struct SBitmap& bmp )
{
    bmp.SetBitmap( wxBitmap(image) );
}

void BitmapVertical::Exit( int idx, bool isScroll )
{
    wxSize size = GetParent()->GetVirtualSize();
    RefreshSize();
    if ( isScroll )
    {
        wxSize afsize = GetParent()->GetVirtualSize(); 
        if ( size != afsize )
        {
            int y = afsize.GetHeight() - size.GetHeight();
            GetParent()->Scroll( -1, GetParent()->GetViewStart().y + y );
        }
    }

    GetParent()->Refresh();
}

void BitmapVertical::Add( const wxImage& image, int idx, bool isScroll )
{
    struct SBitmap& bmp = m_item.at(idx);
    bmp.SetBitmap( wxBitmap( image ) );
    
    m_maxWidth = ( bmp.GetWidth() > m_maxWidth ) ? bmp.GetWidth() : m_maxWidth;
    
    RefreshPosition();
    
    Exit( idx, isScroll );
}

int BitmapVertical::Centered( int width )
{
    int clientWidth = GetParent()->GetClientSize().GetWidth();

    width = ( width < m_maxWidth ) ? width : m_maxWidth;

    int pos = (clientWidth-width)/2;
    if ( pos < 0 )
        pos = 0;
    
    return pos;
}

void BitmapVertical::RefreshPosition()
{
    int y = 0, tempheight = 0;
    for ( auto& it : Get() )
    {
        if ( it->IsOk() )
        {
            it->SetY( y + tempheight );
            y = it->GetY();
            tempheight = it->GetHeight();            
            it->SetX( Centered( it->GetWidth() ) );
        }
    }
}

void BitmapVertical::RefreshSize()
{
    wxSize size;
    for ( const auto& it : Get() )
    {
        if ( it->IsOk() )
        {
            size.SetHeight( it->GetHeight() + size.GetHeight() );
            size.SetWidth( size.GetWidth() > it->GetWidth() ? size.GetWidth() : it->GetWidth() );
        }
    }
    GetParent()->SetVirtualSize( size );
    GetParent()->Refresh();
}