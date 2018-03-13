// project.cpp

#include <algorithm>

#include "ttl_define.h"
#include "tt_path.h"
#include "tt_string.h"
#include "tt_ini_file.h"
#include "tt_utility.h"

#include "exception.h"
#include "main_tree.h"
#include "utility.h"

#include "project.h"

using namespace TTVCDeveloper;


// -- Element ------------------------------------------------------------
Project::Element::Element( Project& project, Image::Index::Value original_icon_index ) :
project_( project ),
original_icon_index_( original_icon_index )
{
}

Project::Element::~Element()
{
}


int
Project::Element::GetTreeViewIconIndex( bool )
{
  return static_cast<int>( original_icon_index_ ) + Image::OffsetOfList;
}


// -- ProjectElement -----------------------------------------------------
Project::ProjectElement::ProjectElement( Project& project ) :
Element( project, Image::Index::ProjectElement ),
name_( project.name_ )
{
}


void
Project::ProjectElement::SetName( const std::string& name )
{
  name_ = name;
}

std::string
Project::ProjectElement::GetName( void ) const
{
  return name_;
}

std::string
Project::ProjectElement::GetDataForFile( void ) const
{
  throw TTVC_DEVELOPER_INTERNAL_EXCEPTION;
}

// -- Directory ----------------------------------------------------------
Project::Directory::Directory( Project& project, const std::string& name ) :
Element( project, Image::Index::Directory ),
name_( name )
{
}


void
Project::Directory::SetName( const std::string& name )
{
  name_ = name;
}

std::string
Project::Directory::GetName( void ) const
{
  return name_;
}

std::string
Project::Directory::GetDataForFile( void ) const
{
  return name_;
}

// -- File ---------------------------------------------------------------
Project::File::File( Project& project, Image::Index::Value original_icon_index, const std::string& path ) :
Element( project, original_icon_index ),
path_( path )
{
}


void
Project::File::SetPath( const std::string& path )
{
  path_ = path;
}

const std::string&
Project::File::GetPath( void ) const
{
  return path_;
}


int
Project::File::GetTreeViewIconIndex( bool use_original )
{
  if ( NOT( use_original ) ) {
    SHFILEINFO file_info;
    if ( ::SHGetFileInfo( this->GetFullPath().c_str(), 0, &file_info, sizeof( SHFILEINFO ), SHGFI_SYSICONINDEX | SHGFI_SMALLICON ) != 0 ) {
      return file_info.iIcon;
    }
  }
  return this->Element::GetTreeViewIconIndex( use_original );
}


std::string
Project::File::GetName( void ) const
{
  return this->GetPath();
}

std::string
Project::File::GetDataForFile( void ) const
{
  return path_;
}


std::string
Project::File::GetFullPath( void )
{
  return TtPath::DirName( project_.path_ ) + "\\" + path_;
}


// -- OtherFile ----------------------------------------------------------
Project::OtherFile::OtherFile( Project& project, const std::string& path ) :
File( project, Image::Index::OtherFile, path )
{
}

int
Project::OtherFile::GetTreeViewIconIndex( bool )
{
  return this->File::GetTreeViewIconIndex( false );
}


// -- SourceFile ---------------------------------------------------------
Project::SourceFile::SourceFile( Project& project, Image::Index::Value original_icon_index, const std::string& path ) :
File( project, original_icon_index, path )
{
}

std::string
Project::SourceFile::GetObjectFilePath( const std::string& output_directory_path )
{
  return output_directory_path + "\\" + TtPath::ChangeExtension( path_, "obj" );
}


// -- ResourceScriptFile -------------------------------------------------
Project::ResourceScriptFile::ResourceScriptFile( Project& project, const std::string& path ) :
SourceFile( project, Image::Index::ResourceScriptFile, path )
{
}

std::string
Project::ResourceScriptFile::GetObjectFilePath( const std::string& output_directory_path )
{
  return output_directory_path + "\\" + TtPath::ChangeExtension( path_, "res" );
}


// -- Enums --------------------------------------------------------------
template <>
TtEnumTable<Project::TargetKind>::TtEnumTable( void )
{
#define REGISTER( NAME ) this->Register( Project::TargetKind::NAME, #NAME )
  REGISTER( Console );
  REGISTER( Windows );
  REGISTER( DynamicLinkLibrary );
  REGISTER( StaticLibrary );
#undef REGISTER
}
template <>
TtEnumTable<Project::TargetKind>
TtEnumBase<Project::TargetKind>::TABLE;


template <>
TtEnumTable<Project::OptimizeKind>::TtEnumTable( void )
{
#define REGISTER( NAME ) this->Register( Project::OptimizeKind::NAME, #NAME )
  REGISTER( NoSpecify );
  REGISTER( NoOptimize );
  REGISTER( Smallest );
  REGISTER( Fastest );
  REGISTER( Full );
#undef REGISTER
}
template <>
TtEnumTable<Project::OptimizeKind>
TtEnumBase<Project::OptimizeKind>::TABLE;


template <>
TtEnumTable<Project::RuntimeLibrary>::TtEnumTable( void )
{
#define REGISTER( NAME ) this->Register( Project::RuntimeLibrary::NAME, #NAME )
  REGISTER( Static );
  REGISTER( StaticDebug );
  REGISTER( Dynamic );
  REGISTER( DynamicDebug );
#undef REGISTER
}
template <>
TtEnumTable<Project::RuntimeLibrary>
TtEnumBase<Project::RuntimeLibrary>::TABLE;


// -- Structure ----------------------------------------------------------
Project::Structure::Structure( void ) :
platform_( Platform::X86 ),
target_kind_( TargetKind::Console ),
target_use_output_edit_( false ),
optimize_kind_( OptimizeKind::NoSpecify ),
runtime_library_( RuntimeLibrary::Static ),
use_exception_( true ),
use_precompile_header_( false ),
precompiled_header_force_include_( true ),
warning_level_( 1 ),
error_if_warning_( false ),
no_logo_( true )
{
}


// -- Project ------------------------------------------------------------
Project::Project( void ) :
project_element_( *this )
{
  this->Reset( "" );
}


void
Project::Reset( const std::string& path )
{
  path_ = path;
  name_ = "";
  use_icon_ = false;
  icon_file_path_ = "";
  icon_file_index_ = 0;
  project_element_.SetName( name_ );
  directories_.clear();
  files_.clear();
  structures_.clear();
}


Project::ProjectElement&
Project::GetProjectElement( void )
{
  return project_element_;
}

std::vector<std::shared_ptr<Project::Directory>>&
Project::GetDirectories( void )
{
  return directories_;
}

std::vector<std::shared_ptr<Project::File>>&
Project::GetFiles( void )
{
  return files_;
}

std::vector<std::shared_ptr<Project::Structure>>&
Project::GetStructures( void )
{
  return structures_;
}


std::string
Project::ConvertRelativePath( const std::string& target )
{
  auto result = TtPath::GetRelativePathTo( path_, false, target, false );
  return TtPath::Canonicalize( result ? result.value() : target );
}

std::string
Project::ConvertAbsolutePath( const std::string& target )
{
  if ( NOT( TtPath::IsRelative( target ) ) ) {
    return TtPath::Canonicalize( target );
  }
  return TtPath::Canonicalize( TtPath::DirName( path_ ) + "\\" + target );
}


std::string
Project::GetIconAbsolutePath( void )
{
  return this->ConvertAbsolutePath( icon_file_path_ );
}


Project::Directory&
Project::AddDirectory( const std::string& name )
{
  directories_.push_back( std::shared_ptr<Directory>( new Directory( *this, name ) ) );
  return *(directories_.back());
}

Project::File&
Project::AddFile( const std::string& path )
{
  if ( TtString::EndWith( path, ".c" ) ) {
    files_.push_back( std::shared_ptr<File>( new CFile( *this, path ) ) );
  }
  else if ( TtString::EndWith( path, ".cpp" ) ||
            TtString::EndWith( path, ".cxx" ) ||
            TtString::EndWith( path, ".cp" ) ||
            TtString::EndWith( path, ".cc" ) ) {
    files_.push_back( std::shared_ptr<File>( new CppFile( *this, path ) ) );
  }
  else if ( TtString::EndWith( path, ".rc" ) ) {
    files_.push_back( std::shared_ptr<File>( new ResourceScriptFile( *this, path ) ) );
  }
  else if ( TtString::EndWith( path, ".h" ) ||
            TtString::EndWith( path, ".hxx" ) ||
            TtString::EndWith( path, ".hpp" ) ) {
    files_.push_back( std::shared_ptr<File>( new HeaderFile( *this, path ) ) );
  }
  else {
    files_.push_back( std::shared_ptr<File>( new OtherFile( *this, path ) ) );
  }
  return *(files_.back());
}


void
Project::EachFileWithIndex( std::function<void ( File& file, int index )> function )
{
  int i = 0;
  for ( auto& file : files_ ) {
    function( *file, i );
    ++i;
  }
}


// -- 以下プロジェクトの保存読み込み関連 ---------------------------------
namespace Tag {
#define DEFINE_PARAMETER_NAME_STRING( name ) static const std::string name( #name )
  DEFINE_PARAMETER_NAME_STRING( Project );
  DEFINE_PARAMETER_NAME_STRING( Name );
  DEFINE_PARAMETER_NAME_STRING( Element );
  DEFINE_PARAMETER_NAME_STRING( Structure );
  DEFINE_PARAMETER_NAME_STRING( TargetKind );
  DEFINE_PARAMETER_NAME_STRING( OutputDirectoryName );
  DEFINE_PARAMETER_NAME_STRING( TargetName );
  DEFINE_PARAMETER_NAME_STRING( TargetArgument );
  DEFINE_PARAMETER_NAME_STRING( TargetCurrentDirectory );
  DEFINE_PARAMETER_NAME_STRING( TargetUseOutputEdit );
  DEFINE_PARAMETER_NAME_STRING( TargetUseInputDialog );
  DEFINE_PARAMETER_NAME_STRING( IncludePath );
  DEFINE_PARAMETER_NAME_STRING( Define );
  DEFINE_PARAMETER_NAME_STRING( OptimizeKind );
  DEFINE_PARAMETER_NAME_STRING( RuntimeLibrary );
  DEFINE_PARAMETER_NAME_STRING( UseException );
  DEFINE_PARAMETER_NAME_STRING( UsePrecompiledHeader );
  DEFINE_PARAMETER_NAME_STRING( PrecompiledHeaderSource );
  DEFINE_PARAMETER_NAME_STRING( PrecompiledHeaderHeader );
  DEFINE_PARAMETER_NAME_STRING( PrecompiledHeaderForceInclude );
  DEFINE_PARAMETER_NAME_STRING( WarningLevel );
  DEFINE_PARAMETER_NAME_STRING( NoWarning );
  DEFINE_PARAMETER_NAME_STRING( ErrorIfWarning );
  DEFINE_PARAMETER_NAME_STRING( LibraryPath );
  DEFINE_PARAMETER_NAME_STRING( NoLogo );
  DEFINE_PARAMETER_NAME_STRING( CompilerOtherOption );
  DEFINE_PARAMETER_NAME_STRING( ResourceCompilerOption );
  DEFINE_PARAMETER_NAME_STRING( ExtensionScriptPath );
  DEFINE_PARAMETER_NAME_STRING( UseIcon );
  DEFINE_PARAMETER_NAME_STRING( IconFilePath );
  DEFINE_PARAMETER_NAME_STRING( IconFileIndex );
  DEFINE_PARAMETER_NAME_STRING( Platform );
}

void
Project::SaveToFile( MainTree& tree )
{
  TtIniFile ini( path_ );
  TtIniSection section = ini[Tag::Project];
  section.Clear();
  section.SetString(  Tag::Name,          name_ );
  section.SetBoolean( Tag::UseIcon,       use_icon_ );
  section.SetString(  Tag::IconFilePath,  icon_file_path_ );
  section.SetInteger( Tag::IconFileIndex, icon_file_index_ );
  this->SaveToFileTree( tree );
  this->SaveToFileStructures();
}

void
Project::SaveToFileAsDefault( void )
{
  TtIniFile ini( path_ );
  TtIniSection section = ini[Tag::Project];
  section.Clear();
  section.SetString(  Tag::Name,          name_ );
  section.SetBoolean( Tag::UseIcon,       use_icon_ );
  section.SetString(  Tag::IconFilePath,  icon_file_path_ );
  section.SetInteger( Tag::IconFileIndex, icon_file_index_ );
  this->SaveToFileStructures();
}

void
Project::SaveToFileTree( MainTree& tree )
{
  TtIniFile ini( path_ );

  using Proc = std::function<void ( MainTree::Item& item, int parent_number )>;
  Proc proc = [section = ini[Tag::Project], number = 1, &proc] ( MainTree::Item& item, int parent_number ) mutable {
    section.SetString(
      Tag::Element + TtUtility::ToStringFrom( number ),
      item.GetParameter()->GetDataForFile() + "\t" +
      ( item.GetParameter()->TypeIs<Project::Directory>() ? "0" : "1" ) + "\t" +
      TtUtility::ToStringFrom( parent_number ) );

    ++number;
    if ( item.GetParameter()->TypeIs<Project::Directory>() ) {
      int current_number = number - 1;
      for ( MainTree::Item tmp : item.GetChildEnumerable() ) {
        proc( tmp, current_number );
      }
    }
  };
  for ( MainTree::Item tmp : tree.GetRoot().GetChildEnumerable() ) {
    proc( tmp, 0 );
  }
  ini.Flush();
}

void
Project::SaveToFileStructures( void )
{
  TtIniFile ini( path_ );
  int index = 0;
  for ( auto& structure : structures_ ) {
    TtIniSection section = ini[Tag::Structure + TtUtility::ToStringFrom( index )];
    section.Clear();
    auto set_string_vector = [&section] ( const std::string& key, std::vector<std::string> v ) {
      for ( unsigned int i = 0; i < v.size(); ++i ) {
        section.SetString( key + TtUtility::ToStringFrom( i ), v[i] );
      }
    };
    section.SetString( Tag::Name, structure->name_ );

    section.SetEnum<Platform>( Tag::Platform, structure->platform_ );
    section.SetEnum<Project::TargetKind>( Tag::TargetKind, structure->target_kind_ );
    section.SetString(  Tag::OutputDirectoryName,    structure->output_directory_name_ );
    section.SetString(  Tag::TargetName,             structure->target_name_ );
    section.SetString(  Tag::TargetArgument,         structure->target_argument_ );
    section.SetString(  Tag::TargetCurrentDirectory, structure->target_current_directory_ );
    section.SetBoolean( Tag::TargetUseOutputEdit,    structure->target_use_output_edit_ );
    section.SetBoolean( Tag::TargetUseInputDialog,   structure->target_use_input_dialog_ );

    section.SetEnum<Project::OptimizeKind>( Tag::OptimizeKind, structure->optimize_kind_ );
    section.SetEnum<Project::RuntimeLibrary>( Tag::RuntimeLibrary, structure->runtime_library_ );
    section.SetBoolean( Tag::UseException, structure->use_exception_ );

    section.SetBoolean( Tag::UsePrecompiledHeader,          structure->use_precompile_header_ );
    section.SetString(  Tag::PrecompiledHeaderSource,       structure->precompiled_header_source_ );
    section.SetString(  Tag::PrecompiledHeaderHeader,       structure->precompiled_header_header_ );
    section.SetBoolean( Tag::PrecompiledHeaderForceInclude, structure->precompiled_header_force_include_ );

    set_string_vector( Tag::IncludePath, structure->include_paths_ );
    set_string_vector( Tag::Define,      structure->defines_ );

    section.SetInteger( Tag::WarningLevel, structure->warning_level_ );
    for ( unsigned int i = 0; i < structure->no_warnings_.size(); ++i ) {
      section.SetInteger( Tag::NoWarning + TtUtility::ToStringFrom( i ), structure->no_warnings_[i] );
    }
    section.SetBoolean( Tag::ErrorIfWarning, structure->error_if_warning_ );

    set_string_vector( Tag::LibraryPath, structure->library_paths_ );

    section.SetBoolean( Tag::NoLogo,                 structure->no_logo_ );
    section.SetString(  Tag::CompilerOtherOption,    structure->compiler_other_option_ );
    section.SetString(  Tag::ResourceCompilerOption, structure->resource_compiler_option_ );
    section.SetString(  Tag::ExtensionScriptPath,    structure->extension_script_path_ );
    ++index;
  }
  ini[Tag::Structure + TtUtility::ToStringFrom( index )].Clear();
  ini.Flush();
}


void
Project::LoadFromFile( MainTree& tree )
{
  if ( NOT( TtPath::FileExists( path_ ) ) ) {
    throw FileNotExistException( path_ );
  }
  this->LoadFromFileTree( tree );
  this->LoadFromFileStructures();
}

void
Project::LoadFromFileAsDefault( void )
{
  TtIniFile ini( path_ );
  TtIniSection section = ini[Tag::Project];
  name_ = section.GetString( Tag::Name );
  this->LoadFromFileStructures();
}

void
Project::LoadFromFileTree( MainTree& tree )
{
  TtIniFile ini( path_ );
  TtIniSection section = ini[Tag::Project];

  std::vector<std::string> keys = section.GetKeys();
  struct FileValue {
    std::string data;
    bool        is_directory;
    int         parent_number;
  };
  std::vector<FileValue> values;

  for ( int i = 1; ; ++i ) {
    std::string key = Tag::Element + TtUtility::ToStringFrom( i );
    auto it = std::find( keys.begin(), keys.end(), key );
    if ( it == keys.end() ) {
      break;
    }
    std::string data = section.GetString( key );
    if ( data.empty() ) {
      throw ProjectFileFormatException( path_, key );
    }
    unsigned int tab_point_1 = data.find( '\t', 0 );
    if ( tab_point_1 == std::string::npos ) {
      throw ProjectFileFormatException( path_, key );
    }
    unsigned int tab_point_2 = data.find( '\t', tab_point_1 + 1 );
    if ( tab_point_2 == std::string::npos ) {
      throw ProjectFileFormatException( path_, key );
    }
    int tmp;
    if ( NOT( TtUtility::StringToInteger( data.substr( tab_point_1 + 1, tab_point_2 - tab_point_1 - 1 ), &tmp ) ) ) {
      throw ProjectFileFormatException( path_, key );
    }
    FileValue file_value( {data.substr( 0, tab_point_1 ), (( tmp == 0 ) ? true : false)} );
    if ( NOT( TtUtility::StringToInteger( data.substr( tab_point_2 + 1 ), &file_value.parent_number ) ) ) {
      throw ProjectFileFormatException( path_, key );
    }
    if ( static_cast<unsigned int>( file_value.parent_number ) > values.size() ||
         ( file_value.parent_number > 0 && NOT( values[file_value.parent_number - 1].is_directory ) ) ) {
      throw ProjectFileFormatException( path_, key );
    }
    values.push_back( file_value );
  }

  // -- 展開 -----
  this->Reset( path_ );
  name_            = section.GetString(  Tag::Name );
  use_icon_        = section.GetBoolean( Tag::UseIcon, false );
  icon_file_path_  = section.GetString(  Tag::IconFilePath );
  icon_file_index_ = section.GetInteger( Tag::IconFileIndex, 0 );
  project_element_.SetName( name_ );
  tree.Reset( project_element_ );

  std::vector<MainTree::Item> items;
  items.push_back( tree.GetRoot() );
  for ( FileValue& value : values ) {
    if ( static_cast<unsigned int>( value.parent_number ) > items.size() - 1 ||
         items[value.parent_number].GetParameter()->TypeIs<Project::File>() ) {
      throw TTVC_DEVELOPER_INTERNAL_EXCEPTION;
    }
    auto add_proc = [&] ( Project::Element& element ) {
      MainTree::Item target = items[value.parent_number].AddChildLast( element.GetName() );
      tree.SetElementToItem( target, element );
      items.push_back( target );
    };
    if ( value.is_directory ) {
      Project::Directory& directory = this->AddDirectory( value.data );
      add_proc( directory );
    }
    else {
      Project::File& file = this->AddFile( value.data );
      add_proc( file );
    }
  }
}

void
Project::LoadFromFileStructures( void )
{
  TtIniFile ini( path_ );
  auto sections = ini.GetSections();

  for ( int i = 0; ; ++i ) {
    std::string name = Tag::Structure + TtUtility::ToStringFrom( i );
    auto it = std::find_if( sections.begin(), sections.end(), [&name] ( TtIniSection& section ) { return section.GetName() == name; } );
    if ( it == sections.end() ) {
      break;
    }
    auto section = *it;
    auto keys = section.GetKeys();
    Project::Structure structure;
    auto get_string_vector = [&section, &keys] ( std::vector<std::string>& v, const std::string& key_base ) {
      for ( unsigned int i = 0; ; ++i ) {
        std::string current = key_base + TtUtility::ToStringFrom( i );
        auto it = std::find_if( keys.begin(), keys.end(), [&current] ( const std::string& key ) { return key == current; } );
        if ( it == keys.end() ) {
          break;
        }
        v.push_back( section.GetString( current ) );
      }
    };

    structure.name_ = section.GetString( Tag::Name, "構成" + TtUtility::ToStringFrom( i ) );

    structure.platform_                 = section.GetEnum<Platform>( Tag::Platform, Platform::X86 );
    structure.target_kind_              = section.GetEnum<TargetKind>( Tag::TargetKind, TargetKind::Windows );
    structure.output_directory_name_    = section.GetString(  Tag::OutputDirectoryName );
    structure.target_name_              = section.GetString(  Tag::TargetName );
    structure.target_argument_          = section.GetString(  Tag::TargetArgument );
    structure.target_current_directory_ = section.GetString(  Tag::TargetCurrentDirectory );
    structure.target_use_output_edit_   = section.GetBoolean( Tag::TargetUseOutputEdit, false );
    structure.target_use_input_dialog_  = section.GetBoolean( Tag::TargetUseInputDialog, false );

    structure.optimize_kind_   = section.GetEnum<OptimizeKind>( Tag::OptimizeKind, OptimizeKind::NoSpecify );
    structure.runtime_library_ = section.GetEnum<RuntimeLibrary>( Tag::RuntimeLibrary, RuntimeLibrary::Static );
    structure.use_exception_   = section.GetBoolean( Tag::UseException, true );

    structure.use_precompile_header_            = section.GetBoolean( Tag::UsePrecompiledHeader, false );
    structure.precompiled_header_source_        = section.GetString(  Tag::PrecompiledHeaderSource );
    structure.precompiled_header_header_        = section.GetString(  Tag::PrecompiledHeaderHeader );
    structure.precompiled_header_force_include_ = section.GetBoolean( Tag::PrecompiledHeaderForceInclude, true );

    get_string_vector( structure.include_paths_, Tag::IncludePath );
    get_string_vector( structure.defines_, Tag::Define );

    structure.warning_level_ = section.GetInteger( Tag::WarningLevel, 0 );
    for ( unsigned int k = 0; ; ++k ) {
      std::string current = Tag::NoWarning + TtUtility::ToStringFrom( k );
      auto tmp = std::find_if( keys.begin(), keys.end(), [&current] ( const std::string& key ) { return key == current; } );
      if ( tmp == keys.end() ) {
        break;
      }
      unsigned int number = section.GetInteger( current, std::numeric_limits<unsigned int>::max()  );
      if ( number == std::numeric_limits<unsigned int>::max() ) {
        continue;
      }
      structure.no_warnings_.push_back( number );
    }
    structure.error_if_warning_ = section.GetBoolean( Tag::ErrorIfWarning, false );

    get_string_vector( structure.library_paths_, Tag::LibraryPath );

    structure.no_logo_                  = section.GetBoolean( Tag::NoLogo, true );
    structure.compiler_other_option_    = section.GetString( Tag::CompilerOtherOption );
    structure.resource_compiler_option_ = section.GetString( Tag::ResourceCompilerOption );
    structure.extension_script_path_    = section.GetString( Tag::ExtensionScriptPath );

    structures_.push_back( std::make_shared<Project::Structure>( structure ) );
  }

  if ( structures_.empty() ) {
    Project::Structure structure;
    structure.name_ = "Debug";
    structures_.push_back( std::make_shared<Project::Structure>( structure ) );
    structure.name_ = "Release";
    structures_.push_back( std::make_shared<Project::Structure>( structure ) );
  }
}
