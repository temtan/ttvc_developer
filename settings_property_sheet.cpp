// settings_property_sheet.cpp

#include "ttl_define.h"
#include "tt_file_dialog.h"

#include "exception.h"
#include "utility_dialogs.h"
#include "common.h"

#include "settings_property_sheet.h"

using namespace TTVCDeveloper;


// -- CompilerPage -------------------------------------------------------
SettingsPropertySheet::CompilerPage::CompilerPage( const std::string& title, Settings::Compiler& compiler, Settings& settings ) :
Page( title ),
compiler_( compiler ),
settings_( settings )
{
  this->PresetSizeAtPageCreate( 268, 220 );
}

bool
SettingsPropertySheet::CompilerPage::Created( void )
{
  make_label_.Create( {this} );
  make_edit_.Create( {this} );
  make_button_.Create( {this, CommandID::MakeButton} );
  compiler_label_.Create( {this} );
  compiler_edit_.Create( {this} );
  compiler_button_.Create( {this, CommandID::CompilerButton} );
  resource_label_.Create( {this} );
  resource_edit_.Create( {this} );
  resource_button_.Create( {this, CommandID::ResourceButton} );
  env_include_label_.Create( {this} );
  env_include_edit_.Create( {this} );
  env_lib_label_.Create( {this} );
  env_lib_edit_.Create( {this} );
  env_libpath_label_.Create( {this} );
  env_libpath_edit_.Create( {this} );

  make_label_.SetPositionSize(          4,  12, 100, 16 );
  make_edit_.SetPositionSize(           4,  30, 412, 20 );
  make_button_.SetPositionSize(       420,  30,  42, 20 );
  compiler_label_.SetPositionSize(      4,  62, 100, 16 );
  compiler_edit_.SetPositionSize(       4,  80, 412, 20 );
  compiler_button_.SetPositionSize(   420,  80,  42, 20 );
  resource_label_.SetPositionSize(      4, 112, 100, 16 );
  resource_edit_.SetPositionSize(       4, 130, 412, 20 );
  resource_button_.SetPositionSize(   420, 130,  42, 20 );
  env_include_label_.SetPositionSize(   4, 162, 100, 16 );
  env_include_edit_.SetPositionSize(    4, 180, 460, 20 );
  env_lib_label_.SetPositionSize(       4, 212, 100, 16 );
  env_lib_edit_.SetPositionSize(        4, 230, 460, 20 );
  env_libpath_label_.SetPositionSize(   4, 262, 100, 16 );
  env_libpath_edit_.SetPositionSize(    4, 280, 460, 20 );

  make_label_.SetText( "Make�R�}���h" );
  make_button_.SetText( "�Q��..." );
  compiler_label_.SetText( "�R���p�C��" );
  compiler_button_.SetText( "�Q��..." );
  resource_label_.SetText( "���\�[�X�R���p�C��" );
  resource_button_.SetText( "�Q��..." );
  env_include_label_.SetText( "���ϐ� INCLUDE" );
  env_lib_label_.SetText( "���ϐ� LIB" );
  env_libpath_label_.SetText( "���ϐ� LIBPATH" );

  this->AddCommandHandler( CommandID::MakeButton, [this] ( int, HWND ) -> WMResult {
    TtOpenFileDialog dialog;
    dialog.GetFilters().push_back( {"Make�R�}���h(NMAKE.EXE)", "nmake.exe"} );
    dialog.SetFileName( make_edit_.GetText() );
    if ( dialog.ShowDialog( *this ) ) {
      make_edit_.SetText( dialog.GetFileName() );
    }
    return {WMResult::Done};
  } );
  this->AddCommandHandler( CommandID::CompilerButton, [this] ( int, HWND ) -> WMResult {
    TtOpenFileDialog dialog;
    dialog.GetFilters().push_back( {"�R���p�C��(CL.EXE)", "cl.exe"} );
    dialog.SetFileName( compiler_edit_.GetText() );
    if ( dialog.ShowDialog( *this ) ) {
      compiler_edit_.SetText( dialog.GetFileName() );
    }
    return {WMResult::Done};
  } );
  this->AddCommandHandler( CommandID::ResourceButton, [this] ( int, HWND ) -> WMResult {
    TtOpenFileDialog dialog;
    dialog.GetFilters().push_back( {"���\�[�X�R���p�C��(RC.EXE)", "rc.exe"} );
    dialog.SetFileName( resource_edit_.GetText() );
    if ( dialog.ShowDialog( *this ) ) {
      resource_edit_.SetText( dialog.GetFileName() );
    }
    return {WMResult::Done};
  } );

  make_edit_.SetText(        compiler_.make_command_path_            );
  compiler_edit_.SetText(    compiler_.compiler_path_                );
  resource_edit_.SetText(    compiler_.resource_compiler_path_       );
  env_include_edit_.SetText( compiler_.environment_variable_include_ );
  env_lib_edit_.SetText(     compiler_.environment_variable_lib_     );
  env_libpath_edit_.SetText( compiler_.environment_variable_libpath_ );

  this->GetHandlers().at_apply = [this] ( void ) -> bool {
    compiler_.make_command_path_            = make_edit_.GetText();
    compiler_.compiler_path_                = compiler_edit_.GetText();
    compiler_.resource_compiler_path_       = resource_edit_.GetText();
    compiler_.environment_variable_include_ = env_include_edit_.GetText();
    compiler_.environment_variable_lib_     = env_lib_edit_.GetText();
    compiler_.environment_variable_libpath_ = env_libpath_edit_.GetText();
    settings_.WriteToFile();
    return true;
  };

  make_label_.Show();
  make_edit_.Show();
  make_button_.Show();
  compiler_label_.Show();
  compiler_edit_.Show();
  compiler_button_.Show();
  resource_label_.Show();
  resource_edit_.Show();
  resource_button_.Show();
  env_include_label_.Show();
  env_include_edit_.Show();
  env_lib_label_.Show();
  env_lib_edit_.Show();
  env_libpath_label_.Show();
  env_libpath_edit_.Show();

  return true;
}


// -- CombinationPage ----------------------------------------------------
SettingsPropertySheet::CombinationPage::CombinationPage( Settings& settings ) :
Page( "�A�g" ),
settings_( settings )
{
}

bool
SettingsPropertySheet::CombinationPage::Created( void )
{
  editor_label_.Create( {this} );
  editor_edit_.Create( {this} );
  editor_button_.Create( {this, CommandID::CombinationEditorButton} );
  argument_label_.Create( {this} );
  argument_edit_.Create( {this} );
  argument_help_.Create( {this} );

  editor_label_.SetPositionSize(    4,  12, 100, 16 );
  editor_edit_.SetPositionSize(     4,  30, 412, 20 );
  editor_button_.SetPositionSize( 420,  30,  42, 20 );
  argument_label_.SetPositionSize(  4,  62, 400, 16 );
  argument_edit_.SetPositionSize(   4,  80, 462, 20 );
  argument_help_.SetPositionSize(   4, 104, 400, 20 );

  editor_label_.SetText( "�G�f�B�^�̃p�X" );
  editor_button_.SetText( "�Q��..." );
  argument_label_.SetText( "�G�f�B�^���J���ۂ̈���" );
  argument_help_.SetText( "( @@file@@�F�t�@�C����    @@line@@�F�s�ԍ� )" );

  this->AddCommandHandler( CommandID::CombinationEditorButton, [this] ( int, HWND ) -> WMResult {
    TtOpenFileDialog dialog;
    dialog.GetFilters().push_back( {"�G�f�B�^(*.EXE)", "*.exe"} );
    dialog.SetFileName( editor_edit_.GetText() );
    if ( dialog.ShowDialog( *this ) ) {
      editor_edit_.SetText( dialog.GetFileName() );
    }
    return {WMResult::Done};
  } );

  editor_edit_.SetText( settings_.editor_path_ );
  argument_edit_.SetText( settings_.editor_argument_ );

  this->GetHandlers().at_apply = [this] ( void ) -> bool {
    settings_.editor_path_ = editor_edit_.GetText();
    settings_.editor_argument_ = argument_edit_.GetText();
    settings_.WriteToFile();
    return true;
  };

  editor_label_.Show();
  editor_edit_.Show();
  editor_button_.Show();
  argument_label_.Show();
  argument_edit_.Show();
  argument_help_.Show();

  return true;
}


// -- ViewPage -----------------------------------------------------------
SettingsPropertySheet::ViewPage::ViewPage( Settings& settings ) :
Page( "�\��" ),
settings_( settings )
{
}

bool
SettingsPropertySheet::ViewPage::Created( void )
{
  tree_use_original_icon_check_.Create( {this} );

  tree_use_original_icon_check_.SetPositionSize( 4, 12, 400, 20 );

  tree_use_original_icon_check_.SetText( "�c���[�̃t�@�C���̃A�C�R���� TTVC Developer �̃A�C�R�����g�p����B" );

  tree_use_original_icon_check_.SetCheck( settings_.tree_use_original_icon_ );

  this->GetHandlers().at_apply = [this] ( void ) -> bool {
    settings_.tree_use_original_icon_ = tree_use_original_icon_check_.GetCheck();
    settings_.WriteToFile();
    return true;
  };

  tree_use_original_icon_check_.Show();

  return true;
}


// -- ShortcutPage -------------------------------------------------------
SettingsPropertySheet::ShortcutPage::ShortcutPage( Settings& settings ) :
Page( "�V���[�g�J�b�g" ),
settings_( settings ),
compile_key_( settings.compile_key_ ),
build_key_( settings.build_key_ ),
rebuild_key_( settings.rebuild_key_ ),
execute_key_( settings.execute_key_ )
{
}

bool
SettingsPropertySheet::ShortcutPage::Created( void )
{
  compile_label_.Create( {this} );
  compile_edit_.Create( {this} );
  compile_button_.Create( {this, CommandID::ShortcutCompileButton} );
  build_label_.Create( {this} );
  build_edit_.Create( {this} );
  build_button_.Create( {this, CommandID::ShortcutBuildButton} );
  rebuild_label_.Create( {this} );
  rebuild_edit_.Create( {this} );
  rebuild_button_.Create( {this, CommandID::ShortcutRebuildButton} );
  execute_label_.Create( {this} );
  execute_edit_.Create( {this} );
  execute_button_.Create( {this, CommandID::ShortcutExecuteButton} );

  compile_label_.SetPositionSize(     4,   8, 128, 16 );
  compile_edit_.SetPositionSize(     80,   4, 150, 20 );
  compile_button_.SetPositionSize(  240,   4,  40, 20 );
  build_label_.SetPositionSize(       4,  36, 128, 16 );
  build_edit_.SetPositionSize(       80,  32, 150, 20 );
  build_button_.SetPositionSize(    240,  32,  40, 20 );
  rebuild_label_.SetPositionSize(     4,  64, 128, 16 );
  rebuild_edit_.SetPositionSize(     80,  60, 150, 20 );
  rebuild_button_.SetPositionSize(  240,  60,  40, 20 );
  execute_label_.SetPositionSize(     4,  92, 128, 16 );
  execute_edit_.SetPositionSize(     80,  88, 150, 20 );
  execute_button_.SetPositionSize(  240,  88,  40, 20 );

  compile_label_.SetText( "�R���p�C��" );
  compile_button_.SetText( "�ݒ�..." );
  build_label_.SetText( "�r���h" );
  build_button_.SetText( "�ݒ�..." );
  rebuild_label_.SetText( "�č\�z" );
  rebuild_button_.SetText( "�ݒ�..." );
  execute_label_.SetText( "���s" );
  execute_button_.SetText( "�ݒ�..." );

  compile_edit_.SetText( compile_key_.GetText() );
  build_edit_.SetText(   build_key_.GetText() );
  rebuild_edit_.SetText( rebuild_key_.GetText() );
  execute_edit_.SetText( execute_key_.GetText() );

  auto make_handler = [this] ( const std::string& name, ShortcutKey& key, KeyEdit& edit ) -> CommandHandler {
    return [&, name, this] ( int, HWND ) -> WMResult {
      ShortcutKeyDialog dialog( name );
      dialog.SetKey( key );
      if ( dialog.ShowDialog( this->GetParentSheet() ) ) {
        key = dialog.GetKey();
        edit.SetText( key.GetText() );
      }
      return {WMResult::Done};
    };
  };
  this->AddCommandHandler( CommandID::ShortcutCompileButton, make_handler( "�R���p�C��", compile_key_, compile_edit_ ) );
  this->AddCommandHandler( CommandID::ShortcutBuildButton,   make_handler( "�r���h",     build_key_,   build_edit_ ) );
  this->AddCommandHandler( CommandID::ShortcutRebuildButton, make_handler( "�č\�z",     rebuild_key_, rebuild_edit_ ) );
  this->AddCommandHandler( CommandID::ShortcutExecuteButton, make_handler( "���s",       execute_key_, execute_edit_ ) );

  this->GetHandlers().at_apply = [this] ( void ) -> bool {
    settings_.compile_key_ = compile_key_;
    settings_.build_key_   = build_key_;
    settings_.rebuild_key_ = rebuild_key_;
    settings_.execute_key_ = execute_key_;
    settings_.WriteToFile();
    return true;
  };

  compile_label_.Show();
  compile_edit_.Show();
  compile_button_.Show();
  build_label_.Show();
  build_edit_.Show();
  build_button_.Show();
  rebuild_label_.Show();
  rebuild_edit_.Show();
  rebuild_button_.Show();
  execute_label_.Show();
  execute_edit_.Show();
  execute_button_.Show();

  return true;
}


// -- ExternalProgramPage --------------------------------------------
// -- ExternalProgramPage::ProgramDialog -----------------------------
SettingsPropertySheet::ExternalProgramPage::ProgramDialog::ProgramDialog( Settings::ExternalProgram& program ) :
program_( program )
{
}

DWORD
SettingsPropertySheet::ExternalProgramPage::ProgramDialog::GetStyle( void )
{
  return WS_SYSMENU | WS_CAPTION | WS_SIZEBOX;
}

DWORD
SettingsPropertySheet::ExternalProgramPage::ProgramDialog::GetExtendedStyle( void )
{
  return WS_EX_TOOLWINDOW;
}

bool
SettingsPropertySheet::ExternalProgramPage::ProgramDialog::Created( void )
{
  this->SetText( "�O���v���O�����̐ݒ�" );

  struct CommandID {
    enum ID : int {
      PathButton = 10001,
      ShowHelpButton,
      OkButton,
      CancelButton,
    };
  };

  name_label_.Create( {this} );
  name_edit_.Create( {this} );
  type_label_.Create( {this} );
  type_combo_.Create( {this} );
  path_label_.Create( {this} );
  path_edit_.Create( {this} );
  path_button_.Create( {this, CommandID::PathButton} );
  argument_label_.Create( {this} );
  argument_edit_.Create( {this} );
  current_label_.Create( {this} );
  current_edit_.Create( {this} );
  use_output_check_.Create( {this} );
  show_help_button_.Create( {this, CommandID::ShowHelpButton} );
  ok_button_.Create( {this, CommandID::OkButton} );
  cancel_button_.Create( {this, CommandID::CancelButton} );

  this->SetClientSize( 552, 224, false );
  this->SetCenterRelativeToParent();
  this->RegisterWMSize( [this] ( int, int w, int h ) -> WMResult {
    name_label_.SetPositionSize(             8,      8,      84,       20 );
    name_edit_.SetPositionSize(             98,      4, w - 110,       20 );
    type_label_.SetPositionSize(             8,     34,      84,       20 );
    type_combo_.SetPositionSize(            98,     30,     140,       50 );
    path_label_.SetPositionSize(             8,     60,      84,       20 );
    path_edit_.SetPositionSize(             98,     56, w - 158,       20 );
    path_button_.SetPositionSize(       w - 56,     56,      42,       20 );
    argument_label_.SetPositionSize(         8,     86,      84,       20 );
    argument_edit_.SetPositionSize(         98,     82, w - 110,       20 );
    current_label_.SetPositionSize(          8,    112,      84,       20 );
    current_edit_.SetPositionSize(          98,    108, w - 110,       20 );
    use_output_check_.SetPositionSize(       8,    136,     138,       20 );
    show_help_button_.SetPositionSize(       8,    160,     200,       28 );
    ok_button_.SetPositionSize(        w - 194, h - 26,      88,       21 );
    cancel_button_.SetPositionSize(    w -  94, h - 26,      88,       21 );
    return {WMResult::Done};
  } );
  this->RegisterWMSizing( [this] ( int flag, RECT& rectangle ) ->WMResult {
    switch ( flag ) {
    case WMSZ_TOP:
    case WMSZ_TOPLEFT:
    case WMSZ_TOPRIGHT:
      rectangle.top = rectangle.bottom - 248;
      break;

    case WMSZ_BOTTOM:
    case WMSZ_BOTTOMLEFT:
    case WMSZ_BOTTOMRIGHT:
      rectangle.bottom = rectangle.top + 248;
      break;
    }
    if ( rectangle.right - rectangle.left < 218 ) {
      switch ( flag ) {
      case WMSZ_LEFT:
      case WMSZ_TOPLEFT:
      case WMSZ_BOTTOMLEFT:
        rectangle.left = rectangle.right - 218;
        break;

      case WMSZ_RIGHT:
      case WMSZ_TOPRIGHT:
      case WMSZ_BOTTOMRIGHT:
        rectangle.right = rectangle.left + 218;
        break;
      }
    }
    return {WMResult::Incomplete};
  } );

  name_label_.SetText( "���O�F" );
  type_label_.SetText( "�^�C�v�F" );
  type_combo_.Push( "�ʏ�" );
  type_combo_.Push( "�ʃt�@�C���I�[�v��" );
  path_label_.SetText( "�t�@�C���p�X�F" );
  path_button_.SetText( "�Q��..." );
  argument_label_.SetText( "�����F" );
  current_label_.SetText( "��ƃt�H���_�F" );
  use_output_check_.SetText( "���ʂ��^�u�ɏo�͂���B" );
  show_help_button_.SetText( "�g�p�\�ϐ��ꗗ�_�C�A���O�̕\��" );
  ok_button_.SetText( "OK" );
  cancel_button_.SetText( "�L�����Z��" );

  name_edit_.SetText( program_.name_ );
  type_combo_.SetSelect( program_.for_open_file_ ? 1 : 0 );
  path_edit_.SetText( program_.path_ );
  argument_edit_.SetText( program_.argument_ );
  current_edit_.SetText( program_.current_directory_ );
  use_output_check_.SetCheck( program_.use_output_edit_ );

  this->AddCommandHandler( CommandID::PathButton, [this] ( int, HWND ) -> WMResult {
    TtOpenFileDialog dialog;
    dialog.GetFilters().push_back( {"�O���v���O����(*.EXE)", "*.exe"} );
    dialog.SetFileName( path_edit_.GetText() );
    if ( dialog.ShowDialog( *this ) ) {
      path_edit_.SetText( dialog.GetFileName() );
    }
    return {WMResult::Done};
  } );
  this->AddCommandHandler( CommandID::ShowHelpButton, [this] ( int, HWND ) -> WMResult {
    project_variables_dialog_.Show();
    return {WMResult::Done};
  } );
  this->AddCommandHandler( CommandID::OkButton, [this] ( int, HWND ) -> WMResult {
    program_.name_ = name_edit_.GetText();
    program_.for_open_file_ = type_combo_.GetSelectedIndex() != 0;
    program_.path_ = path_edit_.GetText();
    program_.argument_ = argument_edit_.GetText();
    program_.current_directory_ = current_edit_.GetText();
    program_.use_output_edit_ = use_output_check_.GetCheck();
    this->EndDialog( 1 );
    return {WMResult::Done};
  } );
  this->AddCommandHandler( CommandID::CancelButton, [this] ( int, HWND ) -> WMResult {
    this->EndDialog( 0 );
    return {WMResult::Done};
  } );

  name_label_.Show();
  name_edit_.Show();
  type_label_.Show();
  type_combo_.Show();
  path_label_.Show();
  path_edit_.Show();
  path_button_.Show();
  argument_label_.Show();
  argument_edit_.Show();
  current_label_.Show();
  current_edit_.Show();
  use_output_check_.Show();
  show_help_button_.Show();
  ok_button_.Show();
  cancel_button_.Show();

  // Show ���Ȃ��ƕ\������Ȃ��̂Œ���
  project_variables_dialog_.SetShowFileVariable( true );
  project_variables_dialog_.ShowDialog( *this );
  return true;
}


// -- ExternalProgramPage --------------------------------------------
SettingsPropertySheet::ExternalProgramPage::ExternalProgramPage( Settings& settings ) :
Page( "�O���v���O����" ),
settings_( settings ),
temporary_( settings.external_programs_ )
{
}

bool
SettingsPropertySheet::ExternalProgramPage::Created( void )
{
  list_.Create( {this, CommandID::ExternalProgramList} );
  up_button_.Create( {this, CommandID::ExternalProgramUpButton} );
  down_button_.Create( {this, CommandID::ExternalProgramDownButton} );
  add_button_.Create( {this, CommandID::ExternalProgramAddButton} );
  edit_button_.Create( {this, CommandID::ExternalProgramEditButton} );
  delete_button_.Create( {this, CommandID::ExternalProgramDeleteButton} );

  list_.SetPositionSize(            4,   4, 414, 288 );
  up_button_.SetPositionSize(     424,   4,  42,  20 );
  down_button_.SetPositionSize(   424,  28,  42,  20 );
  add_button_.SetPositionSize(    424,  64,  42,  20 );
  edit_button_.SetPositionSize(   424,  88,  42,  20 );
  delete_button_.SetPositionSize( 424, 112,  42,  20 );

  up_button_.SetText( "���" );
  down_button_.SetText( "����" );
  add_button_.SetText( "�ǉ�..." );
  edit_button_.SetText( "�ҏW..." );
  delete_button_.SetText( "�폜" );

  this->AddNotifyHandler( CommandID::ExternalProgramList, [this] ( NMHDR* nmhdr ) -> WMResult {
    switch ( nmhdr->code ) {
    case LVN_ITEMACTIVATE: { // �A�C�e���_�u���N���b�N��
      unsigned int index = reinterpret_cast<NMITEMACTIVATE*>( nmhdr )->iItem;
      if ( index >= temporary_.size() ) {
        throw TTVC_DEVELOPER_INTERNAL_EXCEPTION;
      }
      ProgramDialog dialog( temporary_[index] );
      if ( dialog.ShowDialog( this->GetParentSheet() ) ) {
        this->RepaintList( index );
      }
      break;
    }

    case LVN_ITEMCHANGED: // �A�C�e���I����
      this->EnableButtons();
      break;
    }
    return {WMResult::Done};
  } );
  this->AddCommandHandler( CommandID::ExternalProgramUpButton, [this] ( int, HWND ) -> WMResult {
    auto selected = list_.GetSelectedIndices();
    if ( selected.size() > 0 && selected[0] > 0 ) {
      int index = selected[0];
      auto tmp = temporary_[index];
      temporary_.erase( temporary_.begin() + index );
      temporary_.insert( temporary_.begin() + index - 1, tmp );
      this->RepaintList( index - 1 );
    }
    return {WMResult::Done};
  } );
  this->AddCommandHandler( CommandID::ExternalProgramDownButton, [this] ( int, HWND ) -> WMResult {
    auto selected = list_.GetSelectedIndices();
    if ( selected.size() > 0 && selected[0] < (list_.GetItemCount() - 1) ) {
      int index = selected[0];
      auto tmp = temporary_[index];
      temporary_.erase( temporary_.begin() + index );
      temporary_.insert( temporary_.begin() + index + 1, tmp );
      this->RepaintList( index + 1 );
    }
    return {WMResult::Done};
  } );
  this->AddCommandHandler( CommandID::ExternalProgramAddButton, [this] ( int, HWND ) -> WMResult {
    Settings::ExternalProgram program;
    ProgramDialog dialog( program );
    if ( dialog.ShowDialog( this->GetParentSheet() ) ) {
      temporary_.push_back( program );
      this->RepaintList( list_.GetItemCount() - 1 );
    }
    return {WMResult::Done};
  } );
  this->AddCommandHandler( CommandID::ExternalProgramEditButton, [this] ( int, HWND ) -> WMResult {
    auto selected = list_.GetSelectedIndices();
    if ( selected.size() > 0 ) {
      int index = selected[0];
      ProgramDialog dialog( temporary_[index] );
      if ( dialog.ShowDialog( this->GetParentSheet() ) ) {
        this->RepaintList( index );
      }
    }
    return {WMResult::Done};
  } );
  this->AddCommandHandler( CommandID::ExternalProgramDeleteButton, [this] ( int, HWND ) -> WMResult {
    auto selected = list_.GetSelectedIndices();
    if ( selected.size() > 0 ) {
      int index = selected[0];
      temporary_.erase( temporary_.begin() + index );
      list_.GetItem( index ).Remove();
      list_.SetFocus();
      if ( list_.GetItemCount() > 0 ) {
        list_.GetItem( index == 0 ? 0 : index - 1 ).SetSelected( true );
      }
      this->EnableButtons();
    }
    return {WMResult::Done};
  } );

  {
    list_.SetFullRowSelect( true );
    TtListViewColumn column_name = list_.MakeNewColumn();
    column_name.SetText( "���O" );
    column_name.SetWidth( 76 );
    TtListViewColumn column_type = list_.MakeNewColumn();
    column_type.SetText( "�^�C�v" );
    column_type.SetWidth( 48 );
    TtListViewColumn column_path = list_.MakeNewColumn();
    column_path.SetText( "�t�@�C���p�X" );
    column_path.SetWidth( 200 );
    TtListViewColumn column_argument = list_.MakeNewColumn();
    column_argument.SetText( "����" );
    column_argument.SetWidth( 150 );
    TtListViewColumn column_current = list_.MakeNewColumn();
    column_current.SetText( "��ƃt�H���_" );
    column_current.SetWidth( 150 );
    this->RepaintList( -1 );
  }

  this->GetHandlers().at_apply = [this] ( void ) -> bool {
    settings_.external_programs_ = temporary_;
    settings_.WriteToFile();
    return true;
  };

  list_.Show();
  up_button_.Show();
  down_button_.Show();
  add_button_.Show();
  edit_button_.Show();
  delete_button_.Show();

  return true;
}

void
SettingsPropertySheet::ExternalProgramPage::EnableButtons( void )
{
  auto selected = list_.GetSelectedIndices();
  up_button_.SetEnabled( NOT( selected.size() == 0 || selected.front() == 0 ) );
  down_button_.SetEnabled( NOT( selected.size() == 0 || selected.front() == (list_.GetItemCount() - 1) ) );
  add_button_.SetEnabled( true );
  edit_button_.SetEnabled( NOT( selected.size() == 0 ) );
  delete_button_.SetEnabled( NOT( selected.size() == 0 ) );
}

void
SettingsPropertySheet::ExternalProgramPage::RepaintList( int select_index )
{
  list_.LockDraw();
  list_.ClearItems();
  for ( auto& tmp : temporary_ ) {
    TtListViewItem item = list_.MakeNewItem();
    item.SetSubItemText( 0, tmp.name_ );
    item.SetSubItemText( 1, tmp.for_open_file_ ? "��" : "�ʏ�" );
    item.SetSubItemText( 2, tmp.path_ );
    item.SetSubItemText( 3, tmp.argument_ );
    item.SetSubItemText( 4, tmp.current_directory_ );
  }
  list_.UnlockDraw();
  list_.SetFocus();
  if ( select_index >= 0 ) {
    list_.GetItem( select_index ).SetSelected( true );
  }
  this->EnableButtons();
}


// -- OtherPage ----------------------------------------------------------
SettingsPropertySheet::OtherPage::OtherPage( Settings& settings ) :
Page( "���̑�" ),
settings_( settings )
{
}

bool
SettingsPropertySheet::OtherPage::Created( void )
{
  extension_script_label_.Create( {this} );
  extension_script_edit_.Create( {this} );
  extension_script_button_.Create( {this, CommandID::ExtensionScriptButton} );

  extension_script_label_.SetPositionSize(    4,  12, 160,  16 );
  extension_script_edit_.SetPositionSize(     4,  30, 412,  20 );
  extension_script_button_.SetPositionSize( 420,  30,  42,  20 );

  extension_script_label_.SetText( "�g���X�N���v�g�̃p�X" );
  extension_script_button_.SetText( "�Q��..." );

  this->AddCommandHandler( CommandID::ExtensionScriptButton, [this] ( int, HWND ) -> WMResult {
    TtOpenFileDialog dialog;
    dialog.GetFilters().push_back( {"�X�N���v�g�t�@�C��(*.*)", "*.*"} );
    dialog.SetFileName( extension_script_edit_.GetText() );
    if ( dialog.ShowDialog( *this ) ) {
      extension_script_edit_.SetText( dialog.GetFileName() );
    }
    return {WMResult::Done};
  } );

  extension_script_edit_.SetText( settings_.extension_script_path_ );

  this->GetHandlers().at_apply = [this] ( void ) -> bool {
    settings_.extension_script_path_ = extension_script_edit_.GetText();
    settings_.WriteToFile();
    return true;
  };

  extension_script_label_.Show();
  extension_script_edit_.Show();
  extension_script_button_.Show();
  return true;
}


// -- SettingsPropertySheet ----------------------------------------------
SettingsPropertySheet::SettingsPropertySheet( Settings& settings ) :
TtPropertySheet( false ),
compiler_x86_page_( "�R���p�C��x86", settings.compiler_x86_, settings ),
compiler_x64_page_( "�R���p�C��x64", settings.compiler_x64_, settings ),
combination_page_( settings ),
view_page_( settings ),
shortcut_page_( settings ),
external_program_page_( settings ),
other_page_( settings )
{
  this->AddPage( compiler_x86_page_ );
  this->AddPage( compiler_x64_page_ );
  this->AddPage( combination_page_ );
  this->AddPage( view_page_ );
  this->AddPage( shortcut_page_ );
  this->AddPage( external_program_page_ );
  this->AddPage( other_page_ );
}

bool
SettingsPropertySheet::Created( void )
{
  this->SetText( "���ݒ�" );
  this->SetIconAsLarge( Image::ICONS[Image::Index::Main] );

  return true;
}
