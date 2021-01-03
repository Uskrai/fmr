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
    is_opened_ = true;

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

bool WxArchiveHandler::IsOpened() const
    { return is_opened_; }

wxString WxArchiveHandler::GetNext() const
    { return GetFromCurrent(1); }

wxString WxArchiveHandler::GetPrev() const
    { return GetFromCurrent(-1); }

std::vector<SStream> &WxArchiveHandler::GetWriteList()
    { return list_write_stream_; }

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
    const wxArchiveClassFactory *factory;

    if ( ! Find( path, factory ) )
        return false;

    auto instream = std::unique_ptr<wxFileInputStream>(
        new wxFileInputStream( path )
    );

    iterator_flags_ = flags;
    iterator_item_ = factory->NewStream( instream.release() );
    return GetNextStream( stream, is_get_stream );
}

bool WxArchiveHandler::GetNextStream( SStream &stream, bool is_get_stream )
{
    if ( !iterator_item_ )
        return false;

    auto entry = std::unique_ptr<wxArchiveEntry>(
        iterator_item_->GetNextEntry()
    );

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

bool WxArchiveHandler::Find( wxString path, const wxArchiveClassFactory*& factory )
{

    factory = wxArchiveClassFactory::Find( path, wxSTREAM_FILEEXT );
    if ( factory ) 
        return true;

    const wxFilterClassFactory* fcf;
    fcf = wxFilterClassFactory::Find( path, wxSTREAM_FILEEXT );

    if ( fcf )
    {
        path = fcf->PopExtension(path);
        factory = wxArchiveClassFactory::Find( path, wxSTREAM_FILEEXT );
        if ( factory )
            return true;
    }

    return false;
}

bool WxArchiveHandler::CanHandle( wxString path )
{
    const wxArchiveClassFactory* factory;
    bool result = WxArchiveHandler::Find( path, factory );
    return result;
}

bool WxArchiveHandler::CreateDirectories()
{
    if ( !IsOpened() )
        return false;

    const wxArchiveClassFactory *factory;
    wxString path = GetName();
    if ( ! WxArchiveHandler::Find( path, factory ) )
        return false;

    auto file_output = std::shared_ptr<wxTempFileOutputStream>(
        new wxTempFileOutputStream( GetName() )
    );

    auto archive_output = std::unique_ptr<wxArchiveOutputStream>(
         factory->NewStream( *file_output.get() )
    );

    wxArchiveEntry *entry = factory->NewEntry();
    archive_output->PutNextEntry( entry );
    if ( archive_output->Close() )
        return file_output->Commit();
    return false;
}

bool WxArchiveHandler::CreateDirectory( const std::wstring &dirname, bool overwrite )
{
    if ( !IsOpened() )
        return false;

    if ( Path::HasRootPath( dirname ) )
        return false;

    StreamActionType flags = kStreamWrite;

    if ( overwrite )
        flags |= kStreamOverwrite;

    SStream stream;
    stream.SetName( dirname );
    stream.SetDir();
    stream.SetType( flags );

    GetWriteList().push_back( std::move( stream ) );
    return true;
}

bool WxArchiveHandler::CreateFiles( SStream stream, const std::wstring &filename, bool overwrite )
{
    if ( !IsOpened() )
        return false;

    if ( Path::HasRootPath( filename ) )
        return false;

    StreamActionType flags = kStreamWrite;

    if ( overwrite )
        flags |= kStreamOverwrite;

    stream.SetName( filename );
    stream.SetType( flags );

    GetWriteList().push_back( std::move( stream ) );
    return true;
}

bool WxArchiveHandler::Remove( const std::wstring &name, bool recursive )
{
    if ( !IsOpened() )
        return false;

    if ( Path::HasRootPath( name )  )
        return false;

    StreamActionType flags = kStreamRemove;

    if ( recursive )
        flags |= kStreamRecursive;

    SStream stream;
    stream.SetName( name );
    stream.SetType( flags );

    GetWriteList().push_back( std::move( stream ) );
    return true;
}

bool WxArchiveHandler::RemoveAll()
{
    const wxArchiveClassFactory *factory;
    bool ret = false;

    if ( wxFileName::FileExists( GetName() ) && Find( GetName(), factory ) )
        ret = wxRemoveFile( GetName() );

    return ret;
}

bool WxArchiveHandler::CommitWrite()
{
    if ( ! IsOpened() )
        return false;

    const wxArchiveClassFactory *factory;
    if ( !( wxFileName::FileExists( GetName() ) && Find( GetName(), factory ) ) )
        return false;

    wxInputStream *in_stream = new wxFileInputStream( GetName() );
    auto archive_input = std::unique_ptr<wxArchiveInputStream>(
        factory->NewStream( in_stream )
    );

    wxTempFileOutputStream temp_output( GetName() );

    auto archive_output = std::unique_ptr<wxArchiveOutputStream>(
        factory->NewStream( temp_output )
    );

    if ( archive_input->IsOk() &&  archive_input->GetSize() != 0 )
        archive_output->CopyArchiveMetaData( *archive_input );

    SStream temp_stream;
    bool cont = GetFirst( temp_stream, kDirDefault, true );

    while( cont )
    {
        GetWriteList().push_back( std::move( temp_stream ) );
        cont = GetNextStream( temp_stream, true );
    }

    {
        auto it = GetWriteList().begin();
        while( it != GetWriteList().end() )
        {
            bool is_increment_it = true;
            auto next = it;
            while ( next != GetWriteList().end() )
            {
                bool is_increment_next = true;
                if ( it != next && it->GetName() == next->GetName() )
                {
                    if ( next->GetType() & kStreamOverwrite )
                    {
                        GetWriteList().erase( it );
                        is_increment_it = false;
                        break;
                    }
                    else
                    {
                        GetWriteList().erase( next );
                        is_increment_next = false;
                    }
                }
                if ( is_increment_next )
                    ++next;
            }

            if ( is_increment_it )
                ++it;
        }
    }

    std::sort( GetWriteList().begin(), GetWriteList().end(), Compare::NaturalSortable );

    for ( auto it = GetWriteList().begin(); it != GetWriteList().end(); ++it )
    {
        if ( it->IsDir() )
            archive_output->PutNextDirEntry( it->GetName() );

        else
        {
            std::shared_ptr<char[]> buffer = std::shared_ptr<char[]>(
                new char[it->GetSize()]
            );

            size_t length = it->CopyTo( buffer.get(), it->GetSize() );
            archive_output->PutNextEntry( it->GetName() );
            archive_output->Write( buffer.get(), length );
        }
    }

    archive_input.reset();
    archive_output->Close();

    if ( GetWriteList().empty() )
        return CreateDirectories();

    return temp_output.Commit();
}

void WxArchiveHandler::Reset()
{
    Clear();
    Close();
    m_name = L"";
    m_parent = NULL;
}

void WxArchiveHandler::Clear()
{
    GetChild().clear();
    GetWriteList().clear();
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