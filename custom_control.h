// custom_control.h

#pragma once

#include <memory>

#include "tt_window_controls.h"
#include "tt_combo_box.h"
#include "tt_menu.h"
#include "tt_tab.h"
#include "tt_tool_bar.h"
#include "tt_image_list.h"
#include "tt_thread.h"

#include "main_tree.h"
#include "settings.h"
#include "process_manager.h"
#include "utility.h"


namespace TTVCDeveloper {
  using ExternalProgramMenuItem = TtMenuItemWith<Settings::ExternalProgram*>;

  // -- MainMenu ---------------------------------------------------------
  class MainMenu : public TtMenuBar {
  public:
    explicit MainMenu( void );

    void SetBeginningMode( void );
    void SetNormalMode( void );
    void SetMainMode( bool flag );

    void SetBuildingMode( void );
    void UnsetBuildingMode( void );
    void SetBuildingModeFlag( bool flag );

    void TreeViewItemSelected( MainTree::Item& item );

    TtMenuItem GetStopExternalProgramMenuItem( void );

    void SetExternalProgramMenu( TtSubMenuCommand& menu );

  public:
    TtSubMenu file_menu_;
    TtSubMenu project_menu_;
    TtSubMenu build_menu_;
    TtSubMenu execute_menu_;
    TtSubMenu tool_menu_;
    TtSubMenu help_menu_;
    TtSubMenu prototype_menu_;

    TtSubMenu tool_project_folder_menu_;

    TtSubMenuCommand project_history_menu_;
    TtSubMenuCommand current_structure_menu_;
    TtSubMenuCommand external_program_menu_;
  };

  // -- OutputEdit -------------------------------------------------------
  class OutputEdit : public TtEditWithStyle<TtEdit::Style::READONLY | TtEdit::Style::MULTILINE | WS_VSCROLL | WS_HSCROLL> {
  public:
    explicit OutputEdit( void );

    virtual bool CreatedInternal( void ) override;

    using FilePathAndLineHandler = std::function<void ( const std::string& path, int line )>;
    void SetFilePathAndLineHandler( FilePathAndLineHandler handler );

    void Clear( void );
    void AddText( const std::string& data );

    ProcessManager& GetProcessManager( void );

  private:
    FilePathAndLineHandler    file_path_and_line_handler_;
    ProcessManager            process_manager_;
  };

  // -- OutputEditPanel --------------------------------------------------
  class OutputEditPanel : public TtPanel {
  public:
    explicit OutputEditPanel( void );

    virtual bool CreatedInternal( void ) override;

    OutputEdit& GetOutputEdit( void );

  private:
    OutputEdit output_edit_;
    HBRUSH     background_brush_;
  };

  // -- MainTab ----------------------------------------------------------
  class MainTab : public TtTab {
  public:
    enum : int {
      Compiler,
      Execute,
      ExternalProgram,
    };

    explicit MainTab( void );

    virtual bool CreatedInternal( void ) override;

    OutputEdit& GetCompilerEdit( void );
    OutputEdit& GetExecuteEdit( void );
    OutputEdit& GetExternalProgramEdit( void );
  };

  // -- MainToolBar ------------------------------------------------------
  class MainToolBar : public TtWindowWithStyle<TtToolBar, TtToolBar::Style::Flat | TtToolBar::Style::List | TtToolBar::Style::WithToolTips> {
  public:
    explicit MainToolBar( void );

    virtual bool CreatedInternal( void ) override;

    void SetBeginningMode( void );
    void SetNormalMode( void );

    void SetBuildingMode( void );
    void UnsetBuildingMode( void );

  public:
    TtWindowWithStyle<TtComboBox, CBS_DROPDOWN | CBS_DISABLENOSCROLL | WS_VSCROLL> structure_combo_box_;
  };

  // -- TreeToolBar ------------------------------------------------------
  class TreeToolBar : public TtWindowWithStyle<TtToolBar, TtToolBar::Style::Flat | TtToolBar::Style::List | TtToolBar::Style::WithToolTips> {
  public:
    explicit TreeToolBar( void );

    virtual bool CreatedInternal( void ) override;

    void SetBeginningMode( void );
    void SetNormalMode( void );

    void TreeViewItemSelected( MainTree::Item& item );
  };

  // -- TreePopupMenu ------------------------------------------------
  class TreePopupMenu : public TtSubMenuCommand {
  public:
    explicit TreePopupMenu( void );

    void SetBeginningMode( void );
    void SetNormalMode( void );

    void TreeViewItemSelected( MainTree::Item& item );

    void SetExternalProgramMenu( TtSubMenuCommand& menu );

    TtSubMenuCommand external_program_menu_;
  };
}
