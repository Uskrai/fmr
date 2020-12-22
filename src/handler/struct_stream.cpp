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

#include "handler/struct_stream.h"

#include <wx/wfstream.h>
#include <wx/mstream.h>
#include <wx/filename.h>
#include "handler/handler_factory.h"
#include "handler/abstract_handler.h"
#include <wx/wxcrtvararg.h>

namespace fmr
{

char DUMMY_BUFFER;

SStream::SStream( void *data, size_t length )
{
    Open(data,length);
}

SStream::SStream( const wxString &name )
    : SStream()
{
    if ( wxFileName::Exists(name) )
    {
        wxFileInputStream stream(name);
    }
    m_name = name;
}

SStream::SStream( wxInputStream *stream )
    : SStream()
{
    Open(stream);
}

void SStream::Open( void *data, size_t length )
{
    m_stream = std::shared_ptr<wxMemoryOutputStream>(
        new wxMemoryOutputStream(data,length)
    );
}

void SStream::Open( const wxString &name )
{
    if ( wxFileName::Exists(name) )
    {
        wxFileInputStream stream(name);
        m_stream->Write( stream );
        SetName( name );
    }
}

void SStream::Open( wxInputStream *stream )
{
    if ( stream )
    {
        m_stream->Write( *stream );
    }
}

void SStream::Open( const wxMemoryOutputStream &stream )
{
    void *data = NULL;
    size_t length = 0;
    stream.CopyTo( data, length );
    Open( data, length );
}

void SStream::Open( std::shared_ptr<wxMemoryOutputStream> stream )
{
    m_stream = stream;
}

void SStream::Open( wxMemoryOutputStream *stream )
{
    if ( stream )
        Open( *stream );
}

void SStream::SetName( const wxString &name )
{
    m_name = name;
}

void SStream::SetName( const std::wstring &name )
{
    SetName( name );
}

void SStream::SetName( const std::string &name )
{
    SetName( wxString(name) );
}


bool SStream::IsOk() const
{
    return m_stream->IsOk() && 
        m_stream->GetSize() != 0;
}

std::shared_ptr<AbstractHandler> SStream::GetHandler() 
{
    return m_handler;
}

std::shared_ptr<wxMemoryInputStream> SStream::GetStream() const
{
    std::shared_ptr<wxMemoryInputStream> stream;
    if ( m_stream->IsOk() && m_stream->GetSize() != 0 )
        stream = std::shared_ptr<wxMemoryInputStream>( 
            new wxMemoryInputStream(*m_stream) 
        );
    else
        stream = std::shared_ptr<wxMemoryInputStream>( 
            new wxMemoryInputStream( &DUMMY_BUFFER, sizeof(DUMMY_BUFFER))
        );
        
    return stream;
}

std::shared_ptr<wxMemoryOutputStream> SStream::GetOutputStream()
{
    return m_stream;
}

}; // namespace fmr