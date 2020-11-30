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

#include "handler/handlerfactory.h"

#include "handler/defaulthandler.h"
#include "handler/archivehandler.h"

HandlerType HandlerFactory::GetType()
{
    return m_type;
}

bool HandlerFactory::Is( const wxString& path1, const wxString& path2)
{
    HandlerType type1, type2;
    Find( path1, type1 );
    Find( path2, type2 );
    return type1 == type2;
}


bool HandlerFactory::Find( const wxString& path )
{
    return Find( path, m_type );
}

bool HandlerFactory::Find( const wxString& path, HandlerType& type )
{
    if ( ArchiveHandler::CanHandle(path) )
    {
        m_type = Archive;
        return true;
    }

    if ( DefaultHandler::CanHandle(path) )
    {
        m_type = Default;
        return true;
    }
    return false;
}

Handler* HandlerFactory::NewHandler( const wxString& path )
{
    HandlerType type;
    if ( Find( path, type ) )
    {
        return NewHandler( type );
    }    
    return NULL;
}

Handler* HandlerFactory::NewHandler( const HandlerType& type )
{
    switch ( type )
    {
        case Archive:
            return new ArchiveHandler();
        case Default:
            return new DefaultHandler();
        default:
            return NULL;
    }
}

Handler* HandlerFactory::NewHandler()
{
    return NewHandler( m_type );
}
