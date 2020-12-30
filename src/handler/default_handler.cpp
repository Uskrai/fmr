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

void DefaultHandler::Traverse( bool GetStream, DirGetFlags flags )
{
    dir.Open( wxPathOnly( m_name ) );
    
    if ( ! dir.IsOpened() ) return;

    wxString filename;
    
    std::vector<struct SStream> directory, files;
    GetAllFiles( directory, wxDIR_DIRS | wxDIR_HIDDEN );
    GetAllFiles( files, wxDIR_FILES | wxDIR_HIDDEN );
    
    auto copy = [this]( std::vector<struct SStream> &target1, std::vector<struct SStream> &target2 ) -> void
    {
        for ( auto &it : target1 )
            m_all.push_back( it );
        for ( auto &it : target2 )
            m_all.push_back( it );
    };

    auto sort = [this]( std::vector<struct SStream> &target ) -> void
    {
        std::sort( target.begin(), target.end(), Compare::NaturalSortable );
    };


    if ( sort_flag_ == kSortAll )
    {
        copy( directory, files );
        sort( m_all );
    }
    else
    {
        sort( directory );
        sort( files );

        if ( sort_flag_ == kSortDirFirst )
        {
            copy( directory, files );
        }
        else if ( sort_flag_ == kSortFileFirst )
        {
            copy( files, directory );
        }
    }


    std::sort( m_all.begin(), m_all.end(), Compare::NaturalSortable );

    dir.Close();
    if ( GetStream ) TraverseStream();
}

void DefaultHandler::TraverseStream()
{
    for ( auto &it : m_all )
    {
        HandlerType type;
        HandlerFactory::Find( it.GetName(), type );
        if ( type != kHandlerDefault )
            continue;

        it.Open( it.GetName() );
    }
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