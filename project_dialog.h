// project_dialog.h

#pragma once

#include <optional>

#include "tt_dialog.h"
#include "tt_window_controls.h"
#include "tt_combo_box.h"
#include "tt_list_box.h"
#include "tt_tab.h"
#include "tt_icon.h"

#include "project.h"
#include "utility_dialogs.h"


namespace TTVCDeveloper {
  // -- ProjectDialog ----------------------------------------------------
  class ProjectDialog : public TtDialog {
  public:
    struct CommandID {
      enum ID : int {
        StructureComboBox = 10001,
        StructureEditButton,
        Ok,
        Cancel,

        TargetUseOutputCheck,
        ShowProjectVariablesButton,

        IncludeAddButton,
        IncludeEditButton,
        IncludeDeleteButton,
        IncludeConvertButton,

        DefinesList,
        DefinesAddButton,
        DefinesEditButton,
        DefinesDeleteButton,

        PrecompiledHeaderUseCheck,

        WarningsAddButton,
        WarningsDeleteButton,

        LinkAddButton,
        LinkEditButton,
        LinkDeleteButton,
        LinkConvertButton,

        ExtensionScriptButton,

        UseIconCheck,
        IconChangeButton,
      };
    };

    explicit ProjectDialog( Project& source, const Project::Structure* current_structure, bool is_default );

    Project& GetProject( void );
    Project::Structure* GetCurrentStructure( void );

  private:
    virtual bool Created( void ) override;

    void SetStructuresComboBox( void );
    void SetFromStructure( const Project::Structure& structure );
    void SetToStructure( Project::Structure& structure );

    // -- for Panels -----
    using ListBoxPath = TtListBoxWithStyle<WS_VSCROLL | WS_HSCROLL>;
    using ListBoxNumber = TtListBoxWithStyle<WS_VSCROLL |  LBS_SORT>;
    using ButtonMultiline = TtWindowWithStyle<TtButton, BS_MULTILINE>;
    template <class TYPE>
    using ComboBoxWith = TtComboBoxTable<TtWindowWithStyle<TtComboBox, CBS_DROPDOWNLIST | WS_VSCROLL>, TYPE>;

    class PanelBase : public TtPanel {
    public:
      explicit PanelBase( ProjectDialog& parent );

    protected:
      ProjectDialog& parent_;
    };

    // -- ApplicationPanel -----
    class ApplicationPanel : public PanelBase {
    public:
      explicit ApplicationPanel( ProjectDialog& parent );

      virtual bool CreatedInternal( void ) override;
      void SetFromStructure( const Project::Structure& structure );
      void SetToStructure( Project::Structure& structure );

    private:
      using PlatformCombo = ComboBoxWith<Platform>;
      using TargetKindCombo = ComboBoxWith<Project::TargetKind>;

      TtStatic        platform_label_;
      PlatformCombo   platform_combo_;
      TtStatic        target_kind_label_;
      TargetKindCombo target_kind_combo_;
      TtStatic        output_directory_label_;
      TtEdit          output_directory_edit_;
      TtStatic        target_label_;
      TtEdit          target_edit_;
      TtStatic        target_argument_label_;
      TtEdit          target_argument_edit_;
      TtStatic        target_current_label_;
      TtEdit          target_current_edit_;
      TtCheckBox      target_use_output_check_;
      TtCheckBox      target_use_input_check_;
      TtButton        show_variables_button_;
      TtStatic        help_label_;
    };

    // -- Compile1Panel -----
    class Compile1Panel : public PanelBase {
    public:
      explicit Compile1Panel( ProjectDialog& parent );

      virtual bool CreatedInternal( void ) override;
      void SetFromStructure( const Project::Structure& structure );
      void SetToStructure( Project::Structure& structure );

      void SetControlEnabled( void );

    private:
      using OptimizeCombo = ComboBoxWith<Project::OptimizeKind>;
      using RuntimeCombo = ComboBoxWith<Project::RuntimeLibrary>;
      using EditCombo = TtWindowWithStyle<TtComboBox, CBS_DROPDOWN | CBS_DISABLENOSCROLL | WS_VSCROLL>;

      TtStatic      optimize_label_;
      OptimizeCombo optimize_combo_;
      TtStatic      runtime_label_;
      RuntimeCombo  runtime_combo_;
      TtCheckBox    use_exception_check_;
      TtCheckBox    pch_use_check_;
      TtStatic      pch_source_label_;
      EditCombo     pch_source_combo_;
      TtStatic      pch_header_label_;
      EditCombo     pch_header_combo_;
      TtCheckBox    pch_force_check_;
    };

    // -- Compile2Panel -----
    class Compile2Panel : public PanelBase {
    public:
      explicit Compile2Panel( ProjectDialog& parent );

      virtual bool CreatedInternal( void ) override;
      void SetFromStructure( const Project::Structure& structure );
      void SetToStructure( Project::Structure& structure );

    private:
      TtStatic        include_label_;
      ListBoxPath     include_list_;
      TtButton        include_add_button_;
      TtButton        include_edit_button_;
      TtButton        include_delete_button_;
      ButtonMultiline include_convert_button_;
      TtStatic        defines_label_;
      ListBoxPath     defines_list_;
      TtEdit          defines_edit_;
      TtButton        defines_add_button_;
      TtButton        defines_edit_button_;
      TtButton        defines_delete_button_;
    };

    // -- WarningsPanel -----
    class WarningsPanel : public PanelBase {
    public:
      explicit WarningsPanel( ProjectDialog& parent );

      virtual bool CreatedInternal( void ) override;
      void SetFromStructure( const Project::Structure& structure );
      void SetToStructure( Project::Structure& structure );

    private:
      using LevelCombo = TtWindowWithStyle<TtComboBox, CBS_DROPDOWNLIST | WS_VSCROLL>;

      TtStatic       level_label_;
      LevelCombo     level_combo_;
      TtStatic       number_label_;
      ListBoxNumber  number_list_;
      TtEditNumber   number_edit_;
      TtButton       add_button_;
      TtButton       delete_button_;
      TtCheckBox     error_if_warning_check_;
    };

    // -- LinkPanel -----
    class LinkPanel : public PanelBase {
    public:
      explicit LinkPanel( ProjectDialog& parent );

      virtual bool CreatedInternal( void ) override;
      void SetFromStructure( const Project::Structure& structure );
      void SetToStructure( Project::Structure& structure );

    private:
      TtStatic        link_label_;
      ListBoxPath     link_list_;
      TtButton        add_button_;
      TtButton        edit_button_;
      TtButton        delete_button_;
      ButtonMultiline convert_button_;
    };

    // -- OthersPanel -----
    class OthersPanel : public PanelBase {
    public:
      explicit OthersPanel( ProjectDialog& parent );

      virtual bool CreatedInternal( void ) override;
      void SetFromStructure( const Project::Structure& structure );
      void SetToStructure( Project::Structure& structure );

    private:
      TtCheckBox no_logo_check_;
      TtStatic   compiler_other_label_;
      TtEdit     compiler_other_edit_;
      TtStatic   resource_compiler_label_;
      TtEdit     resource_compiler_edit_;
      TtStatic   extension_script_label_;
      TtEdit     extension_script_edit_;
      TtButton   extension_script_button_;
    };

    // -- CommonPanel -----
    class CommonPanel : public PanelBase {
    public:
      explicit CommonPanel( ProjectDialog& parent );

      virtual bool CreatedInternal( void ) override;

      void SetFromProject( const Project& project );
      void SetToProject( Project& project );

      void SetControlEnabled( void );
      void SetIcon( void );

    private:
      std::string icon_file_path_;
      int         icon_file_index_;

      TtStatic              attention_label_;
      TtCheckBox            use_icon_check_;
      std::optional<TtIcon> icon_small_;
      std::optional<TtIcon> icon_large_;
      TtButton              icon_change_button_;
    };

  private:
    using StructureComboBox = ComboBoxWith<Project::Structure*>;

    Project             project_;
    Project::Structure* current_structure_;
    TtIcon              icon_;
    std::string         title_;

    TtStatic          name_label_;
    TtEdit            name_edit_;
    TtStatic          structure_select_label_;
    StructureComboBox structure_combo_box_;
    TtButton          structure_edit_button_;
    TtTab             tab_;
    TtButton          ok_button_;
    TtButton          cancel_button_;

    ApplicationPanel* application_panel_;
    Compile1Panel*    compile1_panel_;
    Compile2Panel*    compile2_panel_;
    WarningsPanel*    warnings_panel_;
    LinkPanel*        link_panel_;
    OthersPanel*      others_panel_;
    CommonPanel*      common_panel_;

    ProjectVariablesDialog project_variables_dialog_;
  };

  // -- StructuresEditDialog ---------------------------------------------
  class StructuresEditDialog : public TtDialog {
  public:
    explicit StructuresEditDialog( const std::vector<std::shared_ptr<Project::Structure>>& source );

    virtual bool Created( void ) override;

    std::vector<std::shared_ptr<Project::Structure>> GetStructures();

  private:
    std::vector<std::shared_ptr<Project::Structure>> structures_;
    TtIcon icon_;

    TtStatic                       list_label_;
    TtListBoxWithStyle<WS_VSCROLL> list_;
    TtStatic                       edit_label_;
    TtEdit                         edit_;
    TtButton                       copy_button_;
    TtButton                       delete_button_;
    TtButton                       ok_button_;
    TtButton                       cancel_button_;
  };
}
