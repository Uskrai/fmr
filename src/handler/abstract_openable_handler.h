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

#ifndef FMR_HANDLER_ABSTRACT_OPENABLE_HANDLER
#define FMR_HANDLER_ABSTRACT_OPENABLE_HANDLER

#include "handler/abstract_handler.h"

namespace fmr
{

/**
 * @brief an abstract class that the child can be opened
 * 
 */

class AbstractOpenableHandler
    : public AbstractHandler
{
    public:
        /**
         * @brief Get the Item Path object
         * 
         * @param stream stream object
         * @return std::wstring the path of the stream's
         */
        virtual std::string GetItemPath( const SStream &stream ) const = 0;

        /**
         * @brief Get the Item Path object
         * 
         * @param index the index from Index() method
         * @return std::wstring the path of the stream pointed by index
         */
        virtual std::string GetItemPath( size_t index ) const = 0;
};

} // namespace fmr

#endif