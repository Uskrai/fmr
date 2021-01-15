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

#include <fmr/common/compare.h>
#include <fmr/handler/struct_stream.h>

#include <gtest/gtest.h>
#include <algorithm>
#include <random>

namespace fmr
{

SStream GetStream( std::wstring name )
{
    SStream stream;
    stream.SetName( name );
    return stream;
}

std::wstring MakeName( size_t size )
{
	std::wstring path;
	std::random_device dev;
	std::mt19937 rng( dev() );
	std::uniform_int_distribution<> lower_char(97,122), upper_char(65,90);

	size_t i = 0;
	std::wstring string;
	while ( i < size )
	{
		if ( !(i & 2) )
			string += (wchar_t)lower_char(rng);
		else
			string += (wchar_t)upper_char( rng );
		i++;
	}
	return string;
}

TEST( SortTest, NaturalTest )
{
    std::vector<SStream> vec_string;


    for ( size_t i = 0; i < 100; i++ )
    {
        SStream stream;
        stream.SetName( std::wstring(L"testing") );
        vec_string.push_back( stream );
    }

    std::sort( vec_string.begin(), vec_string.end(), Compare::NaturalSortable );
}

} // namespace fmr


int main( int argc, char **argv)
{
    testing::InitGoogleTest( &argc, argv );
    return RUN_ALL_TESTS();
}
