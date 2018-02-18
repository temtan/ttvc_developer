// developer.h

#pragma once

#include "tt_process.h"
#include "tt_form.h"

#include "project.h"
#include "settings.h"
#include "squirrel_vm.h"


namespace TTVCDeveloper {
  // -- Developer --------------------------------------------------------
  class Developer {
  public:
    explicit Developer( void );

    bool IsOperable( void );

    Settings& GetSettings( void );
    Project&  GetProject( void );

    void SetCurrentStructure( Project::Structure* structure );
    void SetCurrentStructureByIndex( int index );
    Project::Structure& GetCurrentStructure( void );
    Settings::Compiler& GetCurrentCompilerSettings( void );

    void NewProject( const std::string& path );
    void OpenProject( const std::string& path );

    std::string GetProjectPath( void );
    std::string GetProjectDirectoryPath( void );
    std::string GetOutputDirectoryPath( void );
    std::string GetPathOfOutputDirectory( const std::string& filename );
    std::string GetMakefilePath( void );
    std::string GetTargetFilePath( void );
    std::string GetProjectFilesPathEnumerationString( void );
    using FileSelectFunction = std::function<bool ( Project::File& file )>;
    std::string GetProjectFilesPathEnumerationStringOf( FileSelectFunction function );

    static FileSelectFunction HeaderFileSelect;
    static FileSelectFunction SourceFileSelect;
    static FileSelectFunction HeaderSourceFileSelect;

    void CreateMakefile( SquirrelVM& vm );

    TtProcess::CreateInfo MakeProcessCreateInfo( Settings::ExternalProgram& program, TtForm& parent, const std::string& open_file );

    // -- ProjectVariablesTranslator -------------------------------------
    class ProjectVariablesTranslator {
    public:
      explicit ProjectVariablesTranslator( Developer& developer );

      using UserInputHandler = std::function<std::string ( void )>;
      void SetUserInputHandler( UserInputHandler handler );
      void SetUserInputHandlerAndParentOfDialog( const std::string& title, TtForm& form );

      void SetFileVariable( bool use, const std::string& file );

      std::string Translate( const std::string& source );

    private:
      std::unordered_map<std::string, std::string> table_;
      Developer&       developer_;
      UserInputHandler user_input_handler_;
      bool             use_file_variable_;
      std::string      file_variable_;
    };

  private:
    Settings            settings_;
    Project             project_;
    Project::Structure* current_structure_;
  };
}
