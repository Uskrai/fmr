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

#ifndef FMR_NOWIDE_INCLUDE
#define FMR_NOWIDE_INCLUDE

#if HAVE_BOOST_FILESYSTEM

#if __has_include(<nowide/filesystem.hpp>)
#include <nowide/filesystem.hpp>
const std::locale locale = nowide::nowide_filesystem();
#else
#error "Using boost::filesystem but boost::nowide doesnt exist"
#endif

namespace fmr {

namespace nwd {

#include <fmr/nowide/using-inc.h>

}  // namespace nwd

}  // namespace fmr

#endif  // end of HAVE BOOST_FILESYSTEM_HPP

#endif /* end of include guard: FMR_NOWIDE_INCLUDE */
