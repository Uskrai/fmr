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

#include "handler/handler_factory.h"

#include "handler/default_handler.h"
#include "handler/wxarchive_handler.h"

namespace fmr
{

HandlerType HandlerFactory::GetType()
{
    return m_type;
}

bool HandlerFactory::Is( const std::string& path1, const std::string& path2)
{
    HandlerType type1, type2;
    Find( path1, type1 );
    Find( path2, type2 );
    return type1 == type2;
}


bool HandlerFactory::Find( const std::string& path )
{
    HandlerType type;
    return Find( path, type );
}

bool HandlerFactory::Find( const std::string& path, HandlerType& type )
{
    type = kHandlerNotFound;
    if ( WxArchiveHandler::CanHandle(path) )
    {
        type = kHandlerWxArchive;
        return true;
    }

    return FindOpenable( path, type );
}

bool HandlerFactory::FindOpenable( const std::string &path )
{
    HandlerType type;
    return FindOpenable( path, type );
}

bool HandlerFactory::FindOpenable(  const std::string &path, HandlerType &type )
{
    if ( DefaultHandler::CanHandle( path ) )
    {
        type = kHandlerDefault;
        return true;
    }

    return false;
}

AbstractHandler* HandlerFactory::NewHandler( const std::string& path )
{
    AbstractHandler *handler = NULL;
    HandlerType type;
    if ( Find( path, type ) )
    {
        handler = NewHandler( type );
        handler->Open(path);

    }    
    return handler;
}

AbstractHandler* HandlerFactory::NewHandler( const HandlerType& type )
{
    switch ( type )
    {
        case kHandlerWxArchive:
            return new WxArchiveHandler();
        default:
            return NewOpenableHandler( type );
    }
}

AbstractOpenableHandler *HandlerFactory::NewOpenableHandler( const std::string &path )
{
    HandlerType type;
    AbstractOpenableHandler *handler;
    if ( Find( path, type ))
    {
        handler = NewOpenableHandler( type );
        handler->Open( path );
    }

    return handler;
}

AbstractOpenableHandler *HandlerFactory::NewOpenableHandler( const HandlerType &type )
{
    switch ( type )
    {
        case kHandlerDefault:
            return new DefaultHandler();

        default:
            return nullptr;
    }
    return nullptr;
}

AbstractHandler* HandlerFactory::NewHandler()
{
    return NewHandler( m_type );
}

bool HandlerFactory::IsOpenable( const std::string &path )
{
    HandlerType type, openable_type;
    Find( path, type );
    FindOpenable( path, openable_type );

    return type == openable_type;
}

}; // namespace fmr