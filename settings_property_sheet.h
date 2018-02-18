// settings_property_sheet.h

#pragma once

#include "tt_property_sheet.h"
#include "tt_window_controls.h"
#include "tt_list_view.h"
#include "tt_combo_box.h"
#include "tt_dialog.h"

#include "settings.h"
#include "utility_dialogs.h"


namespace TTVCDeveloper {
  // -- SettingsPropertySheet --------------------------------------------
  class SettingsPropertySheet : public TtPropertySheet {
  private:
    struct CommandID {
      enum ID : int {
        MakeButton = 10001,
        CompilerButton,
        ResourceButton,

        CombinationEditorButton,

        ShortcutCompileButton,
        ShortcutBuildButton,
        ShortcutRebuildButton,
        ShortcutExecuteButton,

        ExternalProgramList,
        ExternalProgramUpButton,
        ExternalProgramDownButton,
        ExternalProgramAddButton,
        ExternalProgramEditButton,
        ExternalProgramDeleteButton,

        ExtensionScriptButton,
      };
    };

    // -- CompilerPage ---------------------------------------------------
    class CompilerPage : public Page {
    public:
      explicit CompilerPage( const std::string& title, Settings::Compiler& compiler, Settings& settings );

      virtual bool Created( void ) override;

    private:
      Settings::Compiler& compiler_;
      Settings&           settings_;

      TtStatic make_label_;
      TtEdit   make_edit_;
      TtButton make_button_;
      TtStatic compiler_label_;
      TtEdit   compiler_edit_;
      TtButton compiler_button_;
      TtStatic resource_label_;
      TtEdit   resource_edit_;
      TtButton resource_button_;
      TtStatic env_include_label_;
      TtEdit   env_include_edit_;
      TtStatic env_lib_label_;
      TtEdit   env_lib_edit_;
      TtStatic env_libpath_label_;
      TtEdit   env_libpath_edit_;
    };

    // -- CombinationPage ------------------------------------------------
    class CombinationPage : public Page {
    public:
      explicit CombinationPage( Settings& settings );

      virtual bool Created( void ) override;

    private:
      Settings& settings_;

      TtStatic editor_label_;
      TtEdit   editor_edit_;
      TtButton editor_button_;
      TtStatic argument_label_;
      TtEdit   argument_edit_;
      TtStatic argument_help_;
    };

    // -- ViewPage -------------------------------------------------------
    class ViewPage : public Page {
    public:
      explicit ViewPage( Settings& settings );

      virtual bool Created( void ) override;

    private:
      Settings& settings_;

      TtCheckBox tree_use_original_icon_check_;
    };

    // -- ShortcutPage ---------------------------------------------------
    class ShortcutPage : public Page {
    public:
      explicit ShortcutPage( Settings& settings );

      virtual bool Created( void ) override;

    private:
      using KeyEdit = TtEditWithStyle<TtEdit::Style::READONLY>;
      using ShortcutKey = TtForm::AcceleratorMap::ShortcutKey;

      Settings&   settings_;
      ShortcutKey compile_key_;
      ShortcutKey build_key_;
      ShortcutKey rebuild_key_;
      ShortcutKey execute_key_;

      TtStatic compile_label_;
      KeyEdit  compile_edit_;
      TtButton compile_button_;
      TtStatic build_label_;
      KeyEdit  build_edit_;
      TtButton build_button_;
      TtStatic rebuild_label_;
      KeyEdit  rebuild_edit_;
      TtButton rebuild_button_;
      TtStatic execute_label_;
      KeyEdit  execute_edit_;
      TtButton execute_button_;
    };

    // -- ExternalProgramPage --------------------------------------------
    class ExternalProgramPage : public Page {
    public:
      // -- ProgramDialog ------------------------------------------------
      class ProgramDialog : public TtDialog {
      public:
        explicit ProgramDialog( Settings::ExternalProgram& program );

        virtual DWORD  GetStyle( void ) override;
        virtual DWORD  GetExtendedStyle( void ) override;
        virtual bool Created( void ) override;

      private:
        Settings::ExternalProgram& program_;

        TtStatic   name_label_;
        TtEdit     name_edit_;
        TtStatic   type_label_;
        TtComboBoxDropDownNotEdit type_combo_;
        TtStatic   path_label_;
        TtEdit     path_edit_;
        TtButton   path_button_;
        TtStatic   argument_label_;
        TtEdit     argument_edit_;
        TtStatic   current_label_;
        TtEdit     current_edit_;
        TtCheckBox use_output_check_;
        TtButton   show_help_button_;
        TtButton   ok_button_;
        TtButton   cancel_button_;

        ProjectVariablesDialog project_variables_dialog_;
      };

      // -- ExternalProgramPage ------------------------------------------
      explicit ExternalProgramPage( Settings& settings );

      virtual bool Created( void ) override;

      void EnableButtons( void );
      void RepaintList( int select_index );

    private:
      Settings& settings_;
      std::vector<Settings::ExternalProgram> temporary_;

      using ProgramList = TtWindowWithStyle<TtListViewReport, LVS_SINGLESEL | LVS_NOSORTHEADER | LVS_SHOWSELALWAYS>;
      ProgramList list_;

      TtButton up_button_;
      TtButton down_button_;
      TtButton add_button_;
      TtButton edit_button_;
      TtButton delete_button_;
    };

    // -- OtherPage ------------------------------------------------------
    class OtherPage : public Page {
    public:
      explicit OtherPage( Settings& settings );

      virtual bool Created( void ) override;

    private:
      Settings& settings_;

      TtStatic extension_script_label_;
      TtEdit   extension_script_edit_;
      TtButton extension_script_button_;
    };

    // ----- SettingsPropertySheet
  public:
    explicit SettingsPropertySheet( Settings& settings );

    virtual bool Created( void ) override;

  private:
    CompilerPage        compiler_x86_page_;
    CompilerPage        compiler_x64_page_;
    CombinationPage     combination_page_;
    ViewPage            view_page_;
    ShortcutPage        shortcut_page_;
    ExternalProgramPage external_program_page_;
    OtherPage           other_page_;
  };
}
