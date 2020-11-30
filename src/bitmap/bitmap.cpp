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

#include "bitmap/bitmap.h"
#include "bitmap/position.h"
#include "bitmap/size.h"

#include <wx/scrolwin.h>

SBitmap G_BITMAP;

Bitmap::Bitmap( wxScrolledWindow* parent )
{
    this->m_parent = parent;
    m_maxWidth = parent->GetClientSize().GetWidth();
}

Bitmap::~Bitmap()
{

}

void Bitmap::Refresh()
{
    size_t i = 0;
    size_t pos = m_posFirst;

    while( Vector::IsExist( m_itemPage, i) )
    {
        if ( Vector::IsExist( m_item, pos ) )
        {
            if ( m_item.at(pos).IsOk() )
            {
                m_itemPage.at(i) = &m_item.at(pos);
                i++;
            }
            pos++;
        }
        else 
        {
            m_itemPage.at(i) = &G_BITMAP;
            i++;
        }
    }
    m_posLast = pos;
    RefreshSize();
    RefreshPosition();
    m_parent->Refresh();
}

bool Bitmap::ChangePage( int step )
{
    size_t &pos = m_posFirst;

    size_t temp = pos;
    size_t i = 0;
    while ( i < m_limit )
    {
        if ( ! Vector::IsExist( m_item, pos + step ) )
            break;

        if ( IsImageOk( pos )  )
            i++;

        pos += step;
    }
    while ( !IsImageOk( pos ) )
        pos += -(step);

    Refresh();
    if ( i == 0 )
        return false;
    return temp != pos;
}

void Bitmap::Resize( size_t limit )
{
    // use limit if m_limit is larger than limit
    limit = m_limit > limit ? limit : m_limit;
    Get().resize(limit);
};

void Bitmap::SetLimit( size_t limit )
{
    m_limit = limit;
    Refresh();
}

bool Bitmap::IsImageOk( int pos )
{
    return Vector::IsExist(m_item,pos) && m_item.at(pos).IsOk(); 
}

void Bitmap::Prepare( const wxImage& image, int pos, struct SBitmap& bmp )
{
    bmp.SetBitmap( wxBitmap(image) );
}

void Bitmap::Add( wxImage& image, int idx )
{
    Size::Prepare( image, m_flagSize, m_parent, m_scaleParent );
    struct SBitmap& bmp = m_item.at(idx);
    bmp.SetBitmap( wxBitmap( image ) );
    
    m_maxWidth = ( bmp.GetWidth() > m_maxWidth ) ? bmp.GetWidth() : m_maxWidth;
    
    Refresh();
    GetParent()->Refresh();
}

void Bitmap::Exit( int idx )
{
    // wxSize size = GetParent(->GetVirtualSize();
    RefreshSize();
    // if ( isScroll )
    // {
        // wxSize afsize = GetParent()->GetVirtualSize(); 
        // if ( size != afsize )
        // {
            // int y = afsize.GetHeight() - size.GetHeight();
            // GetParent()->Scroll( -1, GetParent()->GetViewStart().y + y );
        // }
    // }

    GetParent()->Refresh();
}


int Bitmap::Centered( int width )
{
    int clientWidth = GetParent()->GetClientSize().GetWidth();

    width = ( width < m_maxWidth ) ? width : m_maxWidth;

    int pos = (clientWidth-width)/2;
    if ( pos < 0 )
        pos = 0;
    
    return pos;
}

void Bitmap::RefreshPosition()
{
    Position::Refresh( Get(), m_flagPosition, m_parent );
    // int y = 0, tempheight = 0;
    // for ( auto& it : Get() )
    // {
    //     if ( it->IsOk() )
    //     {
    //         it->SetY( y + tempheight );
    //         y = it->GetY();
    //         tempheight = it->GetHeight();            
    //         it->SetX( Centered( it->GetWidth() ) );
    //     }
    // }
}

void Bitmap::RefreshSize()
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

void Bitmap::Clear()
{
    for ( auto& it : Get() )
    {
        it = &G_BITMAP;
    }
    m_posFirst = 0;
    GetAll().clear();
}