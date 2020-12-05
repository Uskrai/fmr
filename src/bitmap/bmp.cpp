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

#include "bitmap/bmp.h"

SBitmap::SBitmap( bool isLoaded ) { m_isLoaded = isLoaded; }

const wxBitmap& SBitmap::GetBitmap() const { return this->m_item; }
wxBitmap& SBitmap::GetBitmap() { return m_item; }
bool SBitmap::IsOk() const { return m_isOk; }
bool SBitmap::IsLoaded() const { return m_isLoaded; }

bool SBitmap::IsPointed( const wxPoint& area, const wxPoint& position ) const
{
    int posY = area.y + position.y,
        posX = area.x + position.x,
        bmpPosY = GetY(), bmpAfterY = bmpPosY + GetHeight(),
        bmpPosX = GetX(), bmpAfterX = bmpPosX + GetWidth();

        return  posY >= bmpPosY && posY <= bmpAfterY 
            && posX >= bmpPosX && posX <= bmpAfterX;
}

bool SBitmap::IsShown( const wxPoint& area, const wxSize& size ) const
{
    int
    top = area.y, bottom = top + size.GetHeight(),
    left = area.x, right = left + size.GetHeight(),
    bmpPosY = GetY(), bmpAfterY = bmpPosY + GetHeight(),
    bmpPosX = GetX(), bmpAfterX = bmpPosX + GetWidth();

    return    
        (   ( bmpPosY >= top || bmpAfterY >= top )
        &&  ( bmpPosY <= bottom || bmpAfterY <= bottom )  )
        &&  ( ( bmpPosX >= left || bmpAfterY >= left )
        ||  ( bmpPosX <= right || bmpAfterX <= right ) ) ;
}

wxPoint SBitmap::GetPosition()  const { return m_pos; }
wxSize SBitmap::GetSize() const { return wxSize( GetWidth(), GetHeight() ); }

wxString SBitmap::GetName() { return m_name;}
size_t SBitmap::GetIndex() { return m_index; }
float SBitmap::GetScale() { return m_scale; }
int SBitmap::GetWidth() const { return m_item.GetWidth(); }
int SBitmap::GetHeight() const { return m_item.GetHeight(); }
int SBitmap::GetY() const { return m_pos.y; }
int SBitmap::GetX() const { return m_pos.x; }

void SBitmap::SetBitmap( const wxBitmap& bmp ) { m_item = bmp; m_isOk = true; SetLoaded(); }
void SBitmap::SetLoaded( bool stat) { m_isLoaded = stat; }
void SBitmap::SetName( const wxString& name ) { m_name = name;}
void SBitmap::SetIndex( size_t idx ) { m_index = idx; }
void SBitmap::SetScale( float scale ) { m_scale = scale; }
void SBitmap::SetPosition( const wxPoint& pos ) { m_pos = pos; }
void SBitmap::SetY( int PosY ) { m_pos.y = PosY; }
void SBitmap::SetX( int PosX ) { m_pos.x = PosX; }
