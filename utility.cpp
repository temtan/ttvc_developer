// utility.cpp

#include <algorithm>

#pragma warning(push)
#  pragma warning(disable : 4005)
#  pragma warning(disable : 4917)
#  include <Shlobj.h>
#pragma warning(pop)

#include "tt_string.h"
#include "tt_ini_file.h"
#include "tt_path.h"
#include "tt_exception.h"

#include "exception.h"
#include "project.h"

#include "utility.h"

using namespace TTVCDeveloper;


// -----------------------------------------------------------------------
std::string
Utility::GetDefaultProjectFilePath( void )
{
  return TtPath::GetExecutingDirectoryPath() + "\\default.tdp";
}

void
Utility::CreateDefaultProjectFile( void )
{
  Project project;

  project.Reset( Utility::GetDefaultProjectFilePath() );
  project.name_ = "デフォルトプロジェクト名";
  project.use_icon_ = false;
  project.icon_file_path_ = "";
  project.icon_file_index_ = 0;

  Project::Structure structure;
  structure.name_                     = "Debug";
  structure.output_directory_name_    = structure.name_;
  structure.target_name_              = "target.exe";
  structure.target_current_directory_ = "@@output_folder_path@@";
  structure.target_use_output_edit_   = true;
  structure.optimize_kind_            = Project::OptimizeKind::NoOptimize;
  project.GetStructures().push_back( std::make_shared<Project::Structure>( structure ) );

  structure.name_                     = "Release";
  structure.output_directory_name_    = structure.name_;
  structure.target_name_              = "target.exe";
  structure.target_current_directory_ = "@@output_folder_path@@";
  structure.target_use_output_edit_   = true;
  structure.optimize_kind_            = Project::OptimizeKind::Fastest;
  project.GetStructures().push_back( std::make_shared<Project::Structure>( structure ) );

  project.SaveToFileAsDefault();
}


std::string
Utility::TogglePathFullRelative( const std::string& path, const std::string& base_dir )
{
  if ( TtPath::IsRelative( path ) ) {
    return TtPath::Canonicalize( base_dir + "\\" + path );
  }
  auto result = TtPath::GetRelativePathTo( base_dir, true, path, false );
  return result ? result.value() : path;
}


bool
Utility::TouchFile( const std::string& path )
{
  FILE* file;
  errno_t error_number = ::fopen_s( &file, path.c_str(), "a" );
  if ( error_number != 0 ) {
    return false;
  }
  // コレを入れないと何故かエラー
  ::SetLastError( NO_ERROR );
  ::fclose( file );
  return true;
}


bool
Utility::MakeDirectoryOfFileIfNotExist( const std::string& path )
{
  std::string dir_path = TtPath::DirName( path );
  if ( NOT( TtPath::IsDirectory( dir_path ) ) ) {
    return ::SHCreateDirectoryEx( nullptr, dir_path.c_str(), nullptr ) == ERROR_SUCCESS;
  }
  return true;
}


void
Utility::SetSourceFileFilters( TtFileDialog& dialog )
{
  dialog.GetFilters().push_back( {"C++ソースファイル(*.cpp;*.cxx;*.cp;*.cc)", "*.cpp"} );
  dialog.GetFilters().back().GetExtensions().push_back( "*.cxx" );
  dialog.GetFilters().back().GetExtensions().push_back( "*.cp" );
  dialog.GetFilters().back().GetExtensions().push_back( "*.cc" );
  dialog.GetFilters().push_back( {"Cソースファイル(*.c)", "*.c"} );
  dialog.GetFilters().push_back( {"ヘッダファイル(*.h;*.hxx;*.hpp)", "*.h"} );
  dialog.GetFilters().back().GetExtensions().push_back( "*.hxx" );
  dialog.GetFilters().back().GetExtensions().push_back( "*.hpp" );
  dialog.GetFilters().push_back( {"リソーススクリプトファイル(*.rc)", "*.rc"} );
  dialog.GetFilters().push_back( {"すべてのファイル(*.*)", "*.*"} );
  dialog.SetFilterIndex( 5 );
}
