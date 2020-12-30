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

#include "handler/wxarchive_handler.h"
#include "handler/default_handler.h"

#include "base/compare.h"
#include <algorithm>

namespace fmr
{

WxArchiveHandler::WxArchiveHandler( const wxString& path )
{
    Open( path );
}

void WxArchiveHandler::Open( const wxString& path )
{
    m_name = path;

    wxString parent = Path::GetParent(path);
    if ( parent != m_name )
    {
        m_parent = std::shared_ptr<AbstractHandler>(
            new DefaultHandler(parent)
        );
        m_parentName = parent;
    }
}

size_t WxArchiveHandler::Index( const wxString& name ) const
{
    if ( name == m_name ) return 0;

    size_t idx = 0;
    for ( const auto &it : GetChild() )
    {
        if ( it.GetName() == name )
            return idx;
        idx++;
    }

    return -1;
}

const wxString &WxArchiveHandler::GetName() const
    { return m_name; }

const std::shared_ptr<AbstractHandler> WxArchiveHandler::GetParent() const
    { return m_parent; }

std::shared_ptr<AbstractHandler> WxArchiveHandler::GetParent()
    { return m_parent; }

const std::vector<SStream> &WxArchiveHandler::GetChild() const
    { return m_all; }

std::vector<SStream> &WxArchiveHandler::GetChild()
    { return m_all; }

const SStream &WxArchiveHandler::Item( size_t index ) const
    { return GetChild().at(index); }

SStream &WxArchiveHandler::Item( size_t index )
    { return GetChild().at(index); }

size_t WxArchiveHandler::Size() const
    { return GetChild().size(); }

bool WxArchiveHandler::IsExist( size_t index ) const
    { return Vector::IsExist( GetChild(), index  ); }

wxString WxArchiveHandler::GetNext() const
    { return GetFromCurrent(1); }

wxString WxArchiveHandler::GetPrev() const
    { return GetFromCurrent(-1); }

wxString WxArchiveHandler::GetFromCurrent( int i ) const
{
    if ( GetParent() )
    {
        auto &list_stream = GetParent()->GetChild();
        size_t idx = GetParent()->Index( GetName() );
        if ( idx != size_t(-1) && Vector::IsExist( list_stream, idx + i ) )
            return list_stream.at( idx + i ).GetName();
    }

    return wxEmptyString;
}

bool WxArchiveHandler::GetFirst( SStream &stream, DirGetFlags flags, bool is_get_stream )
{
    wxString path = GetName();
    wxInputStream *instream;
    const wxArchiveClassFactory *factory;

    if ( ! Find( path, factory, instream ) )
        return false;

    iterator_flags_ = flags;
    iterator_item_ = factory->NewStream( *instream );
    return GetNextStream( stream, is_get_stream );
}

bool WxArchiveHandler::GetNextStream( SStream &stream, bool is_get_stream )
{
    if ( !iterator_item_ )
        return false;

    wxArchiveEntry *entry = iterator_item_->GetNextEntry();

    if ( !entry )
    {
        Close();
        return false;
    }

    // skip if directory and no kDirDirectory in iterator_flags
    if ( entry->IsDir() && !(iterator_flags_ & kDirDirectory) )
        return GetNextStream( stream, is_get_stream );

    // skip if not directory ( file ) and no kDirFile in iterator_flags
    else if ( !( iterator_flags_ & kDirFile ) )
        return GetNextStream( stream, is_get_stream );


    stream.SetName( entry->GetName() );
    stream.SetDir( entry->IsDir() );
    stream.SetHandlerPath( GetName() );

    if ( is_get_stream )
        stream.Open( iterator_item_ );

    return true;
}

void WxArchiveHandler::Traverse( bool GetStream, DirGetFlags flags )
{
    SStream stream;

    bool cont = GetFirst( stream, flags, GetStream );

    while ( cont )
    {
        m_all.push_back( stream );
        cont = GetNextStream( stream, GetStream );
    }

    std::sort( m_all.begin(), m_all.end(), Compare::NaturalSortable );
}

bool WxArchiveHandler::Find( wxString& path, const wxArchiveClassFactory*& factory, wxInputStream*& in )
{

    factory = wxArchiveClassFactory::Find( path, wxSTREAM_FILEEXT );
    if ( factory ) 
    {
        if ( !in ) delete in;
        in = new wxFileInputStream(path);
        return true;
    }

    wxString filename = path;

    const wxFilterClassFactory* fcf;
    fcf = wxFilterClassFactory::Find( path, wxSTREAM_FILEEXT );

    if ( fcf )
    {
        if ( !in ) delete in;
        in = fcf->NewStream( new wxFileInputStream(filename));
        path = fcf->PopExtension(path);

        factory = wxArchiveClassFactory::Find( path, wxSTREAM_FILEEXT );

        if ( factory )
        {
            return true;
        }
    }

    return false;
}

bool WxArchiveHandler::CanHandle( wxString path )
{
    wxInputStream* in;
    const wxArchiveClassFactory* factory;
    bool result = WxArchiveHandler::Find( path, factory, in );
    if ( result ) delete in;
    return result;
}

void WxArchiveHandler::Clear()
{
    m_name = wxEmptyString;
    GetChild().clear();
}

void WxArchiveHandler::Close()
{
    if ( iterator_item_ )
    {
        iterator_item_->CloseEntry();
        delete iterator_item_;
        iterator_item_ = NULL;
    }
}

WxArchiveHandler::~WxArchiveHandler()
{
    Clear();
}

}; // namespace fmr