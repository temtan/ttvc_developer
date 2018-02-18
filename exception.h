// exception.h

#pragma once

#include "tt_exception.h"
#include "tt_process.h"


namespace TTVCDeveloper {
  // ëΩèdåpè≥óp
  // FileName ï€éùóp
  class WithFilePathException {
  protected:
    explicit WithFilePathException( const std::string& path ) : path_( path ) {}
  public:
    const std::string& GetFilePath( void ) const { return path_; }
  private:
    const std::string path_;
  };


  // -- Exception --------------------------------------------------------
  class Exception : public TtException {
  public:
    explicit Exception( void );

    virtual std::string BasicMessage( void ) = 0;
    virtual std::string Message( void ) = 0;
  };

  // -- InternalException ------------------------------------------------
  class InternalException : public Exception {
  public:
    explicit InternalException( const char* file, unsigned int line );

    const char*  GetFile( void ) const;
    unsigned int GetLine( void ) const;

    virtual std::string BasicMessage( void ) override;
    virtual std::string Message( void ) override;

    virtual std::string Dump( void ) const override;

  private:
    const char*  file_;
    unsigned int line_;
  };
#define TTVC_DEVELOPER_INTERNAL_EXCEPTION TTVCDeveloper::InternalException( __FILE__, __LINE__ )

  // -- FileNotExistException --------------------------------------------
  class FileNotExistException : public Exception,
                                public WithFilePathException {
  public:
    explicit FileNotExistException( const std::string& path );

    virtual std::string BasicMessage( void ) override;
    virtual std::string Message( void ) override;

    virtual std::string Dump( void ) const override;
  };

  // -- ProjectFileFormatException ---------------------------------------
  class ProjectFileFormatException : public Exception,
                                     public WithFilePathException {
  public:
    explicit ProjectFileFormatException( const std::string& path, const std::string& key );

    virtual std::string BasicMessage( void ) override;
    virtual std::string Message( void ) override;

    const std::string& GetKey( void ) const;

    virtual std::string Dump( void ) const override;

  private:
    std::string key_;
  };

  // -- ProcessCreateException -------------------------------------------
  class ProcessCreateException : public Exception {
  public:
    explicit ProcessCreateException( TtProcess::CreateInfo info, unsigned int error_number );

    virtual std::string BasicMessage( void ) override;
    virtual std::string Message( void ) override;

    TtProcess::CreateInfo& GetInfo( void );
    unsigned int GetErrorNumber( void );

    virtual std::string Dump( void ) const override;

  private:
    TtProcess::CreateInfo info_;
    unsigned int          error_number_;
  };

  // -- MakeFileCreateException ------------------------------------------
  class MakeFileCreateException : public Exception {
  public:
    explicit MakeFileCreateException( const std::string& reason );

    virtual std::string BasicMessage( void ) override;
    virtual std::string Message( void ) override;

    virtual std::string Dump( void ) const override;

  private:
    std::string reason_;
  };
}
