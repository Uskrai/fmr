
#include <iostream>
#include <gtest/gtest.h>
#include "handler/std_handler.h"

namespace fmr
{

std::wstring test_path = L"test_dir";

TEST(STDHandlerTest, Size)
{
	std::filesystem::create_directories( test_path + L"/test1");
	std::filesystem::create_directories( test_path + L"/test2");
	STDHandler *handler = new STDHandler();
	handler->Open( test_path );
	SStream stream;
	bool cont = handler->GetFirst( stream, DIR_DEFAULT, true );
	size_t i = 0;
	while ( cont )
	{
		i++;
		cont = handler->GetNextFile( stream, true );
	}

	handler->Traverse();
	EXPECT_EQ( i, handler->Size() );
}

TEST( STDHandlerTest, Sort )
{
	STDHandler *handler = new STDHandler( test_path );
	handler->Traverse();
	EXPECT_EQ( handler->Item(0).GetString(), test_path + "/test1" );

	std::filesystem::remove_all( test_path );
}

} // namespace fmr

int main( int argc, char **argv )
{
	testing::InitGoogleTest( &argc, argv );
	return RUN_ALL_TESTS();
	//arcHandler();
}
