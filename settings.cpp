// settings.cpp

#include <algorithm>

#include "tt_exception.h"

#include "exception.h"
#include "settings.h"

using namespace TTVCDeveloper;


// -----------------------------------------------------------------------
// -----------------------------------------------------------------------
namespace Tag {
#define DEFINE_PARAMETER_NAME_STRING( name ) static const std::string name( #name )
  DEFINE_PARAMETER_NAME_STRING( Compiler );
  DEFINE_PARAMETER_NAME_STRING( X86 );
  DEFINE_PARAMETER_NAME_STRING( X64 );
  DEFINE_PARAMETER_NAME_STRING( MakeCommandPath );
  DEFINE_PARAMETER_NAME_STRING( CompilerPath );
  DEFINE_PARAMETER_NAME_STRING( ResourceCompilerPath );
  DEFINE_PARAMETER_NAME_STRING( EnvironmentVariableInclude );
  DEFINE_PARAMETER_NAME_STRING( EnvironmentVariableLib );
  DEFINE_PARAMETER_NAME_STRING( EnvironmentVariableLibpath );
  DEFINE_PARAMETER_NAME_STRING( EditorPath );
  DEFINE_PARAMETER_NAME_STRING( EditorArgument );
  DEFINE_PARAMETER_NAME_STRING( ExternalProgram );
  DEFINE_PARAMETER_NAME_STRING( Name );
  DEFINE_PARAMETER_NAME_STRING( ForOpenFile );
  DEFINE_PARAMETER_NAME_STRING( Path );
  DEFINE_PARAMETER_NAME_STRING( Argument );
  DEFINE_PARAMETER_NAME_STRING( CurrentDirectory );
  DEFINE_PARAMETER_NAME_STRING( UseOutputEdit );
  DEFINE_PARAMETER_NAME_STRING( ShortcutKey );
  DEFINE_PARAMETER_NAME_STRING( CompileKey );
  DEFINE_PARAMETER_NAME_STRING( BuildKey );
  DEFINE_PARAMETER_NAME_STRING( RebuildKey );
  DEFINE_PARAMETER_NAME_STRING( ExecuteKey );
  DEFINE_PARAMETER_NAME_STRING( TreeUseOriginalIcon );
  DEFINE_PARAMETER_NAME_STRING( ExtensionScriptPath );
}



// -- Settings -----------------------------------------------------------
Settings::ExternalProgram::ExternalProgram( void ) :
name_(),
for_open_file_( false ),
path_(),
argument_(),
current_directory_(),
use_output_edit_( false )
{
}


Settings::Settings( void ) :
compiler_x86_(),
compiler_x64_(),

editor_path_( "" ),
editor_argument_( "" ),

tree_use_original_icon_( true )
{
}


Settings::Compiler&
Settings::GetCompilerByPlatform( Platform platform )
{
  switch ( platform ) {
  case Platform::X86:
    return compiler_x86_;

  case Platform::X64:
    return compiler_x64_;
  }
  throw TTVC_DEVELOPER_INTERNAL_EXCEPTION;
}


void
Settings::ReadFromFile( void )
{
  TtIniFile ini_file;
  this->ReadFromIniFile( ini_file );
}

void
Settings::ReadFromFile( const std::string& filename )
{
  TtIniFile ini_file( filename );
  this->ReadFromIniFile( ini_file );
}

void
Settings::ReadFromIniFile( TtIniFile& ini_file )
{
  {
    TtIniSection section = ini_file[Tag::Compiler];

    auto compiler_get = [&section] ( Settings::Compiler& compiler, const std::string& postfix ) {
      compiler.make_command_path_            = section.GetString( Tag::MakeCommandPath            + postfix, "" );
      compiler.compiler_path_                = section.GetString( Tag::CompilerPath               + postfix, "" );
      compiler.resource_compiler_path_       = section.GetString( Tag::ResourceCompilerPath       + postfix, "" );
      compiler.environment_variable_include_ = section.GetString( Tag::EnvironmentVariableInclude + postfix, "" );
      compiler.environment_variable_lib_     = section.GetString( Tag::EnvironmentVariableLib     + postfix, "" );
      compiler.environment_variable_libpath_ = section.GetString( Tag::EnvironmentVariableLibpath + postfix, "" );
    };
    compiler_get( compiler_x86_, Tag::X86 );
    compiler_get( compiler_x64_, Tag::X64 );

    editor_path_            = section.GetString(  Tag::EditorPath,          ""   );
    editor_argument_        = section.GetString(  Tag::EditorArgument,      ""   );
    tree_use_original_icon_ = section.GetBoolean( Tag::TreeUseOriginalIcon, true );
    extension_script_path_  = section.GetString(  Tag::ExtensionScriptPath, ""   );
  }

  {
    TtIniSection section = ini_file[Tag::ShortcutKey];
    auto read_key = [&section] ( const std::string& tag ) -> ShortcutKey {
      std::string tmp = section.GetString( tag, "" );
      if ( tmp.empty() ) {
        return ShortcutKey( 0 );
      }
      return ShortcutKey::Deserialize( tmp );
    };
    compile_key_ = read_key( Tag::CompileKey );
    build_key_   = read_key( Tag::BuildKey );
    rebuild_key_ = read_key( Tag::RebuildKey );
    execute_key_ = read_key( Tag::ExecuteKey );
  }

  {
    external_programs_.clear();
    auto sections = ini_file.GetSections();
    for ( int i = 0; ; ++i ) {
      std::string name = Tag::ExternalProgram + TtUtility::ToStringFrom( i );
      auto it = std::find_if( sections.begin(), sections.end(), [&name] ( TtIniSection& section ) { return section.GetName() == name; } );
      if ( it == sections.end() ) {
        break;
      }
      auto section = *it;
      ExternalProgram tmp;
      tmp.name_              = section.GetString(  Tag::Name,             "" );
      tmp.for_open_file_     = section.GetBoolean( Tag::ForOpenFile,   false );
      tmp.path_              = section.GetString(  Tag::Path,             "" );
      tmp.argument_          = section.GetString(  Tag::Argument,         "" );
      tmp.current_directory_ = section.GetString(  Tag::CurrentDirectory, "" );
      tmp.use_output_edit_   = section.GetBoolean( Tag::UseOutputEdit, false );
      external_programs_.push_back( tmp );
    }
  }
}


void
Settings::WriteToFile( void ) const
{
  TtIniFile ini_file;
  this->WriteToIniFile( ini_file );
}

void
Settings::WriteToFile( const std::string& filename ) const
{
  TtIniFile ini_file( filename );
  this->WriteToIniFile( ini_file );
}

void
Settings::WriteToIniFile( TtIniFile& ini_file ) const
{
  {
    TtIniSection section = ini_file[Tag::Compiler];

    auto compiler_set = [&section] ( const Settings::Compiler& compiler, const std::string& postfix ) {
      section.SetString( Tag::MakeCommandPath            + postfix, compiler.make_command_path_            );
      section.SetString( Tag::CompilerPath               + postfix, compiler.compiler_path_                );
      section.SetString( Tag::ResourceCompilerPath       + postfix, compiler.resource_compiler_path_       );
      section.SetString( Tag::EnvironmentVariableInclude + postfix, compiler.environment_variable_include_ );
      section.SetString( Tag::EnvironmentVariableLib     + postfix, compiler.environment_variable_lib_     );
      section.SetString( Tag::EnvironmentVariableLibpath + postfix, compiler.environment_variable_libpath_ );
    };

    compiler_set( compiler_x86_, Tag::X86 );
    compiler_set( compiler_x64_, Tag::X64 );

    section.SetString(  Tag::EditorPath,          editor_path_            );
    section.SetString(  Tag::EditorArgument,      editor_argument_        );
    section.SetBoolean( Tag::TreeUseOriginalIcon, tree_use_original_icon_ );
    section.SetString(  Tag::ExtensionScriptPath, extension_script_path_  );
  }

  {
    TtIniSection section = ini_file[Tag::ShortcutKey];
    section.SetString( Tag::CompileKey, compile_key_.Serialize() );
    section.SetString( Tag::BuildKey,   build_key_.Serialize() );
    section.SetString( Tag::RebuildKey, rebuild_key_.Serialize() );
    section.SetString( Tag::ExecuteKey, execute_key_.Serialize() );
  }

  {
    int index = 0;
    for ( auto& external_program : external_programs_ ) {
      TtIniSection section = ini_file[Tag::ExternalProgram + TtUtility::ToStringFrom( index )];
      section.SetString(  Tag::Name,             external_program.name_              );
      section.SetBoolean( Tag::ForOpenFile,      external_program.for_open_file_     );
      section.SetString(  Tag::Path,             external_program.path_              );
      section.SetString(  Tag::Argument,         external_program.argument_          );
      section.SetString(  Tag::CurrentDirectory, external_program.current_directory_ );
      section.SetBoolean( Tag::UseOutputEdit,    external_program.use_output_edit_   );
      ++index;
    }
    ini_file[Tag::ExternalProgram + TtUtility::ToStringFrom( index )].Clear();
  }
}


// -- Settings::Compiler -------------------------------------------------
std::string
Settings::Compiler::GetLinkerCommandPath( void )
{
  return TtPath::DirName( compiler_path_ ) + "\\link.exe";
}

std::string
Settings::Compiler::GetLibCommandPath( void )
{
  return TtPath::DirName( compiler_path_ ) + "\\lib.exe";
}
