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

wxVector<SBitmap>& BitmapVertical::Get() { return m_item; }

void BitmapVertical::Prepare( const wxImage& image, VectorPos pos, struct SBitmap& bmp )
{
    bmp.SetBitmap( wxBitmap(image) );
    if ( bmp.GetWidth() > m_maxWidth )
        m_maxWidth = bmp.GetWidth();
}

#include <iostream>
void BitmapVertical::Exit( int idx )
{
    if ( idx < 0 ) return;


    struct SBitmap& bmp = Get().back();
    int h = bmp.GetY() + bmp.GetHeight();

    GetParent()->SetVirtualSize( -1, h );
    GetParent()->Refresh();
}

void BitmapVertical::Add( const wxImage& image, VectorPos idx )
{
    struct SBitmap bmp;
    Prepare( image, idx, bmp );
    
    if ( bmp.GetWidth() > m_maxWidth )
    {
        m_maxWidth = bmp.GetWidth();
    }
    
    int i;
    wxPoint pos;
    switch ( idx )
    {
        case VECTOR_END:
            i = Push( bmp, pos );
            break;
        default:
            i = Insert( bmp, pos, idx );
            RefreshPosition();
    }
    
    Exit( i );
    GetParent()->GetClientSize();
    GetParent()->Refresh();
}

int BitmapVertical::Push( struct SBitmap& bmp, wxPoint& pos )
{
    Get().push_back(bmp);
    CalcPosition( Get().size() -1, pos );
    Get().back().SetPosition(pos);
    return Get().size() -1;
}

int BitmapVertical::Insert( struct SBitmap& bmp, wxPoint& pos, VectorPos idx)
{
    if ( idx == VECTOR_BEGIN )
    {
        Get().insert( Get().begin(), bmp );
        return 0;
    }
    else
    {
        int i = 0;
        for ( auto it = Get().begin(); it != Get().end(); ++it )
        {
            if ( i == idx )
            {
                Get().insert( it, bmp );
                return i;
            }
        }
    }
    return -1;
}

void BitmapVertical::CalcPosition( int idx, wxPoint& pos )
{
    if ( idx == 0 )
        pos.y = 0;
    else
    {
        struct SBitmap& bmp = Get().at( idx - 1 );
        pos.y = bmp.GetY() + bmp.GetHeight();
    }
    pos.x = Centered( Get().at(idx).GetWidth() ) ;
}

int BitmapVertical::Centered( int width )
{
    int clientWidth = GetParent()->GetClientSize().GetWidth();
    if ( width > m_maxWidth ) 
        width = m_maxWidth;
    
    int pos = (clientWidth-width)/2;
    if ( pos < 0 )
        pos = 0;
    
    return pos;
}

void BitmapVertical::RefreshPosition()
{
    wxPoint pos;
    for ( size_t i = 0; i < Get().size(); i++ )
    {
        CalcPosition( i, pos );
        Get().at(i).SetPosition( pos );
    }
}

void BitmapVertical::Clear()
{
    Get().clear();
}