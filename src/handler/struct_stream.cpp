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

void SStream::Open( void *data, size_t length )
{
    m_stream = std::shared_ptr<wxMemoryOutputStream>(
        new wxMemoryOutputStream(data,length)
    );
}

SStream::SStream( void *data, size_t length )
{
    Open(data,length);
}

SStream::SStream( const wxString &name )
    : SStream()
{
    Open( name );
}

SStream::SStream( wxInputStream *stream )
    : SStream()
{
    Open(stream);
}

SStream::SStream( const SStream &copy )
    : SStream()
{
    Open( copy.m_stream );
    SetName( copy.m_name );
    SetHandlerPath( copy.handler_path_ );
}

SStream::SStream( SStream &&move )
{
    m_stream = std::move( move.m_stream );
    m_name = std::move( move.m_name );
    handler_path_ = std::move( move.handler_path_ );
}

SStream &SStream::operator = ( const SStream &copy )
{
    Open( copy.m_stream );
    SetName( copy.m_name );
    return *this;
}

void SStream::Open( const wxString &name )
{
    if ( wxFileName::FileExists(name) )
    {
        wxFileInputStream stream(name);
        m_stream->Write( stream );
    }
    SetName( name );
}

void SStream::Open( wxInputStream *stream )
{
    if ( stream )
        m_stream->Write( *stream );
}

void SStream::Open( const wxMemoryOutputStream &stream )
{
    Open( stream.GetOutputStreamBuffer()->GetBufferStart(), stream.GetSize() );
}

void SStream::Open( std::shared_ptr<wxMemoryOutputStream> stream )
{
    if ( !stream ) return;
    Open( *stream );
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
    SetName( wxString(name) );
}

void SStream::SetName( const std::string &name )
{
    SetName( wxString(name) );
}

void SStream::SetHandlerPath( const wxString &path )
    { handler_path_ = path; }

void SStream::SetDir( bool is_dir )
    { is_dir_ = is_dir; }

bool SStream::IsOk() const
{
    return m_stream->IsOk() &&
        m_stream->GetSize() != 0;
}

bool SStream::IsDir() const
    { return is_dir_; }

size_t SStream::GetSize() const
    { return m_stream->GetSize(); }

std::shared_ptr<AbstractHandler> SStream::GetHandler()
{
    return m_handler;
}

std::shared_ptr<wxMemoryInputStream> SStream::GetStream() const
{
    std::shared_ptr<wxMemoryInputStream> stream = NULL;
    if ( m_stream )
    {
        if ( m_stream->IsOk() && m_stream->GetSize() != 0 )
        stream = std::shared_ptr<wxMemoryInputStream>(
            new wxMemoryInputStream(*m_stream)
        );
        else
            stream = std::shared_ptr<wxMemoryInputStream>(
                new wxMemoryInputStream( &DUMMY_BUFFER, sizeof(DUMMY_BUFFER))
            );
    }
    else
        stream = std::shared_ptr<wxMemoryInputStream>(
            new wxMemoryInputStream( &DUMMY_BUFFER, sizeof( DUMMY_BUFFER ) )
        );

    return stream;
}

std::shared_ptr<wxMemoryOutputStream> SStream::GetOutputStream()
    { return m_stream; }

const std::shared_ptr<wxMemoryOutputStream> SStream::GetOutputStream() const
    { return m_stream; }

size_t SStream::CopyTo( void *buffer, size_t length ) const
    { return GetOutputStream()->CopyTo( buffer, length ); }

StreamEvent::StreamEvent( const StreamEvent &other )
    : wxCommandEvent( other.GetEventType(), other.GetId() )
{
    SetStream( other.stream_ );
    SetIndex( other.index_ );
}

StreamEvent::StreamEvent( StreamEvent &&other )
    : wxCommandEvent( other.GetEventType(), other.GetId() )
{
    SetStream( other.stream_ );
    index_ = std::move(other.index_);
}

void StreamEvent::SetStream( std::shared_ptr<SStream> stream )
    { stream_ = stream; }

void StreamEvent::SetIndex( size_t index )
    { index_ = index; }

const std::shared_ptr<SStream> StreamEvent::GetStream() const
    { return stream_; }

std::shared_ptr<SStream> StreamEvent::GetStream()
    { return stream_; }

size_t StreamEvent::GetIndex()
    { return index_; }

}; // namespace fmr