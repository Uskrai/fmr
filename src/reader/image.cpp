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

#include "reader/image.h"

namespace Reader
{

Image::Image( wxWindow* parent )
{
    this->parent = parent;
}

void Image::Open( wxString path )
{
    this->files->Open(path);
    this->file = path;
    this->Load(path);
}

void Image::Load( wxString path )
{
    int i = this->files->Index(path);
    this->prev = i - 1;
    this->next = i + 1;
    
    this->LoadAt( i );

    this->isLoadBitmap = true;
    // this->CreateThread();
}

wxThread::ExitCode Image::Entry()
{
    int size = this->files->Size();
    while   
    ( 
        !this->TestDestroy() && 
        (
            ( ( this->next > 0 ) && ( this->next < size ) )
            || ( (this->prev > 0) && ( this->prev < size ) )
        ) // end of !this->testDestroy()
    ) // end of conditional for while
    {
        if ( this->isLoadBitmap == true && !this->TestDestroy()) // used when loading bitmap after opening file
        {
            if ( this->next > 0 && this->next < size && !this->TestDestroy() )
            {
                if ( this->LoadAt( this->next++, VECTOR_PUSH ) )
                {
                    this->LoadBitmap( this->image.back(), VECTOR_PUSH );
                }
            }

            if ( this->prev > 0 && (this->prev < size ) && !this->TestDestroy() )
            {
                if ( this->LoadAt( this->prev--, VECTOR_BEGIN ) )
                {
                    this->LoadBitmap( this->image.front(), VECTOR_BEGIN );
                }
            }
        } // end of loadBitmap;
        if ( this->isPreLoadImage == true && !this->TestDestroy() )
        {
            

        }
    }
    return (wxThread::ExitCode)0;
}

void Image::PreLoadImage( )
{
    
}

bool Image::LoadAt( int index, VectorPos vecPos )
{
    wxString filename = this->files->Item(index);
    if ( wxImage::CanRead(filename) )
    {
        
        ImagePtr img = ImagePtr( new wxImage(filename) );
        switch ( vecPos )
        {
            case VECTOR_PUSH:
            {
                this->image.push_back( img );
            }
            case VECTOR_BEGIN:
            {
                this->image.insert( this->image.begin(), img );
            }
        }
        return true;
    }
    return false;
}

}