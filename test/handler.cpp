
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
#define FILENAME_LENGTH 10
#define TEST_DIRECTORY "handler_test_dir"

namespace fmr
{

std::string GetExtension( AbstractHandler * )
	{ return "/"; }

std::string GetExtension( WxArchiveHandler * )
	{ return ".zip"; }

std::string MakeName( size_t size )
{
	std::random_device dev;
	std::mt19937 rng( dev() );
	std::uniform_int_distribution<> lower_char(97,122), upper_char(65,90);

	size_t i = 0;
	std::string string;
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

template<class T>
class HandlerTest : public ::testing::Test
{
	public:
		void SetUp() override
		{

		}

		static void PrepareHandler()
		{
			handler_->Reset();
			ASSERT_FALSE( handler_->CreateDirectories() );
			ASSERT_FALSE( handler_->RemoveAll() );

			ASSERT_TRUE( handler_->RemoveAll( test_dir_ ) );

			ASSERT_FALSE( handler_->CreateDirectories("") );
			ASSERT_FALSE( handler_->RemoveAll("") );

			ASSERT_TRUE( handler_->CreateDirectories( test_dir_ ) );
			ASSERT_TRUE( handler_->RemoveAll( test_dir_ ) );

			handler_->Open( test_dir_ );
			handler_->CreateDirectories();
			size_t stream_total_size = 0;


			size_t length = FILE_SIZE_START;
			while ( length < FILE_COUNT * FILE_SIZE_INTERVAL )
			{
				SStream item = MakeStream( length );
				std::string name = MakeName( FILENAME_LENGTH );
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

		T *CreateHandler()
		{
			T *handler = new T();
			return handler;
		}

		static void SetUpTestSuite()
		{
			TearDownTestSuite();
			handler_ = new T();
			test_dir_ = TEST_DIRECTORY + GetExtension( handler_ );
			PrepareHandler();
		}

		static void TearDownTestSuite()
		{
			if ( handler_ )
			{
				ASSERT_TRUE( handler_->RemoveAll( test_dir_ ) );
				ASSERT_TRUE( !std::filesystem::exists( test_dir_ ) );

				delete handler_;
				handler_ = NULL;
			}
		}

		void TearDown() override
		{
		}

	protected:
		std::shared_ptr<char[]> buffer_ = NULL;
		inline static T *handler_ = NULL;
		inline static std::string test_dir_;
};


typedef testing::Types<STDHandler,WxArchiveHandler,DefaultHandler> handler_type;

TYPED_TEST_SUITE( HandlerTest, handler_type );

#define HANDLER_TEST_FUNC( TestName ) \
	template<typename T> \
	void TestName ## Function( T *handler, std::string path, HandlerTest<T> *test_obj )

#define HANDLER_TEST_CALL_FUNC( TestName, Postfix, Func ) \
	TYPED_TEST( HandlerTest, TestName ## Postfix ) \
	{ \
		return TestName ## Function ( \
			this->handler_, \
			Func( this->test_dir_ ), \
			this \
		); \
	}

#define HANDLER_TEST( TestName )	\
	HANDLER_TEST_FUNC( TestName ); \
	HANDLER_TEST_CALL_FUNC( TestName, Absolute, Path::MakeAbsolute ); \
	HANDLER_TEST_CALL_FUNC( TestName, Relative, std::string ); \
	HANDLER_TEST_FUNC( TestName )

void TestSizeHandler( AbstractHandler *handler, const std::string &path )
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
void TEST_STREAM( T *handler, const std::string &path )
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
void TEST_WRITE( T *handler, const std::string path )
{
	size_t length = 1000;
	char *buffer = new char[length];
	SStream stream_buffer(buffer, length);
	SStream stream_file("Makefile");

	ASSERT_TRUE( handler->RemoveAll() );
	ASSERT_FALSE( std::filesystem::exists( handler->GetName() ) );
	ASSERT_TRUE( handler->CreateDirectories() );

	ASSERT_TRUE( std::filesystem::exists( handler->GetName() ) );
	ASSERT_TRUE( handler->CommitWrite() );
	handler->CreateDirectory("test1");
	handler->CreateFiles( stream_buffer, "owo" );
	handler->CreateFiles( stream_file, "wew" );
	ASSERT_TRUE( handler->CommitWrite() );

	handler->Reset();

	handler->Open( path );
	handler->Traverse( true );

	ASSERT_EQ( handler->Size(), 3 );

	size_t index = handler->Index( "owo" );

	ASSERT_EQ( handler->Item( index ).GetSize(), length );

	handler->RemoveAll();
	EXPECT_FALSE( handler->CommitWrite() );
	handler->Clear();

	ASSERT_EQ( handler->Size(), 0 );

	ASSERT_FALSE( std::filesystem::exists( path ) );
	delete[] buffer;
}

HANDLER_TEST( SizeTest )
{
	TestSizeHandler( handler, path );
}

HANDLER_TEST( WriteTest )
{
	TEST_WRITE( handler, path );
	test_obj->PrepareHandler();
}

HANDLER_TEST( StreamTest )
{
	TEST_STREAM( handler, path );
}

HANDLER_TEST( OpenTest )
{
	handler->Reset();
	SStream stream;
	ASSERT_FALSE( handler->GetFirst( stream ) );
	ASSERT_FALSE( handler->GetNextStream( stream ) );

	handler->Reset();
	ASSERT_FALSE( handler->GetNextStream( stream ) );
	ASSERT_FALSE( handler->GetFirst( stream ) );

	handler->Open( path );
}

HANDLER_TEST( TraverseTest )
{
	handler->Clear();

	handler->Traverse( );
	size_t size = handler->Size();
	handler->Traverse( );
	ASSERT_EQ( size, handler->Size() ) << "handler should be cleared before traversing";
}

HANDLER_TEST( ChangeFolderTest )
{
	ASSERT_TRUE( handler->GetParent() );
	handler->GetParent()->Traverse();
	std::string next_folder_path = handler->GetFromCurrent( 1 );
	ASSERT_NE( next_folder_path, "" );
}

} // namespace fmr

int main( int argc, char **argv )
{
	testing::InitGoogleTest( &argc, argv );
	return RUN_ALL_TESTS();
}
