// squirrel_vm.cpp

#include "tt_squirrel_utility.h"

#include "main_frame.h"
#include "exception.h"

#include "squirrel_vm.h"

using namespace TTVCDeveloper;


namespace Tag {
#define DEFINE_PARAMETER_NAME_STRING( name ) static const std::string name( #name )
  // DEFINE_PARAMETER_NAME_STRING(  );
  DEFINE_PARAMETER_NAME_STRING( Developer );
  DEFINE_PARAMETER_NAME_STRING( Structure );
  DEFINE_PARAMETER_NAME_STRING( Project );
  DEFINE_PARAMETER_NAME_STRING( Settings );
  DEFINE_PARAMETER_NAME_STRING( ExternalProgram );
  DEFINE_PARAMETER_NAME_STRING( TargetKind );
  DEFINE_PARAMETER_NAME_STRING( OptimizeKind );
  DEFINE_PARAMETER_NAME_STRING( RuntimeLibrary );
  DEFINE_PARAMETER_NAME_STRING( OutputEdit );
  DEFINE_PARAMETER_NAME_STRING( MainTab );
  DEFINE_PARAMETER_NAME_STRING( constructor );
  DEFINE_PARAMETER_NAME_STRING( project_path );
  DEFINE_PARAMETER_NAME_STRING( project_directory_path );
  DEFINE_PARAMETER_NAME_STRING( output_directory_path );
  DEFINE_PARAMETER_NAME_STRING( makefile_path );
  DEFINE_PARAMETER_NAME_STRING( target_file_path );
  DEFINE_PARAMETER_NAME_STRING( project_variables_translate );
  DEFINE_PARAMETER_NAME_STRING( path );
  DEFINE_PARAMETER_NAME_STRING( name );
  DEFINE_PARAMETER_NAME_STRING( use_icon );
  DEFINE_PARAMETER_NAME_STRING( icon_file_path );
  DEFINE_PARAMETER_NAME_STRING( icon_file_index );
  DEFINE_PARAMETER_NAME_STRING( target_kind );
  DEFINE_PARAMETER_NAME_STRING( output_directory_name );
  DEFINE_PARAMETER_NAME_STRING( target_name );
  DEFINE_PARAMETER_NAME_STRING( target_argument );
  DEFINE_PARAMETER_NAME_STRING( target_current_directory );
  DEFINE_PARAMETER_NAME_STRING( target_use_output_edit );
  DEFINE_PARAMETER_NAME_STRING( target_use_input_dialog );
  DEFINE_PARAMETER_NAME_STRING( optimize_kind );
  DEFINE_PARAMETER_NAME_STRING( runtime_library );
  DEFINE_PARAMETER_NAME_STRING( use_exception );
  DEFINE_PARAMETER_NAME_STRING( use_precompile_header );
  DEFINE_PARAMETER_NAME_STRING( precompiled_header_source );
  DEFINE_PARAMETER_NAME_STRING( precompiled_header_header );
  DEFINE_PARAMETER_NAME_STRING( precompiled_header_force_include );
  DEFINE_PARAMETER_NAME_STRING( include_paths );
  DEFINE_PARAMETER_NAME_STRING( defines );
  DEFINE_PARAMETER_NAME_STRING( warning_level );
  DEFINE_PARAMETER_NAME_STRING( no_warnings );
  DEFINE_PARAMETER_NAME_STRING( error_if_warning );
  DEFINE_PARAMETER_NAME_STRING( library_paths );
  DEFINE_PARAMETER_NAME_STRING( no_logo );
  DEFINE_PARAMETER_NAME_STRING( compiler_other_option );
  DEFINE_PARAMETER_NAME_STRING( resource_compiler_option );
  DEFINE_PARAMETER_NAME_STRING( for_open_file );
  DEFINE_PARAMETER_NAME_STRING( argument );
  DEFINE_PARAMETER_NAME_STRING( current_directory );
  DEFINE_PARAMETER_NAME_STRING( use_output_edit );
  DEFINE_PARAMETER_NAME_STRING( use_input_dialog );
  DEFINE_PARAMETER_NAME_STRING( compiler_path );
  DEFINE_PARAMETER_NAME_STRING( resource_compiler_path );
  DEFINE_PARAMETER_NAME_STRING( environment_variable_include );
  DEFINE_PARAMETER_NAME_STRING( environment_variable_lib );
  DEFINE_PARAMETER_NAME_STRING( environment_variable_libpath );
  DEFINE_PARAMETER_NAME_STRING( editor_path );
  DEFINE_PARAMETER_NAME_STRING( editor_argument );
  DEFINE_PARAMETER_NAME_STRING( external_programs );
  DEFINE_PARAMETER_NAME_STRING( structures );
  DEFINE_PARAMETER_NAME_STRING( settings );
  DEFINE_PARAMETER_NAME_STRING( project );
  DEFINE_PARAMETER_NAME_STRING( current_structure );
  DEFINE_PARAMETER_NAME_STRING( extension_script_path );
  DEFINE_PARAMETER_NAME_STRING( additional_make_commands );
  DEFINE_PARAMETER_NAME_STRING( target );
  DEFINE_PARAMETER_NAME_STRING( included_in_build );
  DEFINE_PARAMETER_NAME_STRING( included_in_clean );
  DEFINE_PARAMETER_NAME_STRING( depends );
  DEFINE_PARAMETER_NAME_STRING( commands );
  DEFINE_PARAMETER_NAME_STRING( clear );
  DEFINE_PARAMETER_NAME_STRING( add_text );
  DEFINE_PARAMETER_NAME_STRING( compiler_edit );
  DEFINE_PARAMETER_NAME_STRING( execute_edit );
  DEFINE_PARAMETER_NAME_STRING( external_program_edit );
  DEFINE_PARAMETER_NAME_STRING( main_tab );
  DEFINE_PARAMETER_NAME_STRING( build_functions );
  DEFINE_PARAMETER_NAME_STRING( first );
  DEFINE_PARAMETER_NAME_STRING( last );
  DEFINE_PARAMETER_NAME_STRING( before_compile );
  DEFINE_PARAMETER_NAME_STRING( after_compile );
  DEFINE_PARAMETER_NAME_STRING( before_build );
  DEFINE_PARAMETER_NAME_STRING( after_build );
  DEFINE_PARAMETER_NAME_STRING( before_clean );
  DEFINE_PARAMETER_NAME_STRING( after_clean );
  DEFINE_PARAMETER_NAME_STRING( system );
  DEFINE_PARAMETER_NAME_STRING( make_command_path );
  DEFINE_PARAMETER_NAME_STRING( compiler_x86 );
  DEFINE_PARAMETER_NAME_STRING( compiler_x64 );
  DEFINE_PARAMETER_NAME_STRING( Platform );
  DEFINE_PARAMETER_NAME_STRING( set_output_edit_of_print_output );
  DEFINE_PARAMETER_NAME_STRING( SubMenu );
  DEFINE_PARAMETER_NAME_STRING( items );
  DEFINE_PARAMETER_NAME_STRING( external_program_menu_normal );
  DEFINE_PARAMETER_NAME_STRING( external_program_menu_open_file );
  DEFINE_PARAMETER_NAME_STRING( ExternalProgramNative );
}


// -- SquirrelVM ---------------------------------------------------------
std::unordered_map<HSQUIRRELVM, SquirrelVM*>
SquirrelVM::HANDLE_TABLE;

void
SquirrelVM::PrintFunction( HSQUIRRELVM vm_, const SQChar* s, ... )
{
  SquirrelVM& vm = SquirrelVM::GetVM( vm_ );
  if ( vm.output_edit_ ) {
    va_list arglist;
    va_start( arglist, s );
    TtString::UniqueString buffer( _vscprintf( s, arglist ) );
    vsprintf_s( buffer.GetPointer(), buffer.GetCapacity(), s, arglist );
    va_end( arglist );

    vm.output_edit_->AddText( buffer.GetPointer() );
  }
}

TtSquirrel::VirtualMachine::Closure
SquirrelVM::ConvertClosure( std::function<int ( SquirrelVM& )> closure )
{
  return [closure] ( TtSquirrel::VirtualMachine& vm ) -> int {
    return closure( SquirrelVM::GetVM( vm ) );
  };
}


SquirrelVM&
SquirrelVM::GetVM( HSQUIRRELVM vm )
{
  auto it = HANDLE_TABLE.find( vm );
  if ( it == HANDLE_TABLE.end() ) {
    throw TTVC_DEVELOPER_INTERNAL_EXCEPTION;
  }
  return *it->second;
}

SquirrelVM&
SquirrelVM::GetVM( TtSquirrel::VirtualMachine& vm )
{
  return SquirrelVM::GetVM( vm.GetHandle() );
}


SquirrelVM::SquirrelVM( MainFrame& frame, bool use ) :
TtSquirrel::VirtualMachine(),
use_( use ),
output_edit_( nullptr )
{
  HANDLE_TABLE.insert( {this->GetHandle(), this} );
  if ( use_ ) {
    this->RegisterStandardLibraries();
    this->Initialize();
    this->InitializeMainFrame( frame );

    Native().SetPrintFunction( SquirrelVM::PrintFunction, SquirrelVM::PrintFunction );
  }
}

SquirrelVM::~SquirrelVM()
{
  auto it = HANDLE_TABLE.find( this->GetHandle() );
  if ( it != HANDLE_TABLE.end() ) {
    HANDLE_TABLE.erase( it );
  }
}


void
SquirrelVM::Initialize( void )
{
  // -- Settings 実装 ----------
  this->NewSlotOfRootTableByString(
    Tag::Settings,
    [&] () {
      TtSquirrel::StackRecoverer recoverer( this, 1 );
      Native().NewClass( false );
      Native().SetTypeTag( TtSquirrel::Const::StackTop, TtSquirrel::TypeTag::Create<Settings>() );

      // -- constructor 定義 -----
      this->NewSlotOfTopByString(
        Tag::constructor,
        [&] () {
          this->NewClosure( SquirrelVM::ConvertClosure( [] ( SquirrelVM& vm ) -> int {
            vm.Native().SetInstanceUserPointer( TtSquirrel::Utility::PushedFromTop( 1 ), vm.GetAsFromTop<SQUserPointer>() );

            Settings& self = *vm.GetAsPointerOf<Settings>( TtSquirrel::Const::StackTop );
            vm.Native().PopTop();

            auto compiler_set = [&] ( Platform platform, const std::string& key ) {
              vm.SetToTopByString(
                key,
                [&] () {
                  vm.Native().NewTable();

                  auto compiler = self.GetCompilerByPlatform( platform );
                  vm.NewStringSlotOfTopByString( Tag::make_command_path,            compiler.make_command_path_ );
                  vm.NewStringSlotOfTopByString( Tag::compiler_path,                compiler.compiler_path_ );
                  vm.NewStringSlotOfTopByString( Tag::resource_compiler_path,       compiler.resource_compiler_path_ );
                  vm.NewStringSlotOfTopByString( Tag::environment_variable_include, compiler.environment_variable_include_ );
                  vm.NewStringSlotOfTopByString( Tag::environment_variable_lib,     compiler.environment_variable_lib_ );
                  vm.NewStringSlotOfTopByString( Tag::environment_variable_libpath, compiler.environment_variable_libpath_ );
                } );
            };
            compiler_set( Platform::X86, Tag::compiler_x86 );
            compiler_set( Platform::X64, Tag::compiler_x64 );

            vm.SetStringToTopByString( Tag::editor_path,     self.editor_path_     );
            vm.SetStringToTopByString( Tag::editor_argument, self.editor_argument_ );

            vm.SetToTopByString(
              Tag::external_programs,
              [&] () {
                vm.NewArrayOf( self.external_programs_, [&] ( const Settings::ExternalProgram& one ) {
                  vm.CallAndPushReturnValue(
                    [&] () { vm.GetByStringFromRootTableJoined( {Tag::Settings, Tag::ExternalProgramNative} ); },
                    [&] () {
                      vm.Native().PushRootTable();
                      vm.PushAsUserPointer( &one );
                      return 2;
                    } );
                } );
              } );

            return TtSquirrel::Const::NoneReturnValue;
          } ) );
          Native().SetParamsCheck( 2, "xp" );
        } );

      // -- プロパティ
      this->NewNullSlotOfTopByString( Tag::compiler_x86 );
      this->NewNullSlotOfTopByString( Tag::compiler_x64 );
      this->NewNullSlotOfTopByString( Tag::editor_path );
      this->NewNullSlotOfTopByString( Tag::editor_argument );
      this->NewNullSlotOfTopByString( Tag::external_programs );

      // -- Settings::ExternalProgram 実装 ----------
      this->NewSlotOfTopByStringAsStatic(
        Tag::ExternalProgram,
        [&] () {
          TtSquirrel::StackRecoverer recoverer( this, 1 );
          Native().NewClass( false );

          // -- constructor 定義 -----
          this->NewSlotOfTopByString(
            Tag::constructor,
            [&] () {
              this->NewClosure( SquirrelVM::ConvertClosure( [] ( SquirrelVM& vm ) -> int {
                std::string tmp = vm.GetAsFromTop<std::string>();
                vm.Native().PopTop();
                vm.SetStringToTopByString( Tag::name, tmp );
                return TtSquirrel::Const::NoneReturnValue;
              } ) );
              Native().SetParamsCheck( 2, "xs" );
            } );

          // -- プロパティ
          this->NewStringSlotOfTopByString(  Tag::name,              ""    );
          this->NewBooleanSlotOfTopByString( Tag::for_open_file,     false );
          this->NewStringSlotOfTopByString(  Tag::path,              ""    );
          this->NewStringSlotOfTopByString(  Tag::argument,          ""    );
          this->NewStringSlotOfTopByString(  Tag::current_directory, ""    );
          this->NewBooleanSlotOfTopByString( Tag::use_output_edit,   false );
          this->NewBooleanSlotOfTopByString( Tag::use_input_dialog,  false );
        } );

      // -- Settings::ExternalProgramNative 実装 ----------
      this->NewSlotOfTopByStringAsStatic(
        Tag::ExternalProgramNative,
        [&] () {
          TtSquirrel::StackRecoverer recoverer( this, 1 );
          this->GetByStringFrom( TtSquirrel::Utility::PushedFromTop( 1 ), Tag::ExternalProgram );
          Native().NewClass( true );
          Native().SetTypeTag( TtSquirrel::Const::StackTop, TtSquirrel::TypeTag::Create<Settings::ExternalProgram>() );

          // -- constructor 定義 -----
          this->NewSlotOfTopByString(
            Tag::constructor,
            [&] () {
              this->NewClosure( SquirrelVM::ConvertClosure( [] ( SquirrelVM& vm ) -> int {
                vm.Native().SetInstanceUserPointer( TtSquirrel::Utility::PushedFromTop( 1 ), vm.GetAsFromTop<SQUserPointer>() );

                Settings::ExternalProgram& self = *vm.GetAsPointerOf<Settings::ExternalProgram>( TtSquirrel::Const::StackTop );
                vm.Native().PopTop();

                vm.SetStringToTopByString(  Tag::name,              self.name_ );
                vm.SetBooleanToTopByString( Tag::for_open_file,     self.for_open_file_ );
                vm.SetStringToTopByString(  Tag::path,              self.path_ );
                vm.SetStringToTopByString(  Tag::argument,          self.argument_ );
                vm.SetStringToTopByString(  Tag::current_directory, self.current_directory_ );
                vm.SetBooleanToTopByString( Tag::use_output_edit,   self.use_output_edit_ );
                vm.SetBooleanToTopByString( Tag::use_input_dialog,  self.use_input_dialog_ );

                return TtSquirrel::Const::NoneReturnValue;
              } ) );
              Native().SetParamsCheck( 2, "xp" );
            } );
        } );
    } );

  // -- Project 実装 ----------
  this->NewSlotOfRootTableByString(
    Tag::Project,
    [&] () {
      TtSquirrel::StackRecoverer recoverer( this, 1 );
      Native().NewClass( false );
      Native().SetTypeTag( TtSquirrel::Const::StackTop, TtSquirrel::TypeTag::Create<Project>() );

      // -- Project.TargetKind 定義
      this->NewSlotOfTopByStringAsStatic( Tag::TargetKind, [&] () { this->NewTableOfEnum<Project::TargetKind>(); } );

      // -- Project.OptimizeKind 定義
      this->NewSlotOfTopByStringAsStatic( Tag::OptimizeKind, [&] () { this->NewTableOfEnum<Project::OptimizeKind>(); } );

      // -- Project.RuntimeLibrary 定義
      this->NewSlotOfTopByStringAsStatic( Tag::RuntimeLibrary, [&] () { this->NewTableOfEnum<Project::RuntimeLibrary>(); } );

      // -- constructor 定義 -----
      this->NewSlotOfTopByString(
        Tag::constructor,
        [&] () {
          this->NewClosure( SquirrelVM::ConvertClosure( [] ( SquirrelVM& vm ) -> int {
            vm.Native().SetInstanceUserPointer( TtSquirrel::Utility::PushedFromTop( 1 ), vm.GetAsFromTop<SQUserPointer>() );

            Project& self = *vm.GetAsPointerOf<Project>( TtSquirrel::Const::StackTop );
            vm.Native().PopTop();

            vm.SetStringToTopByString(  Tag::path,            self.path_ );
            vm.SetStringToTopByString(  Tag::name,            self.name_ );
            vm.SetBooleanToTopByString( Tag::use_icon,        self.use_icon_ );
            vm.SetStringToTopByString(  Tag::icon_file_path,  self.icon_file_path_ );
            vm.SetIntegerToTopByString( Tag::icon_file_index, self.icon_file_index_ );

            vm.SetToTopByString(
              Tag::structures,
              [&] () {
                vm.NewArrayOf( self.GetStructures(), [&] ( const std::shared_ptr<Project::Structure>& one ) {
                  vm.CallAndPushReturnValue(
                    [&] () { vm.GetByStringFromRootTableJoined( {Tag::Project, Tag::Structure} ); },
                    [&] () {
                      vm.Native().PushRootTable();
                      vm.PushAsUserPointer( one.get() );
                      return 2;
                    } );
                } );
              } );

            return TtSquirrel::Const::NoneReturnValue;
          } ) );
          Native().SetParamsCheck( 2, "xp" );
        } );

      // -- プロパティ
      this->NewNullSlotOfTopByString( Tag::path );
      this->NewNullSlotOfTopByString( Tag::name );
      this->NewNullSlotOfTopByString( Tag::use_icon );
      this->NewNullSlotOfTopByString( Tag::icon_file_path );
      this->NewNullSlotOfTopByString( Tag::icon_file_index );
      this->NewNullSlotOfTopByString( Tag::structures );

      // -- Project::Structure 実装 ----------
      this->NewSlotOfTopByStringAsStatic(
        Tag::Structure,
        [&] () {
          TtSquirrel::StackRecoverer recoverer( this, 1 );
          Native().NewClass( false );
          Native().SetTypeTag( TtSquirrel::Const::StackTop, TtSquirrel::TypeTag::Create<Project::Structure>() );

          // -- constructor 定義 -----
          this->NewSlotOfTopByString(
            Tag::constructor,
            [&] () {
              this->NewClosure( SquirrelVM::ConvertClosure( [] ( SquirrelVM& vm ) -> int {
                vm.Native().SetInstanceUserPointer( TtSquirrel::Utility::PushedFromTop( 1 ), vm.GetAsFromTop<SQUserPointer>() );

                Project::Structure& self = *vm.GetAsPointerOf<Project::Structure>( TtSquirrel::Const::StackTop );
                vm.Native().PopTop();

                auto set_string_vector = [&] ( const std::string& name, const std::vector<std::string>& v ) {
                  vm.SetToTopByString( name, [&] ( void ) {
                    vm.NewArrayOf( v, [&] ( const std::string& one ) { vm.Native().PushString( one ); } );
                  } );
                };
                auto set_int_vector = [&] ( const std::string& name, const std::vector<int>& v ) {
                  vm.SetToTopByString( name, [&] ( void ) {
                    vm.NewArrayOf( v, [&] ( const int& one ) { vm.Native().PushInteger( one ); } );
                  } );
                };

                vm.SetStringToTopByString(  Tag::name,                             self.name_ );
                vm.SetIntegerToTopByString( Tag::target_kind,                      self.target_kind_ );
                vm.SetStringToTopByString(  Tag::output_directory_name,            self.output_directory_name_ );
                vm.SetStringToTopByString(  Tag::target_name,                      self.target_name_ );
                vm.SetStringToTopByString(  Tag::target_argument,                  self.target_argument_ );
                vm.SetStringToTopByString(  Tag::target_current_directory,         self.target_current_directory_ );
                vm.SetBooleanToTopByString( Tag::target_use_output_edit,           self.target_use_output_edit_ );
                vm.SetBooleanToTopByString( Tag::target_use_input_dialog,          self.target_use_input_dialog_ );
                vm.SetIntegerToTopByString( Tag::optimize_kind,                    self.optimize_kind_ );
                vm.SetIntegerToTopByString( Tag::runtime_library,                  self.runtime_library_ );
                vm.SetBooleanToTopByString( Tag::use_exception,                    self.use_exception_ );
                vm.SetBooleanToTopByString( Tag::use_precompile_header,            self.use_precompile_header_ );
                vm.SetStringToTopByString(  Tag::precompiled_header_source,        self.precompiled_header_source_ );
                vm.SetStringToTopByString(  Tag::precompiled_header_header,        self.precompiled_header_header_ );
                vm.SetBooleanToTopByString( Tag::precompiled_header_force_include, self.precompiled_header_force_include_ );
                set_string_vector(          Tag::include_paths,                    self.include_paths_ );
                set_string_vector(          Tag::defines,                          self.defines_ );
                vm.SetIntegerToTopByString( Tag::warning_level,                    self.warning_level_ );
                set_int_vector(             Tag::no_warnings,                      self.no_warnings_ );
                vm.SetBooleanToTopByString( Tag::error_if_warning,                 self.error_if_warning_ );
                set_string_vector(          Tag::library_paths,                    self.library_paths_ );
                vm.SetBooleanToTopByString( Tag::no_logo,                          self.no_logo_ );
                vm.SetStringToTopByString(  Tag::compiler_other_option,            self.compiler_other_option_ );
                vm.SetStringToTopByString(  Tag::resource_compiler_option,         self.resource_compiler_option_ );
                vm.SetStringToTopByString(  Tag::extension_script_path,            self.extension_script_path_ );

                vm.SetToTopByString( Tag::additional_make_commands, [&] () { vm.Native().NewArray( 0 ); } );
                vm.SetToTopByString( Tag::build_functions, [&] () {
                  vm.Native().NewTable();
                  vm.NewNullSlotOfTopByString( Tag::first );
                  vm.NewNullSlotOfTopByString( Tag::last );
                  vm.NewNullSlotOfTopByString( Tag::before_compile );
                  vm.NewNullSlotOfTopByString( Tag::after_compile );
                  vm.NewNullSlotOfTopByString( Tag::before_build );
                  vm.NewNullSlotOfTopByString( Tag::after_build );
                  vm.NewNullSlotOfTopByString( Tag::before_clean );
                  vm.NewNullSlotOfTopByString( Tag::after_clean );
                } );

                return TtSquirrel::Const::NoneReturnValue;
              } ) );
              Native().SetParamsCheck( 2, "xp" );
            } );

          // -- プロパティ
          this->NewNullSlotOfTopByString( Tag::name );
          this->NewNullSlotOfTopByString( Tag::target_kind );
          this->NewNullSlotOfTopByString( Tag::output_directory_name );
          this->NewNullSlotOfTopByString( Tag::target_name );
          this->NewNullSlotOfTopByString( Tag::target_argument );
          this->NewNullSlotOfTopByString( Tag::target_current_directory );
          this->NewNullSlotOfTopByString( Tag::target_use_output_edit );
          this->NewNullSlotOfTopByString( Tag::target_use_input_dialog );
          this->NewNullSlotOfTopByString( Tag::optimize_kind );
          this->NewNullSlotOfTopByString( Tag::runtime_library );
          this->NewNullSlotOfTopByString( Tag::use_exception );
          this->NewNullSlotOfTopByString( Tag::use_precompile_header );
          this->NewNullSlotOfTopByString( Tag::precompiled_header_source );
          this->NewNullSlotOfTopByString( Tag::precompiled_header_header );
          this->NewNullSlotOfTopByString( Tag::precompiled_header_force_include );
          this->NewNullSlotOfTopByString( Tag::include_paths );
          this->NewNullSlotOfTopByString( Tag::defines );
          this->NewNullSlotOfTopByString( Tag::warning_level );
          this->NewNullSlotOfTopByString( Tag::no_warnings );
          this->NewNullSlotOfTopByString( Tag::error_if_warning );
          this->NewNullSlotOfTopByString( Tag::library_paths );
          this->NewNullSlotOfTopByString( Tag::no_logo );
          this->NewNullSlotOfTopByString( Tag::compiler_other_option );
          this->NewNullSlotOfTopByString( Tag::resource_compiler_option );
          this->NewNullSlotOfTopByString( Tag::extension_script_path );

          this->NewNullSlotOfTopByString( Tag::additional_make_commands );
          this->NewNullSlotOfTopByString( Tag::build_functions );
        } );
    } );

  // -- MakeCommandEntry ----------
  {
    std::string tmp = R"template(
class MakeCommandEntry {
  constructor ( target ) {
    this.target = target;
    this.included_in_build = false;
    this.included_in_clean = false;
    this.depends = [];
    this.commands = [];
  }

  target            = null;
  included_in_build = false;
  included_in_clean = false;
  depends           = null;
  commands          = null;
}
)template";
    this->DoBufferAtRoot( tmp, false, FILE_LINE );
  }

  // -- OutputEdit 実装 ----------
  this->NewSlotOfRootTableByString(
    Tag::OutputEdit,
    [&] () {
      TtSquirrel::StackRecoverer recoverer( this, 1 );
      Native().NewClass( false );
      Native().SetTypeTag( TtSquirrel::Const::StackTop, TtSquirrel::TypeTag::Create<OutputEdit>() );

      // -- constructor 定義 -----
      this->NewSlotOfTopByString(
        Tag::constructor,
        [&] () {
          this->NewClosure( SquirrelVM::ConvertClosure( [] ( SquirrelVM& vm ) -> int {
            vm.Native().SetInstanceUserPointer( TtSquirrel::Utility::PushedFromTop( 1 ), vm.GetAsFromTop<SQUserPointer>() );
            return TtSquirrel::Const::NoneReturnValue;
          } ) );
          Native().SetParamsCheck( 2, "xp" );
        } );

      // -- clear 定義
      this->NewSlotOfTopByString(
        Tag::clear,
        [&] () {
          this->NewClosure( SquirrelVM::ConvertClosure( [] ( SquirrelVM& vm ) -> int {
            OutputEdit& self = *vm.GetInstanceUserPointerAs<OutputEdit>( TtSquirrel::Const::StackTop );
            self.Clear();
            return TtSquirrel::Const::NoneReturnValue;
          } ) );
          Native().SetParamsCheck( 1, "x" );
        } );

      // -- add_text 定義
      this->NewSlotOfTopByString(
        Tag::add_text,
        [&] () {
          this->NewClosure( SquirrelVM::ConvertClosure( [] ( SquirrelVM& vm ) -> int {
            std::string text = vm.GetAsFromTop<std::string>();
            OutputEdit& self = *vm.GetInstanceUserPointerAs<OutputEdit>( TtSquirrel::Utility::PushedFromTop( 1 ) );
            self.AddText( text );
            return TtSquirrel::Const::NoneReturnValue;
          } ) );
          Native().SetParamsCheck( 2, "xs" );
        } );
    } );


  // -- MainTab 実装 ----------
  this->NewSlotOfRootTableByString(
    Tag::MainTab,
    [&] () {
      TtSquirrel::StackRecoverer recoverer( this, 1 );
      Native().NewClass( false );
      Native().SetTypeTag( TtSquirrel::Const::StackTop, TtSquirrel::TypeTag::Create<MainTab>() );

      // -- constructor 定義 -----
      this->NewSlotOfTopByString(
        Tag::constructor,
        [&] () {
          this->NewClosure( SquirrelVM::ConvertClosure( [] ( SquirrelVM& vm ) -> int {
            vm.Native().SetInstanceUserPointer( TtSquirrel::Utility::PushedFromTop( 1 ), vm.GetAsFromTop<SQUserPointer>() );

            MainTab& self = *vm.GetAsPointerOf<MainTab>( TtSquirrel::Const::StackTop );
            vm.Native().PopTop();

            auto register_output_edit = [&] ( const std::string& name, OutputEdit& edit ) {
              vm.SetToTopByString(
                name,
                [&] () {
                  vm.CallAndPushReturnValue(
                    [&] () { vm.GetByStringFromRootTable( Tag::OutputEdit ); },
                    [&] () {
                      vm.Native().PushRootTable();
                      vm.PushAsUserPointer( &edit );
                      return 2;
                    } );
                } );
            };
            register_output_edit( Tag::compiler_edit,         self.GetCompilerEdit() );
            register_output_edit( Tag::execute_edit,          self.GetExecuteEdit() );
            register_output_edit( Tag::external_program_edit, self.GetExternalProgramEdit() );

            return TtSquirrel::Const::NoneReturnValue;
          } ) );
          Native().SetParamsCheck( 2, "xp" );
        } );

      // -- プロパティ
      this->NewNullSlotOfTopByString( Tag::compiler_edit );
      this->NewNullSlotOfTopByString( Tag::execute_edit );
      this->NewNullSlotOfTopByString( Tag::external_program_edit );
    } );

  // -- SubMenu ----------
  {
    std::string tmp = R"template(
class SubMenu {
  static Separator = null;

  constructor( name, items ) {
    this.name = name;
    this.items = items;
  }

  name = null;
  items = null;
}
)template";
    this->DoBufferAtRoot( tmp, false, FILE_LINE );
  }

}


void
SquirrelVM::InitializeMainFrame( MainFrame& frame )
{
  Developer& developer = frame.GetDeveloper();
  // -- Developer 実装 ----------
  this->NewSlotOfRootTableByString(
    Tag::Developer,
    [&] () {
      TtSquirrel::StackRecoverer recoverer( this, 1 );
      Native().NewTable();

      // -- system 定義
      this->NewSlotOfTopByString(
        Tag::system,
        [&] () {
          this->NewClosure( SquirrelVM::ConvertClosure( [] ( SquirrelVM& vm ) -> int {
            std::string str = vm.GetAsFromTop<std::string>();
            vm.Native().PopTop();

            // 自作 system
            {
              TtPipe pipe;
              TtPipe::Handle for_error = pipe.GetWritePipeHandle().Duplicate( true );
              pipe.GetReadPipeHandle().SetInherit( false );

              TtProcess::CreateInfo tmp_info;
              tmp_info.filename_ = "cmd.exe";
              tmp_info.arguments_ = "/C "+ str;
              tmp_info.show_state_ = TtWindow::ShowState::HIDE;

              tmp_info.inherit_handles_ = true;
              tmp_info.standard_output_ = pipe.GetWritePipeHandle();
              tmp_info.standard_error_ = for_error;

              TtProcess process;
              try {
                process.Create( tmp_info );
              }
              catch ( TtWindowsSystemCallException& ) {
                throw TtSquirrel::RuntimeException( "CreateProcess Error", vm.GetCallStack() );
              }
              pipe.GetWritePipeHandle().Close();
              for_error.Close();

              TtString::UniqueString buffer( 1024 );
              for (;;) {
                DWORD received;
                if ( ::ReadFile( pipe.GetReadPipeHandle().GetWindowsHandle(),
                                 buffer.GetPointer(), buffer.GetCapacity() - 1, &received, NULL ) == 0 ) {
                  break;
                }
                buffer.GetPointer()[received] = '\0';
                vm.CallPrint( buffer.GetPointer() );
              }
              process.Wait();
              pipe.GetReadPipeHandle().Close();
            }
            return TtSquirrel::Const::ExistReturnValue;
          } ) );
          Native().SetParamsCheck( 2, ".s" );
        } );

      // -- project_variables_translate 定義 -----
      this->NewSlotOfTopByString(
        Tag::project_variables_translate,
        [&] () {
          this->NewClosure( SquirrelVM::ConvertClosure( [&] ( SquirrelVM& vm ) -> int {
            Developer::ProjectVariablesTranslator translator( developer );
            translator.SetUserInputHandlerAndParentOfDialog( "スクリプトへの入力", frame );
            std::string tmp = vm.GetAsFromTop<std::string>();
            vm.Native().PushString( translator.Translate( tmp ) );
            return TtSquirrel::Const::ExistReturnValue;
          } ) );
          Native().SetParamsCheck( 2, "ts" );
        } );

      // -- set_output_edit_of_print_output 定義 -----
      this->NewSlotOfTopByString(
        Tag::set_output_edit_of_print_output,
        [&] () {
          this->NewClosure( SquirrelVM::ConvertClosure( [&] ( SquirrelVM& vm ) -> int {
            if ( vm.GetTopType() == TtSquirrel::ObjectType::Null ) {
              vm.output_edit_ = nullptr;
            }
            else if ( vm.InstanceAtTopOf( [&] () { vm.GetByStringFromRootTable( Tag::OutputEdit ); } ) ) {
              vm.output_edit_ = vm.GetInstanceUserPointerAs<OutputEdit>( TtSquirrel::Const::StackTop );
            }
            else {
              throw TtSquirrel::RuntimeException( "Argument is not Instance of OutputEdit class", vm.GetCallStack() );
            }
            return TtSquirrel::Const::NoneReturnValue;
          } ) );
          Native().SetParamsCheck( 2, "to|x" );
        } );

      // -- Platform 定義
      this->NewSlotOfTopByString( Tag::Platform, [&] () { this->NewTableOfEnum<Platform>(); } );

      // -- settings
      this->NewSlotOfTopByString(
        Tag::settings,
        [&] () {
          this->CallAndPushReturnValue(
            [&] () { this->GetByStringFromRootTable( Tag::Settings ); },
            [&] () {
              this->Native().PushRootTable();
              this->PushAsUserPointer( &developer.GetSettings() );
              return 2;
            } );
        } );

      // -- project
      this->NewSlotOfTopByString(
        Tag::project,
        [&] () {
          this->CallAndPushReturnValue(
            [&] () { this->GetByStringFromRootTable( Tag::Project ); },
            [&] () {
              this->Native().PushRootTable();
              this->PushAsUserPointer( &developer.GetProject() );
              return 2;
            } );
        } );

      {
        TtSquirrel::Object developer_object = this->GetStackTopObject( true );
        // -- current_structure
        this->NewSlotOfTopByString(
          Tag::current_structure,
          [&] () {
            auto result = TtUtility::FindIf(
              developer.GetProject().GetStructures(),
              [&] ( const std::shared_ptr<Project::Structure>& one ) -> bool {
                return one.get() == &developer.GetCurrentStructure();
              } );
            if ( result == developer.GetProject().GetStructures().end() ) {
              throw TTVC_DEVELOPER_INTERNAL_EXCEPTION;
            }
            int current_index = result - developer.GetProject().GetStructures().begin();

            this->RemainTop( [&] () {
              this->PushObject( developer_object );
              this->GetByStringFromTop( Tag::project );
              this->GetByStringFromTop( Tag::structures );
              this->GetByIntegerFromTop( current_index );
            } );
          } );
      }

      // -- main_tab
      this->NewSlotOfTopByString(
        Tag::main_tab,
        [&] () {
          this->CallAndPushReturnValue(
            [&] () { this->GetByStringFromRootTable( Tag::MainTab ); },
            [&] () {
              this->Native().PushRootTable();
              this->PushAsUserPointer( &frame.GetMainTab() );
              return 2;
            } );
        } );

      // -- 補助情報
      this->NewStringSlotOfTopByString( Tag::project_path,           developer.GetProjectPath() );
      this->NewStringSlotOfTopByString( Tag::project_directory_path, developer.GetProjectDirectoryPath() );
      this->NewStringSlotOfTopByString( Tag::output_directory_path,  developer.GetOutputDirectoryPath() );
      this->NewStringSlotOfTopByString( Tag::makefile_path,          developer.GetMakefilePath() );
      this->NewStringSlotOfTopByString( Tag::target_file_path,       developer.GetTargetFilePath() );
    } );

  // -- Developer.external_program_menu_normal _open_file 作成 ----------
  {
    std::string tmp = R"template(
Developer.external_program_menu_normal    <- SubMenu( "", [] );
Developer.external_program_menu_open_file <- SubMenu( "", [] );
foreach ( program in Developer.settings.external_programs ) {
  if ( program.for_open_file ) {
    Developer.external_program_menu_open_file.items.append( clone program );
  }
  else {
    Developer.external_program_menu_normal.items.append( clone program );
  }
}
)template";
    this->DoBufferAtRoot( tmp, false, FILE_LINE );
  }
}


std::vector<Project::MakeCommandEntry>
SquirrelVM::GetMakeCommandEntries( void )
{
  TtSquirrel::StackRecoverer recoverer_function( this );
  this->GetByStringFromRootTableJoined( {Tag::Developer, Tag::current_structure, Tag::additional_make_commands} );

  std::vector<Project::MakeCommandEntry> v;
  int target_count = Native().GetSize( TtSquirrel::Const::StackTop );

  for ( int i = 0; i < target_count; ++i ) {
    TtSquirrel::StackRecoverer recoverer_loop( this );
    this->GetByIntegerFromTop( i );

    Project::MakeCommandEntry tmp;
    tmp.target_            = this->GetByStringFromTopAndGetAs<std::string>( Tag::target );
    tmp.included_in_build_ = this->GetByStringFromTopAndGetAs<bool>(        Tag::included_in_build );
    tmp.included_in_clean_ = this->GetByStringFromTopAndGetAs<bool>(        Tag::included_in_clean );
    {
      TtSquirrel::StackRecoverer recoverer( this );
      this->GetByStringFromTop( Tag::depends );
      int depends_count = Native().GetSize( TtSquirrel::Const::StackTop );
      for ( int k = 0; k < depends_count; ++k ) {
        tmp.depends_.push_back( this->GetByIntegerFromTopAndGetAs<std::string>( k ) );
      }
    }
    {
      TtSquirrel::StackRecoverer recoverer( this );
      this->GetByStringFromTop( Tag::commands );
      int commands_count = Native().GetSize( TtSquirrel::Const::StackTop );
      for ( int k = 0; k < commands_count; ++k ) {
        tmp.commands_.push_back( this->GetByIntegerFromTopAndGetAs<std::string>( k ) );
      }
    }
    v.push_back( tmp );
  }
  return v;
}

TtSubMenuCommandMaker
SquirrelVM::GetSubMenuCommandMakerOfExternalProgramNormal( void )
{
  return this->GetSubMenuCommandMakerOf( Tag::external_program_menu_normal );
}

TtSubMenuCommandMaker
SquirrelVM::GetSubMenuCommandMakerOfExternalProgramOpenFile( void )
{
  return this->GetSubMenuCommandMakerOf( Tag::external_program_menu_open_file );
}


TtSubMenuCommandMaker
SquirrelVM::GetSubMenuCommandMakerOf( const std::string& key )
{
  using ElementVector = TtSubMenuCommandMaker::ElementVector;
  using Item          = TtSubMenuCommandMaker::Item;
  using Separator     = TtSubMenuCommandMaker::Separator;
  using SubMenu       = TtSubMenuCommandMaker::SubMenu;

  class ExternalProgramItem : public Item {
  public:
    explicit ExternalProgramItem( Settings::ExternalProgram& program ) : program_( program ) {}
    virtual std::string GetName( void ) const override {
      return program_.name_;
    };
    virtual void AfterMake( TtMenuItem& item ) override {
      item.SetParameterAs<Settings::ExternalProgram*>( &program_ );
    }

    Settings::ExternalProgram program_;
  };

  TtSubMenuCommandMaker maker;
  std::function<void ( ElementVector& )> func = [this, &func] ( ElementVector& current ) {
    unsigned int max = this->Native().GetSize( TtSquirrel::Const::StackTop );
    for ( unsigned int i = 0; i < max; ++i ) {
      TtSquirrel::StackRecoverer recoverer( this );

      this->GetByIntegerFromTop( i );
      if ( this->GetTopType() == TtSquirrel::ObjectType::Null ) {
        current.push_back( std::make_shared<Separator>() );
      }
      else if ( this->InstanceAtTopOf( [&] () { this->GetByStringFromRootTable( Tag::SubMenu ); } ) ) {
        TtSquirrel::StackRecoverer tmp( this );
        SubMenu sub;
        sub.name_ = this->GetByStringFromTopAndGetAs<std::string>( Tag::name );
        this->GetByStringFromTop( Tag::items );
        func( sub.elements_ );
        current.push_back( std::make_shared<SubMenu>( sub ) );
      }
      else {
        Settings::ExternalProgram tmp;
        tmp.name_              = this->GetByStringFromTopAndGetAs<std::string>( Tag::name              );
        tmp.for_open_file_     = this->GetByStringFromTopAndGetAs<bool>(        Tag::for_open_file     );
        tmp.path_              = this->GetByStringFromTopAndGetAs<std::string>( Tag::path              );
        tmp.argument_          = this->GetByStringFromTopAndGetAs<std::string>( Tag::argument          );
        tmp.current_directory_ = this->GetByStringFromTopAndGetAs<std::string>( Tag::current_directory );
        tmp.use_output_edit_   = this->GetByStringFromTopAndGetAs<bool>(        Tag::use_output_edit   );
        tmp.use_input_dialog_  = this->GetByStringFromTopAndGetAs<bool>(        Tag::use_input_dialog  );
        current.push_back( std::make_shared<ExternalProgramItem>( tmp ) );
      }
    }
  };
  this->GetByStringFromRootTableJoined( {Tag::Developer, key, Tag::items} );
  func( maker.GetRoot() );
  return maker;
}


bool
SquirrelVM::CallBuildFunction( const std::string& name, std::function<int ( void )> callback, bool default_result )
{
  TtSquirrel::StackRecoverer recoverer( this );

  this->GetByStringFromRootTableJoined( {Tag::Developer, Tag::current_structure, Tag::build_functions, name} );
  if ( this->GetTopType().IsCallable() ) {
    this->CallAndPushReturnValue(
      [&] () { Native().Push( TtSquirrel::Const::StackTop ); },
      [&] () {
        this->GetByStringFromRootTableJoined( {Tag::Developer, Tag::current_structure, Tag::build_functions} );
        return 1 + callback();
      } );
    if ( this->GetTopType() == TtSquirrel::ObjectType::Boolean ) {
      return this->GetAsFromTop<bool>();
    }
  }
  else if ( this->GetTopType() != TtSquirrel::ObjectType::Null ) {
    throw TtSquirrel::ObjectTypeException( this->GetTopType(), TtSquirrel::ObjectType::Closure );
  }
  return default_result;
}


bool
SquirrelVM::CallBuildFunctionFirst( void )
{
  return this->CallBuildFunction( Tag::first, [&] () { return 0; } );
}

bool
SquirrelVM::CallBuildFunctionLast( bool exist_error )
{
  return this->CallBuildFunction( Tag::last, [&] () {
    Native().PushBoolean( exist_error );
    return 1;
  } );
}

bool
SquirrelVM::CallBuildFunctionBeforeCompile( const std::string& target )
{
  return this->CallBuildFunction( Tag::before_compile, [&] () {
    Native().PushString( target );
    return 1;
  } );
}

bool
SquirrelVM::CallBuildFunctionAfterCompile( const std::string& target, DWORD exit_code )
{
  return this->CallBuildFunction( Tag::after_compile, [&] () {
    Native().PushString( target );
    Native().PushInteger( exit_code );
    return 2;
  }, exit_code == 0  );
}

bool
SquirrelVM::CallBuildFunctionBeforeBuild( void )
{
  return this->CallBuildFunction( Tag::before_build, [&] () { return 0; } );
}

bool
SquirrelVM::CallBuildFunctionAfterBuild( DWORD exit_code )
{
  return this->CallBuildFunction( Tag::after_build, [&] () {
    Native().PushInteger( exit_code );
    return 1;
  }, exit_code == 0 );
}

bool
SquirrelVM::CallBuildFunctionBeforeClean( void )
{
  return this->CallBuildFunction( Tag::before_clean, [&] () { return 0; } );
}

bool
SquirrelVM::CallBuildFunctionAfterClean( DWORD exit_code )
{
  return this->CallBuildFunction( Tag::after_clean, [&] () {
    Native().PushInteger( exit_code );
    return 1;
  }, exit_code == 0 );
}
