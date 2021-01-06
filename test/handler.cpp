
#include <iostream>
#include <gtest/gtest.h>
#include "handler/std_handler.h"
#include "handler/default_handler.h"
#include "handler/wxarchive_handler.h"
#include <wx/stopwatch.h>

namespace fmr
{

std::wstring GetExtension( AbstractHandler * )
	{ return L"/"; }

std::wstring GetExtension( WxArchiveHandler * )
	{ return L".zip"; }

template<class T>
class HandlerTest : public ::testing::Test
{
	public:
		void SetUp() override
		{
			handler_ = new T();
			test_dir_ = L"test_dir" + GetExtension( handler_ );
			ASSERT_FALSE( handler_->CreateDirectories() );
			ASSERT_FALSE( handler_->RemoveAll( test_dir_ ) );
		}

		~HandlerTest( )
		{
			handler_->Open( test_dir_ );
			handler_->RemoveAll();
			delete handler_;
		}

	protected:
		T *handler_ = NULL;
		std::wstring test_dir_;
};


typedef testing::Types<STDHandler,DefaultHandler,WxArchiveHandler> handler_type;

TYPED_TEST_SUITE( HandlerTest, handler_type );

void TestSizeHandler( AbstractHandler *handler, const std::wstring &path )
{
	// std::filesystem::create_directories( path + L"test1" );
	// std::filesystem::create_directories( path + L"test2" );

	// handler->Open( path );

	// SStream stream;

	// bool cont = handler->GetFirst( stream, kDirDefault, false );

	// size_t i = 0;

	// while ( cont )
	// {
	// 	i++;
	// 	cont = handler->GetNextStream( stream, false );
	// }

	// handler->Traverse();
	// EXPECT_EQ( i, handler->Size() );
}

void TestSortHandler( AbstractHandler *handler, const std::wstring &path )
{
	// std::filesystem::create_directories( path + L"test1" );
	// std::filesystem::create_directories( path + L"test2" );

	// handler->Open( path );
	// handler->Traverse();
	// EXPECT_EQ( handler->Item(0).GetString(), L"test1" );

	// std::filesystem::remove_all( path );
}

template<typename T>
void TEST_STREAM( T *handler, const std::wstring &path )
{
	// handler->Clear();

	// SStream stream;
	// bool cont = handler->GetFirst( stream, kDirDefault, true );

	// SStream temp_stream( stream );
	// temp_stream.GetStream();

	// std::vector<SStream> vec_stream;
	// size_t file = 0;

	// ASSERT_TRUE( cont ) << path << " should have some file or folder";
	// while ( cont )
	// {
	// 	vec_stream.push_back( stream );
	// 	cont = handler->GetNextStream( stream, true );

	// 	// checking GetStream and copy ctor
	// 	vec_stream.at(file).GetStream();
	// 	file++;
	// }

	// handler->Traverse( true );

	// ASSERT_EQ( vec_stream.size(), handler->Size() );

	// // checking size
	// for ( const SStream &handler_it : handler->GetChild() )
	// 	for ( const SStream &vec_it : vec_stream )
	// 		if ( handler_it.GetName() == vec_it.GetName() )
	// 		{
	// 			EXPECT_EQ( handler_it.GetSize(), vec_it.GetSize() );
	// 		}
}

template<typename T>
void TEST_WRITE( T *handler, const std::wstring path )
{
	// std::wcout << path << '\n';
	// size_t length = 1000;
	// char *buffer = new char[length];
	// SStream stream_buffer(buffer, length);
	// SStream stream_file("Makefile");

	// ASSERT_FALSE( handler->RemoveAll() );
	// ASSERT_TRUE( handler->CreateDirectories() );
	// ASSERT_TRUE( std::filesystem::exists( handler->GetName().ToStdWstring() ) );
	// ASSERT_TRUE( handler->CommitWrite() );
	// handler->CreateDirectory(L"test1");
	// handler->CreateFiles( stream_buffer, L"owo" );
	// handler->CreateFiles( stream_file, L"wew" );
	// ASSERT_TRUE( handler->CommitWrite() );

	// handler->Reset();

	// handler->Open( path );
	// handler->Traverse( true );

	// ASSERT_EQ( handler->Size(), 3 );

	// size_t index = handler->Index( L"owo" );

	// ASSERT_EQ( handler->Item( index ).GetSize(), length );

	// handler->RemoveAll();
	// EXPECT_FALSE( handler->CommitWrite() );
	// handler->Clear();

	// ASSERT_EQ( handler->Size(), 0 );

	// ASSERT_FALSE( std::filesystem::exists( path ) );
	// delete[] buffer;
}

TYPED_TEST( HandlerTest, Sort )
{
	TestSortHandler( this->handler_, this->test_dir_ );
}

TYPED_TEST( HandlerTest, Size )
{
	TestSizeHandler( this->handler_, this->test_dir_ );
}

TYPED_TEST( HandlerTest, Write )
{
	printf("weweaesds\n\n");
	TEST_WRITE( this->handler_, this->test_dir_ );
}

TYPED_TEST( HandlerTest, Stream )
{
	TEST_STREAM( this->handler_, this->test_dir_ );
}

// TEST( STDHandlerTest, Sort )
// {
// 	STDHandler handler = STDHandler( test_path );
// 	TestSortHandler( &handler, test_path );
// }

// TEST(STDHandlerTest, Size)
// {
// 	STDHandler handler = STDHandler();
// 	TestSizeHandler( &handler, test_path );
// }


// TEST( STDHandlerTest, Write )
// {
// 	STDHandler *handler = new STDHandler( test_path );
// 	TEST_WRITE( handler, test_path );
// 	delete handler;
// }

// TEST( STDHandler, Stream )
// {
// 	auto handler = STDHandler("test");
// 	TEST_STREAM( &handler, L"test" );
// }

// TEST( WxArchiveHandler, Write )
// {
// 	std::wstring path = L"test_dir.zip";
// 	auto handler = WxArchiveHandler( path );
// 	TEST_WRITE( &handler, path );
// }

// TEST( WxArchiveHandler, Stream )
// {
// 	WxArchiveHandler handler = WxArchiveHandler( L"test_dir.zip" );
// 	char buffer[1000];
// 	handler.CreateDirectories();
// 	handler.CreateDirectory(L"owo");
// 	handler.CreateFiles( SStream( &buffer, 1000 ), L"wewew" );
// 	handler.CommitWrite();
// 	TEST_STREAM( &handler, L"test_dir.zip" );
// 	handler.RemoveAll();
// }

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

} // namespace fmr

int main( int argc, char **argv )
{
	testing::InitGoogleTest( &argc, argv );
	return RUN_ALL_TESTS();
}
