
#include <iostream>
#include <gtest/gtest.h>
#include "handler/std_handler.h"
#include "handler/default_handler.h"
#include "handler/wxarchive_handler.h"
#include <wx/stopwatch.h>
#include <chrono>
#include <random>


#define FILE_SIZE_START 100
#define FILE_SIZE_INTERVAL 5
#define FILE_COUNT 100
#define FOLDER_COUNT 100

namespace fmr
{

std::wstring GetExtension( AbstractHandler * )
	{ return L"/"; }

std::wstring GetExtension( WxArchiveHandler * )
	{ return L".zip"; }

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

template<class T>
class HandlerTest : public ::testing::Test
{
	public:
		void SetUp() override
		{
			handler_ = new T();
			test_dir_ = L"test_dir" + GetExtension( handler_ );
			ASSERT_FALSE( handler_->CreateDirectories() );
			ASSERT_FALSE( handler_->RemoveAll() );

			ASSERT_TRUE( handler_->RemoveAll( test_dir_ ) );

			ASSERT_FALSE( handler_->CreateDirectories(L"") );
			ASSERT_FALSE( handler_->RemoveAll(L"") );

			ASSERT_TRUE( handler_->CreateDirectories( test_dir_ ) );
			ASSERT_TRUE( handler_->RemoveAll( test_dir_ ) );

			PrepareHandler();
		}

		std::shared_ptr<char[]> MakeBuffer( size_t size )
		{
			char *buffer = new char[size];
			return std::shared_ptr<char[]>( buffer );
		}

		time_t GetMicroSecond()
		{
			using namespace std::literals;

			auto now = std::chrono::system_clock::now();

			return std::chrono::system_clock::to_time_t( now );
		}

		SStream MakeStream( size_t size )
		{
			auto buffer_ = MakeBuffer( size );
			return SStream( buffer_.get(), size );
		}

		void PrepareHandler()
		{
			handler_->Open( test_dir_ );
			handler_->CreateDirectories();
			size_t stream_total_size = 0;


			size_t length = FILE_SIZE_START;
			while ( length < FILE_COUNT * FILE_SIZE_INTERVAL )
			{
				SStream item = MakeStream( length );
				std::wstring name = MakeName( 10 );
				stream_total_size += item.GetSize();

				ASSERT_TRUE( handler_->CreateFiles( item, name ) );

				length += FILE_SIZE_INTERVAL;
			}


			length = 0;
			while ( length < FOLDER_COUNT )
			{
				ASSERT_TRUE( handler_->CreateDirectory( MakeName( 10 ) ) );
				length++;
			}

			handler_->CommitWrite();

			handler_->Clear();
			handler_->Traverse( true );

			size_t traverse_total_size = 0;
			for ( const SStream &it : handler_->GetChild() )
				traverse_total_size += it.GetSize();

			ASSERT_EQ( stream_total_size, traverse_total_size );

			handler_->Clear();
		}

		void TearDown() override
		{
			ASSERT_TRUE( handler_->RemoveAll( test_dir_ ) );
			ASSERT_TRUE( !std::filesystem::exists( test_dir_ ) );
		}

		~HandlerTest( )
		{
			delete handler_;
		}

	protected:
		std::shared_ptr<char[]> buffer_ = NULL;
		T *handler_ = NULL;
		std::wstring test_dir_;
};


typedef testing::Types<STDHandler,WxArchiveHandler,DefaultHandler> handler_type;

TYPED_TEST_SUITE( HandlerTest, handler_type );

void TestSizeHandler( AbstractHandler *handler, const std::wstring &path )
{
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
	EXPECT_EQ( i, handler->Size() ) << "GetFirst's Output and Traverse's Output doesn't have the same size";
}

template<typename T>
void TEST_STREAM( T *handler, const std::wstring &path )
{
	handler->Clear();
	handler->Open( path );

	SStream stream;
	bool cont = handler->GetFirst( stream, kDirDefault, true );

	SStream temp_stream( stream );
	temp_stream.GetStream();

	std::vector<SStream> vec_stream;
	size_t file = 0;

	ASSERT_TRUE( cont ) << path << " should have some file or folder";
	while ( cont )
	{
		vec_stream.push_back( stream );
		cont = handler->GetNextStream( stream, true );

		// checking GetStream and copy ctor
		vec_stream.at(file).GetStream();
		file++;
	}

	handler->Traverse( true );

	ASSERT_EQ( vec_stream.size(), handler->Size() );

	// checking size
	for ( const SStream &handler_it : handler->GetChild() )
		for ( const SStream &vec_it : vec_stream )
			if ( handler_it.GetName() == vec_it.GetName() )
			{
				EXPECT_EQ( handler_it.GetSize(), vec_it.GetSize() );
			}
}

template<typename T>
void TEST_WRITE( T *handler, const std::wstring path )
{
	size_t length = 1000;
	char *buffer = new char[length];
	SStream stream_buffer(buffer, length);
	SStream stream_file("Makefile");

	ASSERT_TRUE( handler->RemoveAll() );
	ASSERT_FALSE( std::filesystem::exists( handler->GetName().ToStdWstring() ) );
	ASSERT_TRUE( handler->CreateDirectories() );

	ASSERT_TRUE( std::filesystem::exists( handler->GetName().ToStdWstring() ) );
	ASSERT_TRUE( handler->CommitWrite() );
	handler->CreateDirectory(L"test1");
	handler->CreateFiles( stream_buffer, L"owo" );
	handler->CreateFiles( stream_file, L"wew" );
	ASSERT_TRUE( handler->CommitWrite() );

	handler->Reset();

	handler->Open( path );
	handler->Traverse( true );

	ASSERT_EQ( handler->Size(), 3 );

	size_t index = handler->Index( L"owo" );

	ASSERT_EQ( handler->Item( index ).GetSize(), length );

	handler->RemoveAll();
	EXPECT_FALSE( handler->CommitWrite() );
	handler->Clear();

	ASSERT_EQ( handler->Size(), 0 );

	ASSERT_FALSE( std::filesystem::exists( path ) );
	delete[] buffer;
}

TYPED_TEST( HandlerTest, SizeTest )
{
	TestSizeHandler( this->handler_, this->test_dir_ );
}

TYPED_TEST( HandlerTest, WriteTest )
{
	TEST_WRITE( this->handler_, this->test_dir_ );
}

TYPED_TEST( HandlerTest, StreamTest )
{
	TEST_STREAM( this->handler_, this->test_dir_ );
}

TYPED_TEST( HandlerTest, OpenTest )
{
	this->handler_->Reset();
	SStream stream;
	ASSERT_FALSE( this->handler_->GetFirst( stream ) );
	ASSERT_FALSE( this->handler_->GetNextStream( stream ) );

	this->handler_->Reset();
	ASSERT_FALSE( this->handler_->GetNextStream( stream ) );
	ASSERT_FALSE( this->handler_->GetFirst( stream ) );
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
