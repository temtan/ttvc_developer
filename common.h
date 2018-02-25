// common.h

#pragma once

#include <memory>
#include <deque>
#include <optional>

#include "tt_exception.h"
#include "tt_image_list.h"


namespace TTVCDeveloper {
  const char* const VERSION = "1.1.1";
  const char* const APPLICATION_NAME = "TTVC Delevoper";

  // -- CommandID --------------------------------------------------------
  namespace CommandID {
    enum ID : int {
      // ファイル
      Close = 2000,
      NewProject,
      OpenProject,
      SaveProject,
      DeleteProjectHistory,

      // プロジェクト設定
      DefaultProjectSettings,
      ProjectSettings,

      // 編集
      OpenFile,
      AddNewFile,
      AddExistingFile,
      AddDirectory,
      DeleteElement,
      DownElement,
      UpElement,
      RenameElement,

      // ビルド
      SelectStructure,
      Compile,
      Build,
      Rebuild,
      Clean,

      // 実行
      Execute,

      // ツール
      Settings,
      OpenProjectFolderByExplorer,
      OpenProjectFolderByConsole,
      ExternalProgram,
      StopExternalProgram,

      // ヘルプ
      VersionInformation,

      // TestMode
      Test1,
      Test2,
      Test3,
      Test4,
      Test5,
    };

    namespace Control {
      enum ID : int {
        MainTree = 3000,
        MainToolBar,
        MainTab,
      };
    }

    namespace MainToolBar {
      enum ID : int {
        NewProject          = CommandID::NewProject,
        OpenProject         = CommandID::OpenProject,
        SaveProject         = CommandID::SaveProject,
        ProjectSettings     = CommandID::ProjectSettings,
        SelectStructure     = CommandID::SelectStructure,
        Compile             = CommandID::Compile,
        Build               = CommandID::Build,
        Rebuild             = CommandID::Rebuild,
        Clean               = CommandID::Clean,
        Execute             = CommandID::Execute,
        ExternalProgram     = CommandID::ExternalProgram,
        StopExternalProgram = CommandID::StopExternalProgram,
      };
    }

    namespace TreeToolBar {
      enum ID : int {
        AddNewFile      = CommandID::AddNewFile,
        AddExistingFile = CommandID::AddExistingFile,
        AddDirectory    = CommandID::AddDirectory,
        DeleteElement   = CommandID::DeleteElement,
        DownElement     = CommandID::DownElement,
        UpElement       = CommandID::UpElement,
        RenameElement   = CommandID::RenameElement,
      };
    }
  }

  // -- other types ------------------------------------------------------
  enum class Platform {
    X86,
    X64,
  };

  using ProjectHistory = std::deque<std::pair<std::string, std::string>>;

  // -- IniFileOperation -------------------------------------------------
  namespace IniFileOperation {
    void SavePlacement( WINDOWPLACEMENT& placement );
    bool LoadPlacement( WINDOWPLACEMENT& placement );

    void SaveSplitterPosition( unsigned int width );
    unsigned int LoadSplitterPosition( void );

    void SaveLastProject( const std::string& path );
    std::string LoadLastProject( void );

    void SaveProjectHistory( const ProjectHistory& history, unsigned int max );
    void LoadProjectHistory( ProjectHistory& history, unsigned int max );

    void SaveErrorLogDump( TtException& e );

    // Secret
    bool LoadTestMode( void );
  }

  namespace Image {
    // -- Index ------------------------------------------------------------
    struct Index {
      enum Value : int {
        None = -1,
        Main,
        MainSmall,

        // MainMenu & MainToolBar
        NewProject,
        OpenProject,
        SaveProject,
        ProjectHistory,
        ProjectSettings,
        DefaultProjectSettings,
        Compile,
        Build,
        Rebuild,
        Clean,
        Execute,
        ExternalProgram,
        StopExternalProgram,

        // TreeToolBar
        AddNewFile,
        AddExistingFile,
        AddDirectory,
        DeleteElement,
        DownElement,
        UpElement,
        RenameElement,

        // Tree
        ProjectElement,
        Directory,
        CFile,
        CppFile,
        ResourceScriptFile,
        HeaderFile,
        OtherFile,
      };
    };

    // -- Objects
    extern std::vector<TtIcon>        ICONS;
    extern std::vector<TtBmpImage>    BMPS;
    extern std::optional<TtImageList> LIST;

    extern int OffsetOfList;

    void Initialize( void );
  }

  namespace Font {
    extern TtFont OUTPUT_MONOSPACED;
  }
}
