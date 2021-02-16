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

#ifndef FMR_NOWIDE_FILESYSTEM
#define FMR_NOWIDE_FILESYSTEM

#include <fmr/nowide/include.h>

#ifdef FMR_USE_BOOST_FILESYSTEM
#define BOOST_FILESYSTEM_NO_DEPRECATED
#include <boost/filesystem.hpp>
#else
#include <filesystem>
#endif

#include <fmr/nowide/using-inc.h>

namespace fmr {

namespace nwd {

#ifdef FMR_USE_BOOST_FILESYSTEM
namespace fs = boost::filesystem;
#else
namespace fs = filesystem;
#endif

}  // namespace nwd

}  // namespace fmr

#endif /* end of include guard: FMR_NOWIDE_FILESYSTEM */
