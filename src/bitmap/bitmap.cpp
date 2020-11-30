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

SBitmap G_BITMAP;

void Bitmap::Clear()
{
    for ( auto& it : Get() )
    {
        it = &G_BITMAP;
    }
    m_posFirst = 0;
    GetAll().clear();
    Refresh();
}

void Bitmap::SetLimit( size_t limit )
{
    Get().resize( limit );
    m_posLast = ( limit < m_limit ) ? 
        m_posLast - ( m_limit - limit ) : m_posLast;
    m_limit = limit;
    Refresh();
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
}

bool Bitmap::NextPage()
{
    return ChangePage( 1 );
}

bool Bitmap::PrevPage()
{
    return ChangePage( -1 );
}

bool Bitmap::IsImageOk( int pos )
{
    return Vector::IsExist(m_item,pos) && m_item.at(pos).IsOk(); 
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
        pos++;

    Refresh();
    if ( i == 0 )
        return false;
    return temp != pos;
}

const SBitmap* Bitmap::Get( const wxPoint& area, const wxPoint& position ) const
{
    for ( const auto& it : Get() )
    {
        if ( it->IsPointed( area, position ) ) return it;
    }
    return nullptr;
}

SBitmap* Bitmap::Get( const wxPoint& area, const wxPoint& position )
{
    for ( auto& it : Get() )
    {
        if ( it->IsPointed( area, position ) ) return it;
    }
    return nullptr;
}

wxVector<const SBitmap*> Bitmap::Get( const wxPoint& area, const wxSize& size ) const
{
    wxVector<const SBitmap*> bmp;
    int i = 0;
    for ( const auto& it : Get() )
    {
        if ( it->IsShown( area, size ) )
            bmp.push_back(it);

        i++;
    }

    return bmp;
}

wxVector<SBitmap*> Bitmap::Get( const wxPoint& area, const wxSize& size )
{
    int i = 0;
    wxVector<SBitmap*> bmp;
    for ( auto& it : Get() )
    {
        // skip not Ok Image;
        if ( !it->IsOk() ) continue;
        
        bmp.push_back(it);
        i++;
    }
    return bmp;
}
