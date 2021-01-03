
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

void TestSortHandler( AbstractHandler *handler, const std::wstring &path )
{
	std::filesystem::create_directories( path + L"test1" );
	std::filesystem::create_directories( path + L"test2" );

	handler->Open( path );
	handler->Traverse();
	EXPECT_EQ( handler->Item(0).GetString(), L"test1" );

	std::filesystem::remove_all( path );
}

TEST(STDHandlerTest, Size)
{
	STDHandler handler = STDHandler();
	TestSizeHandler( &handler, test_path );
}

TEST( STDHandlerTest, Sort )
{
	STDHandler handler = STDHandler( test_path );
	TestSortHandler( &handler, test_path );
}

template<typename T>
void TEST_STREAM( T *handler, const std::wstring &path )
{
	handler->Clear();

	SStream stream;
	bool cont = handler->GetFirst( stream, kDirDefault, true );

	SStream temp_stream( stream );
	temp_stream.GetStream();

	std::vector<SStream> vec_stream;
	size_t file = 0;

	while ( cont )
	{
		vec_stream.push_back( stream );
		cont = handler->GetNextStream( stream, true );

		// checking GetStream and copy ctor
		vec_stream.at(file).GetStream();
		file++;
	}

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
	auto handler = DefaultHandler("test");
	TEST_STREAM( &handler, L"test" );
	TEST_STREAM( &handler, L"../test" );
}

TEST( STDHandler, Stream )
{
	auto handler = STDHandler("test");
	TEST_STREAM( &handler, L"test" );
	TEST_STREAM( &handler, L"../test" );

}

template<typename T>
void TEST_WRITE( T *handler, const std::wstring path )
{
	size_t length = 1000;
	char *buffer = new char[length];
	SStream stream_buffer(buffer, length);
	SStream stream_file("Makefile");

	handler->RemoveAll();
	handler->CommitWrite();
	EXPECT_TRUE( handler->CreateDirectories() );
	EXPECT_TRUE( std::filesystem::exists( handler->GetName().ToStdWstring() ) );
	handler->CreateDirectory(L"test1");
	handler->CreateFiles( stream_buffer, L"owo" );
	handler->CreateFiles( stream_file, L"wew" );
	EXPECT_TRUE( handler->CommitWrite() );
	handler->Reset();

	handler->Open( path );
	handler->Traverse( true );

	EXPECT_EQ( handler->Size(), 3 );

	size_t index = handler->Index( L"owo" );

	EXPECT_EQ( handler->Item( index ).GetSize(), length );

	handler->RemoveAll();
	EXPECT_FALSE( handler->CommitWrite() );
	handler->Clear();

	EXPECT_EQ( handler->Size(), 0 );

	EXPECT_FALSE( std::filesystem::exists( test_path ) );
	delete[] buffer;
}

TEST( DefaultHandler, Write )
{
	DefaultHandler handler = DefaultHandler( test_path );
	TEST_WRITE( &handler, test_path );
}

TEST( STDHandlerTest, Write )
{
	STDHandler *handler = new STDHandler( test_path );
	TEST_WRITE( handler, test_path );
	delete handler;
}

TEST( WxArchiveHandler, Write )
{
	std::wstring path = L"test_dir.zip";
	auto handler = WxArchiveHandler( path );
	TEST_WRITE( &handler, path );
}

TEST( WxArchiveHandler, Stream )
{
	WxArchiveHandler handler = WxArchiveHandler();
	char buffer[1000];
	handler.CreateDirectories();
	handler.CreateDirectory(L"owo");
	handler.CreateFiles( SStream( &buffer, 1000 ), L"wewew" );
	TEST_STREAM( &handler, L"test_dir.zip" );
	handler.RemoveAll();
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
	DefaultHandler handler = DefaultHandler();
	TestSizeHandler( &handler, test_path );
}

TEST( DefaultHandlerTest, Sort )
{
	DefaultHandler handler = DefaultHandler();
	TestSortHandler( &handler, test_path );
}

} // namespace fmr

int main( int argc, char **argv )
{
	testing::InitGoogleTest( &argc, argv );
	return RUN_ALL_TESTS();
}
