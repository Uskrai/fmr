/*
 *  Copyright (c) 2021 Uskrai
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

#ifndef FMR_COMMON_EVENT
#define FMR_COMMON_EVENT

#include <wx/event.h>
#include <vector>

namespace event
{
    template<typename EventType, typename Class, typename EventArg, typename EventHandler>
    void Bind( std::vector<EventType> type, void (Class::*fp)(EventArg&), EventHandler *handler, int id = wxID_ANY, int lastId = wxID_ANY )
    {
        for ( auto &it : type )
            handler->Bind( it, fp, handler, id, lastId );
    }
} // namespace window end

#endif // FMR_COMMON_WINDOW end

