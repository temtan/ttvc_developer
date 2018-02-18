// squirrel_vm.h

#pragma once

#include "tt_menu.h"

#include "tt_squirrel_virtual_machine.h"

#include "project.h"


namespace TTVCDeveloper {
  class Developer;
  class MainFrame;
  class OutputEdit;

  // -- SquirrelVM -------------------------------------------------------
  class SquirrelVM : public TtSquirrel::VirtualMachine {
  private:
    static std::unordered_map<HSQUIRRELVM, SquirrelVM*> HANDLE_TABLE;

    static void PrintFunction( HSQUIRRELVM vm, const SQChar* s, ... );

    static TtSquirrel::VirtualMachine::Closure ConvertClosure( std::function<int ( SquirrelVM& )> closure );

  public:
    static SquirrelVM& GetVM( HSQUIRRELVM vm );
    static SquirrelVM& GetVM( TtSquirrel::VirtualMachine& vm );

  public:
    explicit SquirrelVM( MainFrame& frame, bool use );
    virtual ~SquirrelVM();

    void Initialize( void );
    void InitializeMainFrame( MainFrame& frame );

    std::vector<Project::MakeCommandEntry> GetMakeCommandEntries( void );

    TtSubMenuCommandMaker GetSubMenuCommandMakerOfExternalProgramNormal( void );
    TtSubMenuCommandMaker GetSubMenuCommandMakerOfExternalProgramOpenFile( void );
    TtSubMenuCommandMaker GetSubMenuCommandMakerOf( const std::string& key );

    bool CallBuildFunction( const std::string& name, std::function<int ( void )> callback, bool default_result = true );

    bool CallBuildFunctionFirst( void );
    bool CallBuildFunctionLast( bool exist_error );
    bool CallBuildFunctionBeforeCompile( const std::string& target );
    bool CallBuildFunctionAfterCompile( const std::string& target, DWORD exit_code );
    bool CallBuildFunctionBeforeBuild( void );
    bool CallBuildFunctionAfterBuild( DWORD exit_code );
    bool CallBuildFunctionBeforeClean( void );
    bool CallBuildFunctionAfterClean( DWORD exit_code );

  public:
    bool        use_;
    OutputEdit* output_edit_;
  };
}
