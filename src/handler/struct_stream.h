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

#ifndef FMR_HANDLER_STRUCT_STREAM
#define FMR_HANDLER_STRUCT_STREAM

#include <memory>
#include <string>

#include "base/compare.h"
#include "handler/abstract_handler.h"

#include <wx/mstream.h>
class wxString;
class wxInputStream;
class wxOutputStream;
class wxMemoryInputStream;
class wxMemoryOutputStream;
class AbstractHandler;

namespace fmr
{

struct SStream
    : public Compare::Sortable
{
    SStream( void *data = NULL, size_t length = 0 );
    SStream( const wxString &name );
    SStream( wxInputStream *stream );
    SStream( const wxOutputStream &stream);
    SStream( wxOutputStream *stream );

    void Open( void *data = NULL, size_t length = 0 );
    void Open( const wxString &name );
    void Open( wxInputStream *stream );
    void Open( const wxMemoryOutputStream &stream );
    void Open( wxMemoryOutputStream *stream );

    void SetName( const wxString &name );
    void SetName( const std::wstring &name );
    void SetName( const std::string &name );

    bool IsOk() const;

    std::wstring GetString() const { return GetName(); }
    std::wstring GetName() const { return m_name; }
    std::shared_ptr<AbstractHandler> GetHandler();
    std::shared_ptr<wxMemoryInputStream> GetStream();
    std::shared_ptr<wxMemoryOutputStream> GetOutputStream();

    std::shared_ptr<wxMemoryOutputStream> m_stream;
    std::shared_ptr<AbstractHandler> m_handler;
    std::wstring m_name;
};

}; // namespace fmr

#endif