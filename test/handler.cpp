
#include <iostream>
#include <gtest/gtest.h>
#include "handler/std_handler.h"
#include "handler/default_handler.h"
#include "handler/wxarchive_handler.h"

namespace fmr
{

std::wstring test_path = L"test_dir/";

void TestSizeHandler( AbstractHandler *handler, const std::wstring &path )
{
	std::filesystem::create_directories( path + L"test1" );
	std::filesystem::create_directories( path + L"test2" );

	handler->Open( path );

	SStream stream;

	bool cont = handler->GetFirst( stream, kDirDefault, false );

	size_t i = 0;

	while ( cont )
	{
		i++;
		cont = handler->GetNextStream( stream, false );
	}

	handler->Traverse();
	EXPECT_EQ( i, handler->Size() );
}

void TestSortHandler( DefaultHandler *handler, const std::wstring &path )
{
	handler->Open( path );
	handler->Traverse();
	EXPECT_EQ( handler->Item(0).GetString(), path + L"test1" );

	std::filesystem::remove_all( path );
}

TEST(STDHandlerTest, Size)
{
	std::filesystem::create_directories( test_path + L"test1");
	std::filesystem::create_directories( test_path + L"test2");
	STDHandler *handler = new STDHandler();
	TestSizeHandler( handler, test_path );
	delete handler;
}

TEST( STDHandlerTest, Sort )
{
	STDHandler *handler = new STDHandler( test_path );
	handler->Traverse();
	EXPECT_EQ( handler->Item(0).GetString(), L"test1" );

	std::filesystem::remove_all( test_path );
	delete handler;
}

template<typename T>
void TEST_STREAM( T *handler )
{
	SStream stream;
	bool cont = handler->GetFirst( stream, kDirDefault, true );

	SStream temp_stream( stream );
	temp_stream.GetStream();

	std::vector<SStream> vec_stream;
	size_t file = -1;

	while ( cont )
	{
		vec_stream.push_back( stream );

		// for checking copy stream
		if ( file != size_t(-1) )
		{
			vec_stream.at(file).GetStream();
			file++;
		}
		else file = 0;

		cont = handler->GetNextStream( stream, true );
	}
	if ( file != size_t(-1) )
		vec_stream.at(file).GetStream();

	handler->Traverse( true );

	EXPECT_EQ( vec_stream.size(), handler->Size() );

	// checking size
	for ( const auto &handler_it : handler->GetChild() )
		for ( const auto &vec_it : vec_stream )
			if ( handler_it.GetName() == vec_it.GetName() )
			{
				EXPECT_EQ( handler_it.GetSize(), vec_it.GetSize() );
			}
}

TEST( DefaultHandler, Stream )
{
	auto handler = new DefaultHandler("test");
	TEST_STREAM( handler );
	handler->Open("../test");
	handler->Clear();
	TEST_STREAM( handler );
	delete handler;
}

TEST( STDHandler, Stream )
{
	auto handler = new STDHandler("test");
	TEST_STREAM( handler );
	handler->Open("../test");
	handler->Clear();
	TEST_STREAM( handler );
	delete handler;

}

void TEST_WRITE( STDHandler *handler, const std::wstring path )
{
	size_t length = 1000;
	char *buffer = new char[length];
	SStream stream_buffer(buffer, length);
	SStream stream_file("Makefile");

	handler->RemoveAll();
	handler->CommitWrite();
	EXPECT_TRUE( handler->CreateDirectories() );
	handler->CreateDirectory(L"test1");
	handler->CreateFiles( L"owo", stream_buffer );
	handler->CreateFiles( L"wew", stream_file );
	EXPECT_TRUE( handler->CommitWrite() );
	handler->Reset();

	handler->Open( path );
	handler->Traverse( true );

	EXPECT_EQ( handler->Size(), 3 );

	size_t index = handler->Index( L"owo" );

	EXPECT_EQ( handler->Item( index ).GetSize(), length );

	handler->RemoveAll();
	handler->CommitWrite();
	handler->Clear();

	EXPECT_EQ( handler->Size(), 0 );

	EXPECT_FALSE( std::filesystem::exists( test_path ) );
	EXPECT_FALSE( handler->CommitWrite() );
	delete[] buffer;
}

TEST( STDHandlerTest, Write )
{
	STDHandler *handler = new STDHandler( test_path );
	TEST_WRITE( handler, test_path );
	delete handler;
}

// TEST( wxArchiveTest, Size )
// {
// 	WxArchiveHandler *handler = new WxArchiveHandler( "test_zip.zip" );
// 	SStream stream;
// 	bool cont = handler->GetFirst( stream, kDirDefault, false );
// 	size_t i = 0;

// 	while ( cont )
// 	{
// 		i++;
// 		cont = handler->GetNextStream( stream, false );
// 	}

// 	handler->Traverse();
// 	EXPECT_EQ( i, handler->Size() );
// }

// TEST( wxArchiveTest, Sort )
// {
// 	WxArchiveHandler *handler = new WxArchiveHandler( "test_zip.zip" );
// 	handler->Traverse();

// 	EXPECT_EQ( handler->Item(0).GetString(), L"test_zip/" );
// }

TEST( DefaultHandlerTest, Size )
{
	DefaultHandler *handler = new DefaultHandler();
	TestSizeHandler( handler, test_path );
	delete handler;
}

TEST( DefaultHandlerTest, Sort )
{
	DefaultHandler *handler = new DefaultHandler();
	TestSortHandler( handler, test_path );
	delete handler;
}

} // namespace fmr

int main( int argc, char **argv )
{
	testing::InitGoogleTest( &argc, argv );
	return RUN_ALL_TESTS();
	//arcHandler();
}
