// main_frame.h

#pragma once

#include <memory>
#include <optional>

#include "tt_form.h"
#include "tt_tool_bar.h"
#include "tt_critical_section.h"

#include "common.h"
#include "exception.h"
#include "developer.h"
#include "main_tree.h"
#include "custom_control.h"
#include "tree_dragdrop.h"
#include "squirrel_vm.h"


namespace TTVCDeveloper {
  class MainFrame : public TtForm {
  public:
    explicit MainFrame( void );
    ~MainFrame();

    virtual DWORD GetStyle( void ) override;
    virtual DWORD GetExtendedStyle( void ) override;
    virtual bool  Created( void ) override;

    void RegisterHandlers( void );

    Developer& GetDeveloper( void );
    MainTab&   GetMainTab( void );

    void SetTitleBar( void );
    void SetBeginningMode( void );
    void SetNormalMode( void );
    void SetControlEnableExternalProgramExecuting( void );
    void SetControlEnableExternalProgramEnded( void );

    void LoadPlacement( void );

    void SetCurrentStructureMenu( void );

    void SetFromSettings( void );
    void InitializeSquirrelVMGeneral( void );
    void SetExternalProgramMenu( void );
    TtSubMenuCommand CreateEmptyExternalProgramMenu( void );
    void ExecuteExternalProgram( Settings::ExternalProgram& external_program, const std::string& open_file );

    void InitializeProjectHistory( void );
    void SetProjectHistory( void );
    void SetAccelerator( void );

    void Build( SquirrelVM& vm );
    void Execute( void );

    void LoadProjectFile( const std::string& path );
    void CreateProcessErrorMessageBox( ProcessCreateException& e );
    void ProjectSaveLoadErrorHandling( std::function<void ( void )> function );
    bool SaveProjectConfirmation( void );

    void InitializeSquirrelVMBuild( void );
    void SquirrelErrorHandling( std::function<void ( void )> function );
    bool SquirrelErrorHandlingReturnErrorNotOccurred( std::function<bool ( void )> function );
    bool IfVMIsUseableSquirrelErrorHandlingReturnErrorNotOccurred( std::function<bool ( void )> function );

  public:
    struct ParseOutputLineResult {
      bool        success;
      std::string path;
      int         line;
    } ParseOutputLine( const std::string& str );

  private:
    Developer developer_;

    bool              project_changed_;
    TtCriticalSection process_execute_lock_;

    std::optional<SquirrelVM>                    vm_general_;
    TtSubMenuCommandMaker                        external_program_menu_normal_maker_;
    TtSubMenuCommandMaker                        external_program_menu_open_file_maker_;
    std::vector<TtUtility::SharedDestructorCall> external_program_menu_holder_;

    std::optional<SquirrelVM> vm_build_;

    TtIcon title_bar_icon_large_;
    TtIcon title_bar_icon_small_;

    ProjectHistory project_history_;

    // -- control member -------------------------------------------------
    // -- 全体 -----
    MainMenu                main_menu_;
    MainToolBar             main_tool_bar_;
    TtVirticalSplitterPanel main_splitter_panel_;

    // -- 左側 -----
    TtPanel     tree_panel_;
    TreeToolBar tree_tool_bar_;
    MainTree    tree_;

    // -- 右側 -----
    MainTab     tab_;

    // -- 他 -----
    TreePopupMenu tree_popup_menu_;

    // -- ツリー操作絡み -----
    MainTree::Item                         editing_item_;
    std::optional<TtExtraordinarilyWindow> label_edit_;

    // -- ツリー D&D 絡み
    MainTree::Item::DragHandler drag_handler_;
    TreeDataObject*             tree_data_object_;
    TreeDropSource*             tree_drop_source_;
    TreeDropTarget*             tree_drop_target_;
  };
}
