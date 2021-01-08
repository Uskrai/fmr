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

#include "handler/abstract_handler.h"
#include "handler/abstract_openable_handler.h"

namespace fmr
{

enum HandlerType
{
    kHandlerWxArchive,
    kHandlerDefault,
    kHandlerNotFound
};

class HandlerFactory
{
    public:
        static bool Is( const std::string& path1, const std::string& path2);
        static bool Find( const std::string& path );
        static bool Find( const std::string& path, HandlerType& type );
        static AbstractHandler* NewHandler( const std::string& path );
        static AbstractHandler* NewHandler( const HandlerType& type );
        static AbstractOpenableHandler *NewOpenableHandler( const std::string &path );
        static AbstractOpenableHandler *NewOpenableHander( const HandlerType &type );
        AbstractHandler* NewHandler();
        HandlerType GetType();
    private:
        HandlerType m_type;
};

}; // namespace fmr
#endif