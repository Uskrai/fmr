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

#include "handler/std_handler.h"
#include <wx/wfstream.h>

namespace fs = std::filesystem;

#include "base/path.h"
#include "base/compare.h"
#include <algorithm>

namespace fmr
{

STDHandler::STDHandler( const wxString &path )
{
    Open( path );
}

void STDHandler::Open( const wxString &path )
{
    filename_ = path;
    name_ = Path::GetDirName(path);
    is_opened_ = name_ != L"";

    wxString parent = Path::GetParent( name_ );
    if ( parent != name_ )
    {
        parent_ = std::make_shared<STDHandler>(
            STDHandler(parent)
        );
    }
}

const wxString &STDHandler::GetName() const
    { return name_; }

const std::shared_ptr<AbstractHandler> STDHandler::GetParent() const
    { return parent_; }

std::shared_ptr<AbstractHandler> STDHandler::GetParent()
    { return parent_; }

const std::vector<SStream> &STDHandler::GetChild() const
    { return list_stream_; }

std::vector<SStream> &STDHandler::GetChild()
    { return list_stream_; }

wxString STDHandler::GetNext() const
    { return GetFromCurrent( 1 ); }

wxString STDHandler::GetPrev() const
    { return GetFromCurrent( -1 ); }

size_t STDHandler::Size() const
    { return list_stream_.size(); }

SStream &STDHandler::Item( size_t index )
    { return list_stream_.at( index ); }

const SStream &STDHandler::Item( size_t index ) const
    { return list_stream_.at( index ); }

bool STDHandler::IsOpened() const
    { return is_opened_; }

bool STDHandler::IsExist( size_t idx ) const
    { return Vector::IsExist( list_stream_ ,idx); }

bool STDHandler::GetFirst( SStream &stream, DirGetFlags flags, bool is_get_stream )
{
    fs::directory_options options;

    if ( flags & kDirFollowSymLink )
        options |= fs::directory_options::follow_directory_symlink;

    if ( flags & kDirSkipDenied )
        options |= fs::directory_options::skip_permission_denied;

    iterator_flags_ = flags;
    iterator_item_ = fs::directory_iterator( GetName().ToStdWstring(), options );
    iterator_ = fs::begin( iterator_item_ );
    return GetNextStream( stream );
}

bool STDHandler::GetNextStream( SStream &stream, bool is_get_stream )
{
    if ( iterator_ == fs::end( iterator_item_ ) )
        return false;

    fs::path path = iterator_->path();
    iterator_++;

    stream.SetName( path.wstring() );
    stream.SetDir( fs::is_directory( path ) );

    if ( is_get_stream )
        stream.Open( path.wstring() );

    return true;
}

wxString STDHandler::GetFromCurrent( int step ) const
{
    if ( GetParent() )
    {
        wxString parent = GetParent()->GetName();
        const auto &arr = GetParent()->GetChild();
        size_t idx = Index( GetName() );

        if ( Vector::IsExist(arr, idx + step) )
            return arr.at(idx + step).GetName();
    }
    return "";
}

size_t STDHandler::Index( const wxString &name ) const
{
    size_t i = 0;
    if ( name == GetName() ) return 0;

    for ( const auto &it : list_stream_ )
    {
        if ( name == it.GetName() ) return i;
        i++;
    }
    return -1;
}

void STDHandler::Traverse( bool is_get_stream, DirGetFlags flags )
{
    SStream stream;
    bool cont = GetFirst( stream, flags , is_get_stream );

    while ( cont )
    {
        list_stream_.push_back( stream );
        cont = GetNextStream( stream, is_get_stream );
    }

    std::sort( list_stream_.begin(), list_stream_.end(), Compare::NaturalSortable );
}

bool STDHandler::MakeDir( std::wstring directory_name, bool overwrite )
{
    if ( ! IsOpened() )
        return false;

    std::wstring path = GetName().ToStdWstring() + directory_name;
    if ( fs::exists( path ) && !overwrite )
        return false;

    SStream stream;

    stream.SetName( path );
    stream.SetDir();

    list_write_stream_.push_back( stream );

    return true;
}

bool STDHandler::MakeFile( const std::wstring &file_name, SStream stream, bool overwrite )
{
    if ( !IsOpened() )
        return false;

    std::wstring path = GetName().ToStdWstring() + file_name;
    if ( fs::exists( path ) && !overwrite )
        return false;

    stream.SetName( path );
    list_write_stream_.push_back( std::move( stream ) );

    return true;
}

void STDHandler::Reset()
{
    Close();
    Clear();
    is_opened_ = false;
    name_ = L"";
    filename_ = L"";
    parent_ = NULL;
}

void STDHandler::Clear()
{
    list_stream_.clear();
    list_write_stream_.clear();
}

void STDHandler::Close()
{
    iterator_item_.~directory_iterator();
    iterator_flags_ = kDirNone;
    iterator_.~directory_iterator();
}

} // namespace fmr