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

#ifndef FMR_EXPLORER_LOADTHREAD
#define FMR_EXPLORER_LOADTHREAD

#include "explorer/common.h"
#include "thread/thread.h"
#include "handler/struct_stream.h"
#include "handler/abstract_handler.h"
#include "handler/handler_factory.h"

namespace fmr
{

namespace explorer
{

wxDECLARE_EVENT( EVT_STREAM_FOUND, StreamEvent );

class FindThread
    : public BaseThread
{
    public:
        FindThread( ThreadController *parent, wxThreadKind type, int id )
            : BaseThread( parent, type, id ){};
        void SetParameter( std::vector<StreamBitmap> &list_stream );

        bool Find( StreamBitmap &item );
    private:
        std::vector<StreamBitmap> list_stream_;

        ExitCode Entry();
};


}; // end of namespace Explorer

}; // namespace fmr

#endif