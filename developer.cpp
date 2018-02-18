// developer.cpp

#pragma warning(push)
#  pragma warning(disable : 4005)
#  pragma warning(disable : 4917)
#  include <Shlobj.h>
#pragma warning(pop)
#pragma comment(lib, "Shell32.lib")

#include "ttl_define.h"
#include "tt_text_template.h"
#include "tt_path.h"

#include "exception.h"
#include "utility_dialogs.h"
#include "utility.h"

#include "developer.h"

using namespace TTVCDeveloper;


#include "makefile_template.dat"


// -- Developer ----------------------------------------------------------
Developer::Developer( void ) :
settings_(),
project_(),
current_structure_( nullptr )
{
}

bool
Developer::IsOperable( void )
{
  if ( NOT( current_structure_ ) ) {
    if ( project_.GetStructures().size() == 0 ) {
      return false;
    }
    this->SetCurrentStructureByIndex( 0 );
  }
  return true;
}

Settings&
Developer::GetSettings( void )
{
  return settings_;
}

Project&
Developer::GetProject( void )
{
  return project_;
}


void
Developer::SetCurrentStructure( Project::Structure* structure )
{
  current_structure_ = structure;
}

void
Developer::SetCurrentStructureByIndex( int index )
{
  current_structure_ = project_.GetStructures().at( index ).get();
}


Project::Structure&
Developer::GetCurrentStructure( void )
{
  if ( NOT( this->IsOperable() ) ) {
    throw TTVC_DEVELOPER_INTERNAL_EXCEPTION;
  }
  return *current_structure_;
}


Settings::Compiler&
Developer::GetCurrentCompilerSettings( void )
{
  return settings_.GetCompilerByPlatform( this->GetCurrentStructure().platform_ );
}


std::string
Developer::GetProjectPath( void )
{
  return project_.path_;
}

std::string
Developer::GetProjectDirectoryPath( void )
{
  return TtPath::DirName( this->GetProjectPath() );
}

std::string
Developer::GetOutputDirectoryPath( void )
{
  return this->GetProjectDirectoryPath() + "\\" + current_structure_->output_directory_name_;
}

std::string
Developer::GetPathOfOutputDirectory( const std::string& filename )
{
  return this->GetOutputDirectoryPath() + "\\" + filename;
}

std::string
Developer::GetMakefilePath( void )
{
  return this->GetPathOfOutputDirectory( "Makefile" );
}

std::string
Developer::GetProjectFilesPathEnumerationString( void )
{
  return this->GetProjectFilesPathEnumerationStringOf( [] ( Project::File& ) { return true; } );
}

std::string
Developer::GetProjectFilesPathEnumerationStringOf( std::function<bool ( Project::File& file )> function )
{
  std::string tmp;
  bool first = true;
  for ( auto& file : project_.GetFiles() ) {
    if ( NOT( first ) ) {
      tmp.append( " " );
    }
    if ( function( *file ) ) {
      tmp.append( "\"" + file->GetPath() + "\"" );
      first = false;
    }
  }
  return tmp;
}

Developer::FileSelectFunction
Developer::HeaderFileSelect = [] ( Project::File& file ) -> bool {
  return file.TypeIs<Project::HeaderFile>();
};

Developer::FileSelectFunction
Developer::SourceFileSelect = [] ( Project::File& file ) -> bool {
  return file.TypeIs<Project::SourceFile>();
};

Developer::FileSelectFunction
Developer::HeaderSourceFileSelect = [] ( Project::File& file ) -> bool {
  return Developer::HeaderFileSelect( file ) || Developer::SourceFileSelect( file );
};


std::string
Developer::GetTargetFilePath( void )
{
  return this->GetPathOfOutputDirectory( current_structure_->target_name_ );
}


void
Developer::CreateMakefile( SquirrelVM& vm )
{
  TtTextTemplate::Document document;
  document.ParseText( MAKEFILE_TEMPLATE );

  document["application_name"] = APPLICATION_NAME;
  document["application_version"] = VERSION;

  document["make_command_directory"] = TtPath::DirName( this->GetCurrentCompilerSettings().make_command_path_ );

  document["compiler_path"] = this->GetCurrentCompilerSettings().compiler_path_;
  if ( current_structure_->target_kind_ == Project::TargetKind::StaticLibrary ) {
    document["linker_path"] = this->GetCurrentCompilerSettings().GetLibCommandPath();
  }
  else {
    document["linker_path"] = this->GetCurrentCompilerSettings().GetLinkerCommandPath();
  }
  for ( const std::string& include_path : current_structure_->include_paths_ ) {
    document["includes"] = [&include_path] ( TtTextTemplate::InternalDocument& tmp ) {
      tmp["include"] = include_path;
    };
  }
  for ( const std::string& define : current_structure_->defines_ ) {
    document["defines"] = [&define] ( TtTextTemplate::InternalDocument& tmp ) {
      tmp["define"] = define;
    };
  }
  for ( const int no_warning : current_structure_->no_warnings_ ) {
    document["no_warning"] = [&no_warning] ( TtTextTemplate::InternalDocument& tmp ) {
      tmp["number"] = TtUtility::ToStringFrom( no_warning );
    };
  }
  {
    std::string tmp;
    auto append = [&tmp] ( const std::string& str ) {
      tmp.append( " " + str );
    };
    auto append_if = [&tmp, &append] ( bool flag, const std::string& str ) {
      if ( flag ) {
        append( str );
      }
    };

    append( std::unordered_map<Project::OptimizeKind, std::string>( {
      {Project::OptimizeKind::NoSpecify,  ""},
      {Project::OptimizeKind::NoOptimize, "/Od"},
      {Project::OptimizeKind::Smallest,   "/O1"},
      {Project::OptimizeKind::Fastest,    "/O2"},
      {Project::OptimizeKind::Full,       "/Ox"},
    } )[current_structure_->optimize_kind_] );
    append( std::unordered_map<Project::RuntimeLibrary, std::string>( {
      {Project::RuntimeLibrary::Static,       "/MT"},
      {Project::RuntimeLibrary::StaticDebug,  "/MTd"},
      {Project::RuntimeLibrary::Dynamic,      "/MD"},
      {Project::RuntimeLibrary::DynamicDebug, "/MDd"},
    } )[current_structure_->runtime_library_] );
    append_if( current_structure_->use_exception_, "/EHsc" );

    append_if( current_structure_->warning_level_ == 5, "/Wall" );
    append_if( current_structure_->warning_level_ != 5, "/W" + TtUtility::ToStringFrom( current_structure_->warning_level_ ) );
    append_if( current_structure_->error_if_warning_, "/WX" );

    append_if( current_structure_->no_logo_, "/nologo" );
    append( current_structure_->compiler_other_option_ );
    document["compiler_flags"] = tmp;
  }

  document["environment_variable_include"] = this->GetCurrentCompilerSettings().environment_variable_include_;
  document["environment_variable_lib"]     = this->GetCurrentCompilerSettings().environment_variable_lib_;
  document["environment_variable_libpath"] = this->GetCurrentCompilerSettings().environment_variable_libpath_;

  document["resource_compiler_path"] = this->GetCurrentCompilerSettings().resource_compiler_path_;
  {
    std::string tmp;
    if ( current_structure_->no_logo_ ) {
      tmp.append( "/nologo" );
    }
    tmp.append( " " + current_structure_->resource_compiler_option_ );
    document["resource_compiler_flags"] = tmp;
  }

  document["target"] = this->GetTargetFilePath();

  try {
    auto it = std::unordered_map<Project::TargetKind, std::string>( {
      {Project::TargetKind::Windows,            "/SUBSYSTEM:WINDOWS"},
      {Project::TargetKind::Console,            "/SUBSYSTEM:CONSOLE"},
      {Project::TargetKind::DynamicLinkLibrary, "/DLL"},
      {Project::TargetKind::StaticLibrary,      ""},
    } ).at( current_structure_->target_kind_.ToValue() );
    document["sub_system"] = it;
  } catch ( std::out_of_range ) {
    throw TTVC_DEVELOPER_INTERNAL_EXCEPTION;
  }

  project_.EachFileWithIndex( [&document, this] ( Project::File& file, int index ) {
    int number = index + 1;
    const std::string output_directory_path = current_structure_->output_directory_name_;
    if ( file.TypeIs<Project::SourceFile>() ) {
      document["files"] = [&] ( TtTextTemplate::InternalDocument& tmp ) {
        tmp["number"] = TtUtility::ToStringFrom( number );
        tmp["source_path"] = file.GetPath();
        std::string object_path = dynamic_cast<Project::SourceFile&>( file ).GetObjectFilePath( output_directory_path );
        tmp["object_path"] = object_path;
        std::string object_full_path = this->GetProjectDirectoryPath() + "\\" + object_path;
        if ( NOT( Utility::MakeDirectoryOfFileIfNotExist( object_full_path ) ) ) {
          throw MakeFileCreateException( "ディレクトリの作成に失敗しました。\r\nディレクトリ : " + TtPath::DirName( object_full_path ) );
        }
      };
      document["obj_all"] = [&] ( TtTextTemplate::InternalDocument& tmp ) {
        tmp["number"] = TtUtility::ToStringFrom( number );
      };

      if ( file.TypeIs<Project::ResourceScriptFile>() ) {
        document["resource_compile"] = [&] ( TtTextTemplate::InternalDocument& tmp ) {
          tmp["number"] = TtUtility::ToStringFrom( number );
        };
      }
      else if ( current_structure_->use_precompile_header_ && file.GetPath() == current_structure_->precompiled_header_source_ ) {
        document["precompiled_header_source_object"] = [&] ( TtTextTemplate::InternalDocument& tmp ) {
          tmp["number"] = TtUtility::ToStringFrom( number );
        };
      }
      else {
        document["file_compile"] = [&] ( TtTextTemplate::InternalDocument& tmp ) {
          tmp["number"] = TtUtility::ToStringFrom( number );
          tmp["precompiled_header_options"] = current_structure_->use_precompile_header_;
        };
      }
    }
  } );

  {
    int index = 0;
    for ( const std::string& library_path : current_structure_->library_paths_ ) {
      document["libs"] = [&library_path, &index] ( TtTextTemplate::InternalDocument& tmp ) {
        tmp["number"]   = TtUtility::ToStringFrom( index + 1 );
        tmp["lib_path"] = library_path;
      };
      document["lib_all"] = [&index] ( TtTextTemplate::InternalDocument& tmp ) {
        tmp["number"] = TtUtility::ToStringFrom( index + 1 );
      };
      ++index;
    }
  }

  {
    auto tmp = [this, &document] ( const std::string& key, const std::string& value ) {
      if ( current_structure_->use_precompile_header_ ) {
        document[key] = [&value] ( TtTextTemplate::InternalDocument& tmp ) {
          tmp["file"] = value;
        };
      }
    };
    tmp( "precompiled_header_source", current_structure_->precompiled_header_source_ );
    tmp( "precompiled_header_header", current_structure_->precompiled_header_header_ );
    tmp( "precompiled_header_object", current_structure_->output_directory_name_ + "\\" + TtPath::ChangeExtension( current_structure_->precompiled_header_header_, "pch" ) );
    document["precompiled_header_force_include"] = current_structure_->use_precompile_header_ && current_structure_->precompiled_header_force_include_;

    document["precompiled_header_compile"] = current_structure_->use_precompile_header_;
  }

  if ( vm.use_ ) {
    std::vector<Project::MakeCommandEntry> entries = vm.GetMakeCommandEntries();
    int index = 0;
    for ( Project::MakeCommandEntry& entry : entries ) {
      document["additional"] = [&entry, &index] ( TtTextTemplate::InternalDocument& tmp ) {
        tmp["number"] = TtUtility::ToStringFrom( index + 1 );
        tmp["target"] = entry.target_;
      };
      if ( entry.included_in_build_ ) {
        document["additional_all"] = [&entry, &index] ( TtTextTemplate::InternalDocument& tmp ) {
          tmp["number"] = TtUtility::ToStringFrom( index + 1 );
        };
      }
      if ( entry.included_in_clean_ ) {
        document["additional_clean"] = [&entry, &index] ( TtTextTemplate::InternalDocument& tmp ) {
          tmp["number"] = TtUtility::ToStringFrom( index + 1 );
        };
      }
      document["additional_compile"] = [&entry, &index] ( TtTextTemplate::InternalDocument& tmp ) {
        tmp["number"] = TtUtility::ToStringFrom( index + 1 );
        for ( auto& depend : entry.depends_ ) {
          tmp["depends"] = [&depend] ( TtTextTemplate::InternalDocument& tmp_depend ) {
            tmp_depend["file"] = depend;
          };
        }
        for ( auto& command : entry.commands_ ) {
          tmp["commands"] = [&command] ( TtTextTemplate::InternalDocument& tmp_command ) {
            tmp_command["command"] = command;
          };
        }
      };
      ++index;
    }
  }

  // ファイル出力
  {
    std::string makefile_path = this->GetMakefilePath();
    if ( NOT( Utility::MakeDirectoryOfFileIfNotExist( makefile_path ) ) ) {
      throw MakeFileCreateException( "ディレクトリの作成に失敗しました。\r\nファイル : " + makefile_path );
    }

    FILE* file;
    errno_t error_number = ::fopen_s( &file, makefile_path.c_str(), "w" );
    if ( error_number != 0 ) {
      throw MakeFileCreateException( "ファイルを開けませんでした。\r\n" + makefile_path );
    }
    int ret = ::fputs( document.MakeText().c_str(), file );
    ::fclose( file );
    if ( ret == EOF ) {
      throw MakeFileCreateException( "ファイルの書き込みに失敗しました。\r\n" + makefile_path );
    }
  }
}


TtProcess::CreateInfo
Developer::MakeProcessCreateInfo( Settings::ExternalProgram& program, TtForm& parent, const std::string& open_file )
{
  ProjectVariablesTranslator translator( *this );
  translator.SetUserInputHandlerAndParentOfDialog( program.name_ + "への入力", parent );
  translator.SetFileVariable( program.for_open_file_, open_file );

  TtProcess::CreateInfo info( translator.Translate( program.path_ ) );
  info.SetArguments( translator.Translate( program.argument_ ) );
  if ( NOT( program.current_directory_.empty() ) ) {
    info.SetCurrentDirectory( translator.Translate( program.current_directory_ ) );
  }
  return info;
}


// -- ProjectVariablesTranslator -------------------------------------
Developer::ProjectVariablesTranslator::ProjectVariablesTranslator( Developer& developer ) :
table_(),
developer_( developer ),
user_input_handler_( nullptr ),
use_file_variable_( false ),
file_variable_( "" )
{
}

void
Developer::ProjectVariablesTranslator::SetUserInputHandler( UserInputHandler handler )
{
  user_input_handler_ = handler;
}

void
Developer::ProjectVariablesTranslator::SetUserInputHandlerAndParentOfDialog( const std::string& title, TtForm& form )
{
  this->SetUserInputHandler( [title, &form] ( void ) -> std::string {
    UserInputDialog dialog( title );
    dialog.ShowDialog( form );
    return dialog.GetInput();
  } );
}

void
Developer::ProjectVariablesTranslator::SetFileVariable( bool use, const std::string& file )
{
  use_file_variable_ = use;
  file_variable_ = file;
}


std::string
Developer::ProjectVariablesTranslator::Translate( const std::string& source )
{
  TtTextTemplate::Document document;
  document.ParseText( source );
  auto q = [this, &document] ( const std::string& key, std::function<std::string ( void )> lazy_string ) {
    if ( document.HasKey( key ) ) {
      auto it = table_.find( key );
      if ( it == table_.end() ) {
        table_[key] = lazy_string();
      }
      document[key] = table_[key];
    }
  };
#define Q( key, str ) q( (key), [&] ( void ) { return (str); } )
  Q( "project_name",        developer_.GetProject().name_ );
  Q( "project_folder_path", developer_.GetProjectDirectoryPath() );
  Q( "project_folder_name", TtPath::BaseName( developer_.GetProjectDirectoryPath() ) );
  Q( "target_path",         developer_.GetTargetFilePath() );
  Q( "output_folder_path",  developer_.GetOutputDirectoryPath() );
  Q( "output_folder_name",  TtPath::BaseName( developer_.GetOutputDirectoryPath() ) );
  Q( "all_files",           developer_.GetProjectFilesPathEnumerationString() );
  Q( "header_files",        developer_.GetProjectFilesPathEnumerationStringOf( HeaderFileSelect ) );
  Q( "source_files",        developer_.GetProjectFilesPathEnumerationStringOf( SourceFileSelect ) );
  Q( "header_source_files", developer_.GetProjectFilesPathEnumerationStringOf( HeaderSourceFileSelect ) );
  if ( use_file_variable_ ) {
    Q( "file", file_variable_ );
  }
#undef Q
  q( "user_input", user_input_handler_ );
  return document.MakeText();
}
