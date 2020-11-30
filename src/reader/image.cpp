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
#include <iostream>

namespace Reader
{

Image::Image( wxScrolledWindow* parent )
{
    this->parent = parent;
    this->maxWidth = parent->GetClientSize().GetWidth();
}

void Image::Open( const wxString& path )
{
    if ( !(this->files) )
    {
        delete this->files;
    }
    this->files = Handler::Find(path);
    this->files->Traverse();
    this->file = path;
    this->Load(path);
}

Image::VectorBitmap Image::Get()
{
    return this->bitmap;
}

int Image::Get( const wxPoint& area, const wxPoint& position ) const
{
    int
    posY = area.y + position.y,
    posX = area.x + position.x,
    bmpPosY, bmpAfterY,
    bmpPosX, bmpAfterX;
    for ( size_t i = 0; i < this->bitmap.size(); i++ )
    {
        bmpPosY = this->imagePosY.at(i);
        bmpAfterY = bmpPosY + this->bitmap.at(i).GetHeight();
        bmpPosX = this->imagePosX.at(i);
        bmpAfterX = bmpPosX + this->bitmap.at(i).GetWidth();
        if  (    
                posY >= bmpPosY && posY <= bmpAfterY 
                && posX >= bmpPosX && posX <= bmpAfterX
            )
        {
            return i;
        }
    }
    return -1;
}

wxVector<int> Image::Get( const wxPoint& position, const wxSize& size ) const
{
    int
    top = position.y, bottom = top + size.GetHeight(),
    left = position.x, right = left + size.GetHeight(),
    bmpPosY, bmpAfterY, bmpPosX, bmpAfterX;

    wxVector<int> bmp;
    for ( size_t i = 0; i < this->bitmap.size(); i++ )
    {
        bmpPosY = this->imagePosY.at(i);
        bmpAfterY = bmpPosY + this->bitmap.at(i).GetHeight();
        bmpPosX = this->imagePosX.at(i);
        bmpAfterX = bmpPosY + this->bitmap.at(i).GetWidth();
        if  (   (   ( bmpPosY >= top || bmpAfterY >= top )
                &&  ( bmpPosY <= bottom || bmpAfterY <= bottom )  )
                &&  ( ( bmpPosX >= left || bmpAfterY >= left )
                ||  ( bmpPosX <= right || bmpAfterX <= right ) ) 
            )
        {
            bmp.push_back( i );
        }
    }
    return bmp;
}

void Image::Load( const wxString& path )
{
    int i = this->files->Index(path);
    this->prev = i - 1;
    this->next = i + 1;
    this->showNext = 0;
    this->showPrev = 0;
    
    if ( this->LoadAt( i ) ) 
    {
        this->LoadBitmap( this->image.back() ); // load first opened file
    }

    this->isThreadLoadBitmap = true;
    this->CreateThread();
    this->GetThread()->Run();
}

void Image::ThreadLoadBitmap()
{
    // checking whether to load the next index or not
    bool isShowNext =   this->files->IsExist(this->next) 
                        && this->showNext < this->config->Read("showNext", 10 );

    // loading the next index if there is no Thread Destroy Testing
    if ( isShowNext && !this->TestDestroy() )
    {
        if ( this->LoadAt( this->next, VECTOR_PUSH ) )
        {
            this->LoadBitmap( this->image.back(), VECTOR_PUSH );
            this->showNext++;
        }
        this->next++;
    }

    // checking whether to load the previous index or not
    bool isShowPrev =   this->files->IsExist( this->prev ) 
                        && this->showPrev < this->config->Read("showPrev",10);

    // loading the next index if there is no Thread Destroy Testing
    if ( isShowPrev && !this->TestDestroy() )
    {
        if ( this->LoadAt( this->prev, VECTOR_BEGIN ) )
        {
            this->LoadBitmap( this->image.front(), VECTOR_BEGIN );
            this->showPrev++;
        }
        this->prev--;
    }

    // check whether the loading already done or not if true would disable loadBitmap;
    if ( (!isShowNext && !isShowPrev) )
    { 
        this->isThreadLoadBitmap = false; 
    } 
}

void Image::ThreadPreLoadImage()
{
    if ( !this->isThreadLoadBitmap )
    {

    if ( this->files->IsExist( this->next ) && this->config->Read("CacheNext",10) ) 
    {
        this->LoadAt( this->next, VECTOR_PUSH );
        this->next++;
        this->cacheNext++;
    }

    if ( this->files->IsExist( this->prev ) && this->config->Read("CachePrev", 10)  )
    {
        this->LoadAt( this->prev, VECTOR_PUSH );
        this->prev--;
        this->cachePrev++;
    }

    } // end of conditional isThreadLoadBitmap
}

wxThread::ExitCode Image::Entry() // might need to refactor
{
    while ( this->isThreadLoadBitmap || this->isThreadPreLoadImage )
    {
        if ( this->isThreadLoadBitmap && !this->TestDestroy())
        {
            this->ThreadLoadBitmap();
        }

        if ( this->isThreadPreLoadImage && !this->TestDestroy() )
        {
            this->ThreadPreLoadImage();
        } 

        if ( this->TestDestroy() )
        {
            this->isThreadLoadBitmap = false;
            this->isThreadPreLoadImage = false;
        }
    }
    return (wxThread::ExitCode)0;
}

void Image::LoadBitmap( const wxImage& img, VectorPos vectorPos )
{
    wxBitmap bmp = wxBitmap( img );
    int viewY = -1, viewX = -1; // changed when insert to beginning so user's visiblity doesnt move

    if ( bmp.GetWidth() > this->maxWidth )
    {
        this->maxWidth = bmp.GetWidth();
    }

    switch ( vectorPos )
    {
        case VECTOR_PUSH:
        {
            wxCriticalSectionLocker locker(gCS);
            this->bitmap.push_back(bmp);
            this->AddPosition( bmp ); // Add Image Position if just push
            break;
        }
        case VECTOR_BEGIN:
        {
            viewY = this->GetParent()->GetViewStart().y + bmp.GetHeight();
            wxCriticalSectionLocker locker(this->gCS);
            this->bitmap.insert( this->bitmap.begin(), bmp );
            this->RefreshImagePosition(); // Recalculate image Position if insert to begining
            break;
        }
    }

    this->GetParent()->SetVirtualSize( this->maxWidth,this->imagePosY.back() ); // expand virtual size
    this->GetParent()->Scroll( viewX, viewY); // Adjust user's view if needed
    this->GetParent()->Refresh();
}

bool Image::LoadAt( int index, VectorPos vecPos )
{
    if ( this->files->IsExist(index))
    {
        wxInputStream* filename = this->files->Item(index); // get InputStream from given index
        if ( wxImage::CanRead( *(filename) ) ) // check whether the handler can Read the filename
        {
            wxImage img = wxImage( *(filename) );
            switch ( vecPos )
            {
                case VECTOR_PUSH:
                {
                    this->image.push_back( img ); // push image to the end
                }
                case VECTOR_BEGIN:
                {
                    this->image.insert( this->image.begin(), img ); // insert image in the beginning
                }
            }
            return true; // to determinate whether the image can be loaded or not
        }
    }
    return false;
}

int Image::GetCenteredPosition( int width )
{
    int clientWidth = this->GetParent()->GetSize().GetWidth();
    if ( width > this->maxWidth ) width = this->maxWidth;
    int pos = (clientWidth-width)/2;
    return pos;
}

void Image::RefreshImagePosition()
{
    wxVector<int> x,y;
    x.reserve( this->bitmap.size() );
    y.reserve( this->bitmap.size() + 1 );
    for ( const auto& it : this->bitmap )
    {
        this->AddPosition( it, x, y );
    }

    wxCriticalSectionLocker locker(gCS);
    this->imagePosX = x;
    this->imagePosY = y;
}

void Image::AddPosition( const wxBitmap& bmp, wxVector<int>& x, wxVector<int>& y )
{
    wxCriticalSectionLocker locker(this->gCS);
    if ( y.empty() )
    {
        y.push_back(0);
    }
    x.push_back( this->GetCenteredPosition( bmp.GetWidth() ) );
    y.push_back( y.back() + bmp.GetHeight() );
}

void Image::AddPosition( const wxBitmap& bmp )
{
    this->AddPosition( bmp, this->imagePosX, this->imagePosY);
}


void Image::Clear()
{
    this->bitmap.clear();
    this->image.clear();
    this->imagePosY.clear();
    this->imagePosX.clear();
    this->maxWidth = 0;

    if ( this->GetThread() && this->GetThread()->IsRunning() )
    {
        this->GetThread()->Delete();
    }
}

Image::~Image()
{
    this->Clear();
    if ( !(this->files) )
    {
        delete this->files;
    }
}

} // end of namespace reader