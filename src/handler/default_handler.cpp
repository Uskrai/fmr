/*
 * 
 * Copyright (c) 2020 Uskrai
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 * 
 */

#include "handler/default_handler.h"
#include "handler/handler_factory.h"
#include <wx/wfstream.h>

#include "base/path.h"
#include "base/compare.h"
#include <algorithm>

namespace fmr
{


DefaultHandler::DefaultHandler( const wxString& path )
{
    this->Open(path);
}
void DefaultHandler::Open( const wxString& path )
{
    m_name = Path::GetDirName(path);

    wxString parent = Path::GetParent(m_name);
    if ( parent != m_name )
    {
        m_parent = std::shared_ptr<AbstractHandler>( 
            new DefaultHandler( parent )
        );
        m_parentName = parent;
    }
}

const wxString &DefaultHandler::GetName() const
    { return m_name; }

const std::shared_ptr<AbstractHandler> DefaultHandler::GetParent() const
    { return m_parent; }

std::shared_ptr<AbstractHandler> DefaultHandler::GetParent()
    { return m_parent; }

const std::vector<SStream> &DefaultHandler::GetChild() const
    { return m_all; }

std::vector<SStream> &DefaultHandler::GetChild()
    { return m_all; }

const SStream &DefaultHandler::Item( size_t idx ) const
    { return m_all.at(idx); }

SStream &DefaultHandler::Item( size_t idx ) 
    { return m_all.at(idx); }

size_t DefaultHandler::Size() const
    { return GetChild().size(); }

bool DefaultHandler::IsExist( size_t idx ) const
    { return Vector::IsExist( GetChild(), idx ); }

wxString DefaultHandler::GetNext() const
    { return GetFromCurrent(1); }

wxString DefaultHandler::GetPrev() const
    { return GetFromCurrent(-1); }

wxString DefaultHandler::GetFromCurrent( int i ) const
{
    if ( GetParent() )
    {
        const std::vector<SStream> &list_stream = GetParent()->GetChild();
        size_t idx = GetParent()->Index( GetName() );
        if ( idx != size_t(-1) && Vector::IsExist(list_stream, idx + i ) )
            return list_stream.at( idx + i ).GetName();
    }
    return wxEmptyString;
}

size_t DefaultHandler::Index( const wxString& path ) const
{
    if ( path == GetName() )
        return 0;
    
    size_t idx = 0;
    while( Vector::IsExist( m_all, idx) )
    {
        if ( m_all.at(idx).GetName() == path )
            return idx;
        idx++;
    }
    return -1;
}

void DefaultHandler::Traverse( bool is_get_stream, DirGetFlags flags )
{
    SStream stream;
    bool cont = GetFirst( stream, flags, is_get_stream );

    while ( cont )
    {
        m_all.push_back( stream );
        cont = GetNextStream( stream, is_get_stream );
    }

    std::sort( m_all.begin(), m_all.end(), Compare::NaturalSortable );
}

void DefaultHandler::OpenStream( const wxString &filename, SStream &stream, bool is_get_stream )
{
    stream.SetName( GetName() + filename );
    stream.SetHandlerPath( GetName() );
    stream.SetDir( wxFileName::DirExists( stream.GetName() ) );

    if ( is_get_stream )
        stream.Open( GetName() +  filename );
}

bool DefaultHandler::GetFirst( SStream &stream, DirGetFlags flags, bool is_get_stream )
{
    opened_directory_.Open( GetName() );

    if ( ! opened_directory_.IsOpened() )
        return false;

    int flag_dir = wxDIR_HIDDEN;
    if ( flags & kDirFile )
        flag_dir += wxDIR_FILES;

    if ( flags & kDirDirectory )
        flag_dir += wxDIR_DIRS;

    if ( !( flags & kDirFollowSymLink) )
        flag_dir += wxDIR_NO_FOLLOW;

    wxString filename;
    if ( ! opened_directory_.GetFirst( &filename, wxEmptyString, flag_dir ) )
        return false;


    OpenStream( filename, stream, is_get_stream );
    return true;
}

bool DefaultHandler::GetNextStream( SStream &stream, bool is_get_stream )
{
    wxString filename;

    if ( ! opened_directory_.GetNext( &filename ) )
        return false;

    OpenStream( filename, stream, is_get_stream );
    return true;
}

void DefaultHandler::GetAllFiles( std::vector<struct SStream> &vec_stream, int dir_flags )
{
    wxString filename;
    bool cont = dir.GetFirst( &filename, wxEmptyString, dir_flags );
    while ( cont )
    {
        filename =  Path::GetFullPath( dir.GetNameWithSep() + filename );
        SStream stream;
        stream.SetName( filename );
        stream.SetHandlerPath( GetName() );
        vec_stream.push_back( stream );


        cont = dir.GetNext( &filename );
    }
}

void DefaultHandler::Clear()
{
    m_all.clear();
}

void DefaultHandler::Close()
{
    Clear();
    m_name = wxEmptyString;
}

DefaultHandler::~DefaultHandler()
{
    Close();
}

}; // namespace fmr