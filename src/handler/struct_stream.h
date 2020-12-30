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
#include <wx/event.h>

class wxInputStream;
class wxOutputStream;
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
    SStream( const SStream &copy );
    SStream( SStream &&move );

    SStream &operator=( const SStream &copy );

    void Open( void *data = NULL, size_t length = 0 );
    void Open( const wxString &name );
    void Open( wxInputStream *stream );
    void Open( const wxMemoryOutputStream &stream );
    void Open( wxMemoryOutputStream *stream );
    void Open( std::shared_ptr<wxMemoryOutputStream> stream );

    void SetName( const wxString &name );
    void SetName( const std::wstring &name );
    void SetName( const std::string &name );

    void SetHandlerPath( const wxString &path );
    void SetDir( bool is_dir = true );

    bool IsOk() const;
    bool IsDir() const;

    std::wstring GetString() const { return m_name.ToStdWstring(); }
    const wxString &GetName() const { return m_name; };
    const wxString &GetHandlerPath() const { return handler_path_; }
    size_t GetSize() const;
    std::shared_ptr<AbstractHandler> GetHandler();
    std::shared_ptr<wxMemoryInputStream> GetStream() const ;
    std::shared_ptr<wxMemoryOutputStream> GetOutputStream();

    std::shared_ptr<wxMemoryOutputStream> m_stream;
    std::shared_ptr<AbstractHandler> m_handler;
    wxString m_name, handler_path_;

    bool is_dir_ = false;
};

class StreamEvent
    : public wxCommandEvent
{
    public:
        StreamEvent( wxEventType comand_type, int id = 0 )
            : wxCommandEvent( comand_type, id ) {};
        StreamEvent( const StreamEvent &other );
        StreamEvent( StreamEvent &&other );
        wxEvent *Clone(){ return new StreamEvent( *this ); }

        void SetStream( std::shared_ptr<SStream> stream );
        void SetIndex( size_t index );

        const std::shared_ptr<SStream> GetStream() const;
        std::shared_ptr<SStream> GetStream();
        size_t GetIndex();
    private:
        std::shared_ptr<SStream> stream_;
        size_t index_;
};

typedef void (wxEvtHandler::*StreamEventFunction)(StreamEvent&);

#define StreamEventHandler( func ) wxEVENT_HANDLER_CAST( StreamEventFunction, func )

#define EVT_STREAM( id, type, func ) \
    wx__DECLARE_EVT1( type, id, StreamEventHandler(func) )

}; // namespace fmr

#endif