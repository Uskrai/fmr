
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
	handler->Open( test_path );
	SStream stream;
	bool cont = handler->GetFirst( stream, kDirDefault, true );
	size_t i = 0;
	while ( cont )
	{
		i++;
		cont = handler->GetNextStream( stream, true );
	}

	handler->Traverse();
	EXPECT_EQ( i, handler->Size() );
}

TEST( STDHandlerTest, Sort )
{
	STDHandler *handler = new STDHandler( test_path );
	handler->Traverse();
	EXPECT_EQ( handler->Item(0).GetString(), test_path + "test1" );

	std::filesystem::remove_all( test_path );
}

template<typename T>
void TEST_WRITE( T *handler, const std::wstring path )
{
	SStream stream("Makefile");
	handler->CommitWrite();
	handler->MakeDir(L"test1");
	handler->MakeFile( L"owo", stream );
	handler->CommitWrite();
	handler->Reset();

	handler->Open( test_path );
	handler->Traverse( true );

	EXPECT_EQ( handler->Size(), 2 );

	size_t index = handler->Index( handler->GetName() + L"owo" );

	EXPECT_EQ( handler->Item( index ).GetSize(), stream.GetSize() );
	EXPECT_EQ( handler->Item( index ).GetString(), std::wstring( path + L"owo" ) );

	handler->RemoveAll();
	handler->CommitWrite();
	handler->CommitWrite();
	handler->Clear();
	handler->Traverse();

	EXPECT_EQ( handler->Size(), 0 );

	handler->CommitWrite();
}

TEST( STDHandlerTest, Write )
{
	STDHandler *handler = new STDHandler( test_path );
	TEST_WRITE( handler, test_path );
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
}

TEST( DefaultHandlerTest, Sort )
{
	DefaultHandler *handler = new DefaultHandler();
	TestSortHandler( handler, test_path );
}

} // namespace fmr

int main( int argc, char **argv )
{
	testing::InitGoogleTest( &argc, argv );
	return RUN_ALL_TESTS();
	//arcHandler();
}
