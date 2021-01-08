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

DefaultHandler::DefaultHandler( DefaultHandler &&handler )
{
    using std::move;
    opened_name_ = move( handler.opened_name_ );
    parent_handler_ = move( handler.parent_handler_ );
    m_all = move( handler.m_all );
    list_write_stream_ = move( handler.list_write_stream_ );
}

DefaultHandler::DefaultHandler( const DefaultHandler &handler )
{
    if ( handler.is_opened_ )
        Open( MakeDirectory(
            handler.GetName().ToStdWstring()
        ));

    m_filename = handler.m_filename;
    m_all = handler.m_all;
    list_write_stream_ = handler.list_write_stream_;
}

void DefaultHandler::Open( const wxString& path )
{
    opened_name_ = Path::GetDirName(path.ToStdWstring());
    is_opened_ = true;

    std::wstring parent = Path::GetParent( GetName().ToStdWstring() );
    if ( ! Path::IsRoot( GetName().ToStdWstring() ))
    {
        parent_handler_ = std::shared_ptr<DefaultHandler>( 
            new DefaultHandler( parent )
        );
    }
}

const wxString &DefaultHandler::GetName() const
    { return opened_name_; }

const std::shared_ptr<AbstractHandler> DefaultHandler::GetParent() const
    { return parent_handler_; }

std::shared_ptr<AbstractHandler> DefaultHandler::GetParent()
    { return parent_handler_; }

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

bool DefaultHandler::IsOpened() const
    { return is_opened_; }

wxString DefaultHandler::GetNext() const
    { return GetFromCurrent(1); }

wxString DefaultHandler::GetPrev() const
    { return GetFromCurrent(-1); }

std::wstring DefaultHandler::GetItemPath( const SStream &stream ) const
{
    return Path::Append(
        stream.GetHandlerPath().ToStdWstring(),
        stream.GetName().ToStdWstring()
    );
}

std::wstring DefaultHandler::GetItemPath( size_t index ) const
{
    return GetItemPath( Item( index ) );
}

wxString DefaultHandler::GetFromCurrent( int step ) const
{
    if ( !GetParent() )
        return wxEmptyString;

    size_t idx = GetParent()->Index( GetName() );

    if ( GetParent()->IsExist( idx + step ) )
        return parent_handler_->GetItemPath( idx + step );

    return L"";
}

size_t DefaultHandler::Index( const wxString& path ) const
{
    if ( path == GetName() )
        return 0;

    std::wstring temp = path.ToStdWstring();
    if ( Path::IsAbsolute( temp ) )
        temp = MakeRelative( GetName().ToStdWstring(), path.ToStdWstring() );

    Path::RemoveDirSep( temp );

    size_t idx = 0;
    while( Vector::IsExist( m_all, idx) )
    {
        if ( m_all.at(idx).GetName() == temp )
            return idx;
        idx++;
    }
    return -1;
}

void DefaultHandler::Traverse( bool is_get_stream, DirGetFlags flags )
{
    SStream stream;
    std::vector<SStream> vec_stream;

    bool cont = GetFirst( stream, flags, is_get_stream );

    vec_stream.reserve( Size() );
    while ( cont )
    {
        vec_stream.push_back( stream );
        cont = GetNextStream( stream, is_get_stream );
    }

    vec_stream.shrink_to_fit();
    std::sort( vec_stream.begin(), vec_stream.end(), Compare::NaturalSortable );
    m_all = std::move( vec_stream );
}

void DefaultHandler::OpenStream( const std::wstring &filename, SStream &stream, bool is_get_stream )
{
    std::wstring appended_path = Path::Append( GetName().ToStdWstring(), filename );
    stream.SetName( Path::MakeRelative( GetName().ToStdWstring(), appended_path) );
    stream.SetHandlerPath( GetName() );
    stream.SetDir( wxFileName::DirExists( stream.GetName() ) );

    if ( is_get_stream )
        stream.Open( appended_path );
}

bool DefaultHandler::GetFirst( SStream &stream, DirGetFlags flags, bool is_get_stream )
{
    if ( GetName() == L"" )
        return false;

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


    OpenStream( filename.ToStdWstring(), stream, is_get_stream );
    return true;
}

bool DefaultHandler::GetNextStream( SStream &stream, bool is_get_stream )
{
    wxString filename;

    if ( !opened_directory_.IsOpened() )
        return false;

    if ( ! opened_directory_.GetNext( &filename ) )
        return false;

    OpenStream( filename.ToStdWstring(), stream, is_get_stream );
    return true;
}

bool DefaultHandler::CreateDirectories()
{
    return  IsOpened()
            && CreateDirectories( GetName().ToStdWstring() );
}

bool DefaultHandler::CreateDirectories( const std::wstring &path )
{
    return  path != ""
            && (
                wxDir::Exists( path )
                || wxDir::Make( path, wxS_DIR_DEFAULT, wxPATH_MKDIR_FULL )
            );
}

bool DefaultHandler::CreateDirectory( const std::wstring &directory_name, bool overwrite )
{
    if ( ! IsOpened() )
        return false;

    if ( !IsChild( GetName().ToStdWstring(), directory_name ) )
        return false;

    SStream stream;
    StreamActionType flags = kStreamWrite;

    if ( overwrite )
        flags |= kStreamOverwrite;

    stream.SetName( directory_name );
    stream.SetHandlerPath( GetName() );
    stream.SetDir();
    stream.SetType( flags );

    list_write_stream_.push_back( std::move( stream ) );
    return true;
}

bool DefaultHandler::CreateFiles( SStream stream, const std::wstring &filename, bool overwrite )
{
    if ( !IsOpened() )
        return false;

    if ( !IsChild( GetName().ToStdWstring(), filename ) )
        return false;

    StreamActionType flags = kStreamWrite;

    if ( overwrite )
        flags |= kStreamOverwrite;

    stream.SetName( filename );
    stream.SetHandlerPath( GetName() );
    stream.SetType( flags );

    list_write_stream_.push_back( std::move( stream ) );
    return true;
}

bool DefaultHandler::Remove( const std::wstring &name, bool recursive )
{
    if ( !IsOpened() )
        return false;

    if ( !IsChild( GetName().ToStdWstring(), name ) )
        return false;

    SStream stream;
    StreamActionType flags = kStreamRemove;

    if ( recursive )
        flags |= kStreamRecursive;

    stream.SetName( name );
    stream.SetHandlerPath( GetName() );
    stream.SetType( flags );

    list_write_stream_.push_back( std::move( stream ) );
    return true;
}

bool DefaultHandler::RemoveAll()
{
    return  IsOpened()
            && RemoveAll( GetName().ToStdWstring() );
}

bool DefaultHandler::RemoveAll( const std::wstring &path )
{
    return  !path.empty()
            &&  (
                !wxFileName::Exists( path )
                || wxDir::Remove( path, wxPATH_RMDIR_RECURSIVE )
            );
}

void DefaultHandler::DoCreateFile( const SStream &stream )
{
    wxTempFileOutputStream out_stream( stream.GetHandlerPath() + stream.GetName() );
    char *buffer = new char[stream.GetSize()];

    size_t length = stream.CopyTo( buffer, stream.GetSize() );

    out_stream.Write( buffer, length );
    out_stream.Commit();
    delete[] buffer;
}

void DefaultHandler::DoRemove( const SStream &stream )
{
    int flags = ( stream.GetType() & kStreamRecursive )
                ? wxPATH_RMDIR_RECURSIVE
                : 0;

    wxDir::Remove( stream.GetHandlerPath() + stream.GetName(), flags );
}

void DefaultHandler::DoCreateDirectory( const SStream &stream )
{
    int flags = ( stream.GetType() & kStreamRecursive )
                ? wxPATH_MKDIR_FULL
                : 0;

    wxDir::Make( stream.GetHandlerPath() + stream.GetName(), wxS_DIR_DEFAULT, flags );
}

bool DefaultHandler::CommitWrite()
{
    if ( ! wxDir::Exists( GetName() ) )
        return false;

    for ( const auto &it : list_write_stream_ )
    {
        if ( it.GetType() & kStreamRead )
            continue;

        if ( wxFileName::Exists( it.GetHandlerPath() + it.GetName() ) )
        {
            if ( it.GetType() & kStreamOverwrite )
                DoRemove( it );
            else
                continue;
        }

        if ( it.IsDir() )
            DoCreateDirectory( it );
        else if ( it.GetType() & kStreamWrite )
            DoCreateFile( it );
        else if ( it.GetType() & kStreamRemove )
            DoRemove( it );

    }

    list_write_stream_.clear();

    return true;
}

void DefaultHandler::Reset()
{
    Clear();
    Close();

    list_write_stream_.clear();
    opened_name_ = wxEmptyString;
    is_opened_ = false;
    parent_handler_ = NULL;
}

void DefaultHandler::Clear()
{
    m_all.clear();
    list_write_stream_.clear();
}

void DefaultHandler::Close()
{
    Clear();
    opened_directory_.Close();
}

DefaultHandler::~DefaultHandler()
{
    Close();
}

}; // namespace fmr