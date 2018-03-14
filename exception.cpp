// exception.cpp

#include "tt_string.h"

#include "exception.h"

using namespace TTVCDeveloper;



// -- Exception --------------------------------------------------------
Exception::Exception( void ) :
TtException()
{
}

// -- InternalException ------------------------------------------------
InternalException::InternalException( const char* file, unsigned int line ) :
file_( file ),
line_( line )
{
}


const char*
InternalException::GetFile( void ) const
{
  return file_;
}

unsigned int
InternalException::GetLine( void ) const
{
  return line_;
}


std::string
InternalException::BasicMessage( void )
{
  return "内部エラーが発生しました。";
}

std::string
InternalException::Message( void )
{
  std::string tmp = this->BasicMessage();
  ( TtString::Appender( tmp ) << "\r\n" <<
    "ファイル : " << file_ << "\r\n" <<
    "行番号 : " << line_ );
  return tmp;
}


std::string
InternalException::Dump( void ) const
{
  std::ostringstream os;
  os << typeid( *this ).name() << " : " << file_ << "(" << line_ << ")";
  return os.str();
}


// -- FileNotExistException ----------------------------------------------
FileNotExistException::FileNotExistException( const std::string& path ) :
WithFilePathException( path )
{
}

std::string
FileNotExistException::BasicMessage( void )
{
  return "ファイルが存在しません。";
}

std::string
FileNotExistException::Message( void )
{
  std::string tmp = this->BasicMessage();
  tmp.append( "\r\n" );
  tmp.append( "ファイル : " + this->GetFilePath() );
  return tmp;
}

std::string
FileNotExistException::Dump( void ) const
{
  std::ostringstream os;
  os << typeid( *this ).name() << " : " << this->GetFilePath();
  return os.str();
}


// -- ProjectFileFormatException ---------------------------------------
ProjectFileFormatException::ProjectFileFormatException( const std::string& path, const std::string& key ) :
WithFilePathException( path ),
key_( key )
{
}

std::string
ProjectFileFormatException::BasicMessage( void )
{
  return "プロジェクトファイルの形式にエラーがありました。";
}

std::string
ProjectFileFormatException::Message( void )
{
  std::string tmp = this->BasicMessage();
  tmp.append( "\r\n" );
  tmp.append( "ファイル : " + this->GetFilePath() );
  tmp.append( "\r\n" );
  tmp.append( "キー : " + key_ );
  return tmp;
}

const std::string&
ProjectFileFormatException::GetKey( void ) const
{
  return key_;
}

std::string
ProjectFileFormatException::Dump( void ) const
{
  std::ostringstream os;
  os << typeid( *this ).name() << " : " << this->GetFilePath() << "(" << key_ << ")";
  return os.str();
}


// -- ProcessCreateException ---------------------------------------------
ProcessCreateException::ProcessCreateException( TtProcess::CreateInfo info, unsigned int error_number ) :
info_( info ),
error_number_( error_number )
{
}

std::string
ProcessCreateException::BasicMessage( void )
{
  return "実行ファイルの起動に失敗しました。";
}

std::string
ProcessCreateException::Message( void )
{
  std::string tmp = this->BasicMessage();
  tmp.append( "\r\n" );
  tmp.append( "実行ファイル : " + info_.filename_ );
  tmp.append( "\r\n" );
  tmp.append( "エラーメッセージ : " + TtUtility::GetWindowsSystemErrorMessage( error_number_ ) );
  return tmp;

}

TtProcess::CreateInfo&
ProcessCreateException::GetInfo( void )
{
  return info_;
}

unsigned int
ProcessCreateException::GetErrorNumber( void )
{
  return error_number_;
}

std::string
ProcessCreateException::Dump( void ) const
{
  std::ostringstream os;
  os << typeid( *this ).name() << " : " << info_.filename_ << "(" << error_number_ << ")";
  return os.str();
}


// -- MakeFileCreateException --------------------------------------------
MakeFileCreateException::MakeFileCreateException( const std::string& reason ) :
reason_( reason )
{
}

std::string
MakeFileCreateException::BasicMessage( void )
{
  return "メイクファイルの作成に失敗しました。";
}

std::string
MakeFileCreateException::Message( void )
{
  std::string tmp = this->BasicMessage();
  tmp.append( "\r\n" );
  tmp.append( reason_ );
  return tmp;
}

std::string
MakeFileCreateException::Dump( void ) const
{
  std::ostringstream os;
  os << typeid( *this ).name() << " : " << reason_;
  return os.str();
}
