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
#ifndef HANDLER_HANDLER_FACTORY
#define HANDLER_HANDLER_FACTORY

class Handler;
class wxString;


enum HandlerType
{
    Archive,
    Default
};



class HandlerFactory
{
    public:
        bool Find( const wxString& path );
        bool Find( const wxString& path, HandlerType& type );
        Handler* NewHandler( const wxString& path );
        Handler* NewHandler( const HandlerType& type );
        Handler* NewHandler();
        HandlerType GetType();
    private:
        HandlerType m_type;
};

#endif