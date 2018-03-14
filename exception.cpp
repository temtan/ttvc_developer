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
  return "�����G���[���������܂����B";
}

std::string
InternalException::Message( void )
{
  std::string tmp = this->BasicMessage();
  ( TtString::Appender( tmp ) << "\r\n" <<
    "�t�@�C�� : " << file_ << "\r\n" <<
    "�s�ԍ� : " << line_ );
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
  return "�t�@�C�������݂��܂���B";
}

std::string
FileNotExistException::Message( void )
{
  std::string tmp = this->BasicMessage();
  tmp.append( "\r\n" );
  tmp.append( "�t�@�C�� : " + this->GetFilePath() );
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
  return "�v���W�F�N�g�t�@�C���̌`���ɃG���[������܂����B";
}

std::string
ProjectFileFormatException::Message( void )
{
  std::string tmp = this->BasicMessage();
  tmp.append( "\r\n" );
  tmp.append( "�t�@�C�� : " + this->GetFilePath() );
  tmp.append( "\r\n" );
  tmp.append( "�L�[ : " + key_ );
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
  return "���s�t�@�C���̋N���Ɏ��s���܂����B";
}

std::string
ProcessCreateException::Message( void )
{
  std::string tmp = this->BasicMessage();
  tmp.append( "\r\n" );
  tmp.append( "���s�t�@�C�� : " + info_.filename_ );
  tmp.append( "\r\n" );
  tmp.append( "�G���[���b�Z�[�W : " + TtUtility::GetWindowsSystemErrorMessage( error_number_ ) );
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
  return "���C�N�t�@�C���̍쐬�Ɏ��s���܂����B";
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
