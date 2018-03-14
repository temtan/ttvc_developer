// project_dialog.cpp

#include <algorithm>

#include "ttl_define.h"
#include "tt_file_dialog.h"
#include "tt_folder_browser_dialog.h"
#include "tt_message_box.h"
#include "tt_pick_icon_dialog.h"
#include "tt_path.h"
#include "tt_utility.h"

#include "exception.h"
#include "utility.h"

#include "project_dialog.h"

using namespace TTVCDeveloper;


// -- ProjectDialog ----------------------------------------------------
ProjectDialog::ProjectDialog( Project& source, const Project::Structure* current_structure, bool is_default ) :
project_(),
current_structure_( nullptr ),
icon_( Image::ICONS[is_default ? Image::Index::DefaultProjectSettings : Image::Index::ProjectSettings] ),
title_( std::string(is_default ? "デフォルトの" : "" ) + "プロジェクト設定" ),

name_label_(),
name_edit_(),
structure_select_label_(),
structure_combo_box_(),
structure_edit_button_(),
tab_(),
ok_button_(),
cancel_button_(),

application_panel_( nullptr ),
compile1_panel_( nullptr ),
compile2_panel_( nullptr ),
warnings_panel_( nullptr ),
link_panel_( nullptr ),
others_panel_( nullptr ),
common_panel_( nullptr ),

project_variables_dialog_()
{
  project_.path_ = source.path_;
  project_.name_ = source.name_;
  project_.use_icon_ = source.use_icon_;
  project_.icon_file_path_ = source.icon_file_path_;
  project_.icon_file_index_ = source.icon_file_index_;
  project_.GetFiles() = source.GetFiles();
  project_.GetDirectories() = source.GetDirectories();
  for ( std::shared_ptr<Project::Structure>& s : source.GetStructures() ) {
    project_.GetStructures().push_back( std::shared_ptr<Project::Structure>( new Project::Structure( *s ) ) );
    if ( s.get() == current_structure ) {
      current_structure_ = project_.GetStructures().back().get();
    }
  }
  if ( current_structure_ == nullptr ) {
    if ( project_.GetStructures().empty() ) {
      throw TTVC_DEVELOPER_INTERNAL_EXCEPTION;
    }
    current_structure_ = project_.GetStructures().front().get();
  }
}


Project&
ProjectDialog::GetProject( void )
{
  return project_;
}

Project::Structure*
ProjectDialog::GetCurrentStructure( void )
{
  return current_structure_;
}



bool
ProjectDialog::Created( void )
{
  this->SetText( title_ );
  this->SetIconAsSmall( icon_ );

  name_label_.Create( {this} );
  name_edit_.Create( {this} );
  structure_select_label_.Create( {this} );
  structure_combo_box_.Create( {this, CommandID::StructureComboBox} );
  structure_edit_button_.Create( {this, CommandID::StructureEditButton} );
  tab_.Create( {this} );
  tab_.RegisterTabChangeHandlersTo( *this );
  ok_button_.Create( {this, CommandID::Ok} );
  cancel_button_.Create( {this, CommandID::Cancel} );

  this->SetClientSize( 600, 450, false );
  this->SetCenterRelativeToParent();
  {
    const int w  = this->GetClientWidth();
    const int h = this->GetClientHeight();
    name_label_.SetPositionSize(                   8,  14,      32,  16 );
    name_edit_.SetPositionSize(                   40,  10, w - 430,  20 );
    structure_select_label_.SetPositionSize( w - 372,  14,     132,  16 );
    structure_combo_box_.SetPositionSize(    w - 248,  10,     148, 100 );
    structure_edit_button_.SetPositionSize(  w -  96,   8,      88,  24 );

    tab_.SetPositionSize( 8, 36, w - 16, h - 68 );

    ok_button_.SetPositionSize(      w - 196,  h - 26, 88, 22 );
    cancel_button_.SetPositionSize(  w -  96,  h - 26, 88, 22 );
  }

  name_label_.SetText( "名前" );
  structure_select_label_.SetText( "現在のプロジェクト構成" );
  structure_edit_button_.SetText( "構成の編集" );
  ok_button_.SetText( "OK" );
  cancel_button_.SetText( "キャンセル" );

  application_panel_ = &tab_.Append<ApplicationPanel>( "アプリケーション", std::ref( *this ) );
  compile1_panel_    = &tab_.Append<Compile1Panel>(    "コンパイル1",      std::ref( *this ) );
  compile2_panel_    = &tab_.Append<Compile2Panel>(    "コンパイル2",      std::ref( *this ) );
  warnings_panel_    = &tab_.Append<WarningsPanel>(    "警告",             std::ref( *this ) );
  link_panel_        = &tab_.Append<LinkPanel>(        "リンク",           std::ref( *this ) );
  others_panel_      = &tab_.Append<OthersPanel>(      "その他",           std::ref( *this ) );
  common_panel_      = &tab_.Append<CommonPanel>(      "共通",             std::ref( *this ) );

  this->AddCommandHandler( CommandID::StructureComboBox, [this] ( int code, HWND ) -> WMResult {
    if ( code == CBN_SELCHANGE ) {
      this->SetToStructure( *current_structure_ );
      current_structure_ = structure_combo_box_.GetSelectedItemData();
      this->SetFromStructure( *current_structure_ );
    }
    return {WMResult::Done};
  } );

  this->AddCommandHandler( CommandID::StructureEditButton, [this] ( int, HWND ) -> WMResult {
    this->SetToStructure( *current_structure_ );
    StructuresEditDialog dialog( project_.GetStructures() );
    if ( dialog.ShowDialog( *this ) ) {
      project_.GetStructures() = dialog.GetStructures();
      current_structure_ = project_.GetStructures().front().get();
      this->SetStructuresComboBox();
      this->SetFromStructure( *current_structure_ );
    }
    return {WMResult::Done};
  } );

  this->AddCommandHandler( CommandID::Ok, [this] ( int, HWND ) -> WMResult {
    this->SetToStructure( *current_structure_ );
    common_panel_->SetToProject( project_ );
    project_.name_ = name_edit_.GetText();
    this->EndDialog( 1 );
    return {WMResult::Done};
  } );

  this->AddCommandHandler( CommandID::Cancel, [this] ( int, HWND ) -> WMResult {
    this->EndDialog( 0 );
    return {WMResult::Done};
  } );

  name_edit_.SetText( project_.name_ );
  this->SetStructuresComboBox();
  this->SetFromStructure( *current_structure_ );
  common_panel_->SetFromProject( project_ );

  name_label_.Show();
  name_edit_.Show();
  structure_select_label_.Show();
  structure_combo_box_.Show();
  structure_edit_button_.Show();
  tab_.Show();
  ok_button_.Show();
  cancel_button_.Show();

  // Show しないと表示されないので注意
  project_variables_dialog_.SetShowFileVariable( false );
  project_variables_dialog_.ShowDialog( *this );

  return true;
}


void
ProjectDialog::SetStructuresComboBox( void )
{
  structure_combo_box_.Clear();
  for ( auto& structure : project_.GetStructures() ) {
    structure_combo_box_.PushWithData( structure->name_, structure.get() );
  }
  structure_combo_box_.SetSelectByItemData( current_structure_ );
}

void
ProjectDialog::SetFromStructure( const Project::Structure& structure )
{
  application_panel_->SetFromStructure( structure );
  compile1_panel_->SetFromStructure( structure );
  compile2_panel_->SetFromStructure( structure );
  warnings_panel_->SetFromStructure( structure );
  link_panel_->SetFromStructure( structure );
  others_panel_->SetFromStructure( structure );
}

void
ProjectDialog::SetToStructure( Project::Structure& structure )
{
  application_panel_->SetToStructure( structure );
  compile1_panel_->SetToStructure( structure );
  compile2_panel_->SetToStructure( structure );
  warnings_panel_->SetToStructure( structure );
  link_panel_->SetToStructure( structure );
  others_panel_->SetToStructure( structure );
}

// -- PanelBase -----
ProjectDialog::PanelBase::PanelBase( ProjectDialog& parent ) :
parent_( parent )
{
}

// -- ApplicationPanel -----
ProjectDialog::ApplicationPanel::ApplicationPanel( ProjectDialog& parent ) :
PanelBase( parent )
{
}

bool
ProjectDialog::ApplicationPanel::CreatedInternal( void )
{
  this->TtPanel::CreatedInternal();

  platform_label_.Create( {this} );
  platform_combo_.Create( {this} );
  target_kind_label_.Create( {this} );
  target_kind_combo_.Create( {this} );
  output_directory_label_.Create( {this} );
  output_directory_edit_.Create( {this} );
  target_label_.Create( {this} );
  target_edit_.Create( {this} );
  target_argument_label_.Create( {this} );
  target_argument_edit_.Create( {this} );
  target_current_label_.Create( {this} );
  target_current_edit_.Create( {this} );
  target_use_output_check_.Create( {this, CommandID::TargetUseOutputCheck} );
  target_use_input_check_.Create( {this} );
  show_variables_button_.Create( {this, CommandID::ShowProjectVariablesButton} );
  help_label_.Create( {this} );

  this->RegisterWMSize( [this] ( int, int w, int ) -> WMResult {
    platform_label_.SetPositionSize(            4,   8,     128,  16 );
    platform_combo_.SetPositionSize(          136,   4,      84,  70 );
    target_kind_label_.SetPositionSize(         4,  36,     128,  16 );
    target_kind_combo_.SetPositionSize(       136,  32,     188,  70 );
    output_directory_label_.SetPositionSize(    4,  64,     128,  16 );
    output_directory_edit_.SetPositionSize(   136,  60, w - 148,  20 );
    target_label_.SetPositionSize(              4,  92,     128,  16 );
    target_edit_.SetPositionSize(             136,  88, w - 148,  20 );
    target_argument_label_.SetPositionSize(     4, 130,     128,  16 );
    target_argument_edit_.SetPositionSize(    136, 126, w - 148,  20 );
    target_current_label_.SetPositionSize(      4, 158,     128,  16 );
    target_current_edit_.SetPositionSize(     136, 154, w - 148,  20 );
    target_use_output_check_.SetPositionSize(   8, 182,     200,  20 );
    target_use_input_check_.SetPositionSize(    8, 206,     200,  20 );
    show_variables_button_.SetPositionSize(     8, 238,     200,  28 );
    help_label_.SetPositionSize(              220, 248,     340,  16 );
    return {WMResult::Done};
  } );

  platform_label_.SetText(          "プラットフォーム" );
  target_kind_label_.SetText(       "ターゲット種類" );
  output_directory_label_.SetText(  "出力ディレクトリ名" );
  target_label_.SetText(            "出力ファイル名" );
  target_argument_label_.SetText(   "実行時引数" );
  target_current_label_.SetText(    "実行時作業フォルダ" );
  target_use_output_check_.SetText( "実行結果をタブに出力する。" );
  target_use_input_check_.SetText(  "標準入力ダイアログを使用する。" );
  show_variables_button_.SetText(   "使用可能変数一覧ダイアログの表示" );
  help_label_.SetText(              "※実行時引数と実行時作業フォルダでは変数が使用できます。" );

  platform_combo_.PushWithData( "x86", Platform::X86 );
  platform_combo_.PushWithData( "x64", Platform::X64 );

  target_kind_combo_.PushWithData( "Windowsアプリケーション",      Project::TargetKind::Windows );
  target_kind_combo_.PushWithData( "コンソールアプリケーション",   Project::TargetKind::Console );
  target_kind_combo_.PushWithData( "ダイナミックリンクライブラリ", Project::TargetKind::DynamicLinkLibrary );
  target_kind_combo_.PushWithData( "スタティックライブラリ",       Project::TargetKind::StaticLibrary );

  this->AddCommandHandler( CommandID::TargetUseOutputCheck, [this] ( int, HWND ) -> WMResult {
    target_use_input_check_.SetEnabled( target_use_output_check_.GetCheck() );
    return {WMResult::Done};
  } );
  this->AddCommandHandler( CommandID::ShowProjectVariablesButton, [this] ( int, HWND ) -> WMResult {
    parent_.project_variables_dialog_.Show();
    return {WMResult::Done};
  } );

  platform_label_.Show();
  platform_combo_.Show();
  target_kind_label_.Show();
  target_kind_combo_.Show();
  output_directory_label_.Show();
  output_directory_edit_.Show();
  target_label_.Show();
  target_edit_.Show();
  target_argument_label_.Show();
  target_argument_edit_.Show();
  target_current_label_.Show();
  target_current_edit_.Show();
  target_use_output_check_.Show();
  target_use_input_check_.Show();
  show_variables_button_.Show();
  help_label_.Show();

  return true;
}

void
ProjectDialog::ApplicationPanel::SetFromStructure( const Project::Structure& structure )
{
  platform_combo_.SetSelectByItemData( structure.platform_ );
  target_kind_combo_.SetSelectByItemData( structure.target_kind_ );
  target_edit_.SetText( structure.target_name_ );
  output_directory_edit_.SetText( structure.output_directory_name_ );
  target_argument_edit_.SetText( structure.target_argument_ );
  target_current_edit_.SetText( structure.target_current_directory_ );
  target_use_output_check_.SetCheck( structure.target_use_output_edit_ );
  target_use_input_check_.SetCheck( structure.target_use_input_dialog_ );

  target_use_input_check_.SetEnabled( target_use_output_check_.GetCheck() );
}

void
ProjectDialog::ApplicationPanel::SetToStructure( Project::Structure& structure )
{
  structure.platform_ = platform_combo_.GetSelectedItemData();
  structure.target_kind_ = target_kind_combo_.GetSelectedItemData();
  structure.target_name_ = target_edit_.GetText();
  structure.output_directory_name_ = output_directory_edit_.GetText();
  structure.target_argument_ = target_argument_edit_.GetText();
  structure.target_current_directory_ = target_current_edit_.GetText();
  structure.target_use_output_edit_ = target_use_output_check_.GetCheck();
  structure.target_use_input_dialog_ = target_use_input_check_.GetCheck();
}


// -- Compile1Panel -----
ProjectDialog::Compile1Panel::Compile1Panel( ProjectDialog& parent ) :
PanelBase( parent )
{
}

bool
ProjectDialog::Compile1Panel::CreatedInternal( void )
{
  this->TtPanel::CreatedInternal();

  optimize_label_.Create( {this} );
  optimize_combo_.Create( {this} );
  runtime_label_.Create( {this} );
  runtime_combo_.Create( {this} );
  use_exception_check_.Create( {this} );
  pch_use_check_.Create( {this, CommandID::PrecompiledHeaderUseCheck} );
  pch_source_label_.Create( {this} );
  pch_source_combo_.Create( {this} );
  pch_header_label_.Create( {this} );
  pch_header_combo_.Create( {this} );
  pch_force_check_.Create( {this} );

  this->RegisterWMSize( [this] ( int, int w, int ) -> WMResult {
    optimize_label_.SetPositionSize(          4,   8, 250,  16 );
    optimize_combo_.SetPositionSize(          4,  24, 250, 100 );
    runtime_label_.SetPositionSize(           4,  60, 250,  16 );
    runtime_combo_.SetPositionSize(           4,  76, 250,  80 );
    use_exception_check_.SetPositionSize(     4, 112, 250,  20 );
    pch_use_check_.SetPositionSize(     w - 280,   4, 250,  20 );
    pch_source_label_.SetPositionSize(  w - 280,  28, 250,  16 );
    pch_source_combo_.SetPositionSize(  w - 280,  44, 250, 240 );
    pch_header_label_.SetPositionSize(  w - 280,  78, 250,  16 );
    pch_header_combo_.SetPositionSize(  w - 280,  94, 250, 240 );
    pch_force_check_.SetPositionSize(   w - 280, 128, 280,  20 );
    return {WMResult::Done};
  } );

  optimize_label_.SetText( "最適化" );
  runtime_label_.SetText( "使用するランタイムライブラリ" );
  use_exception_check_.SetText( "C++ の例外を有効にする。 (/EHsc)" );
  pch_use_check_.SetText( "プリコンパイル済みヘッダーを使用する。(/Yu)" );
  pch_source_label_.SetText( "作成用ソースファイル" );
  pch_header_label_.SetText( "対象ヘッダファイル(/Yc)" );
  pch_force_check_.SetText( "対象ヘッダファイルを強制インクルードする。(/FI)" );

  optimize_combo_.PushWithData( "指定なし",               Project::OptimizeKind::NoSpecify );
  optimize_combo_.PushWithData( "無効 (/Od)",             Project::OptimizeKind::NoOptimize );
  optimize_combo_.PushWithData( "サイズの最小化 (/O1)",   Project::OptimizeKind::Smallest );
  optimize_combo_.PushWithData( "実行速度の最大化 (/O2)", Project::OptimizeKind::Fastest );
  optimize_combo_.PushWithData( "最大限の最適化 (/Ox)",   Project::OptimizeKind::Full );

  runtime_combo_.PushWithData( "マルチスレッド (/MT)",               Project::RuntimeLibrary::Static );
  runtime_combo_.PushWithData( "マルチスレッド デバッグ (/MTd)",     Project::RuntimeLibrary::StaticDebug );
  runtime_combo_.PushWithData( "マルチスレッド DLL (/MD)",           Project::RuntimeLibrary::Dynamic );
  runtime_combo_.PushWithData( "マルチスレッド デバッグ DLL (/MDd)", Project::RuntimeLibrary::DynamicDebug );

  for ( auto& file : parent_.project_.GetFiles() ) {
    if ( file->TypeIs<Project::SourceFile>() ) {
      pch_source_combo_.Push( file->GetPath() );
    }
    if ( file->TypeIs<Project::HeaderFile>() ) {
      pch_header_combo_.Push( file->GetPath() );
    }
  }

  this->AddCommandHandler( CommandID::PrecompiledHeaderUseCheck, [this] ( int code, HWND ) -> WMResult {
    if ( code == BN_CLICKED ) {
      this->SetControlEnabled();
    }
    return {WMResult::Done};
  } );

  optimize_label_.Show();
  optimize_combo_.Show();
  runtime_label_.Show();
  runtime_combo_.Show();
  use_exception_check_.Show();
  pch_use_check_.Show();
  pch_source_label_.Show();
  pch_source_combo_.Show();
  pch_header_label_.Show();
  pch_header_combo_.Show();
  pch_force_check_.Show();

  return true;
}

void
ProjectDialog::Compile1Panel::SetFromStructure( const Project::Structure& structure )
{
  optimize_combo_.SetSelectByItemData( structure.optimize_kind_ );
  runtime_combo_.SetSelectByItemData( structure.runtime_library_ );
  use_exception_check_.SetCheck( structure.use_exception_ );
  pch_use_check_.SetCheck( structure.use_precompile_header_ );
  pch_source_combo_.SetText( structure.precompiled_header_source_ );
  pch_header_combo_.SetText( structure.precompiled_header_header_ );
  pch_force_check_.SetCheck( structure.precompiled_header_force_include_ );
  this->SetControlEnabled();
}

void
ProjectDialog::Compile1Panel::SetToStructure( Project::Structure& structure )
{
  structure.optimize_kind_             = optimize_combo_.GetSelectedItemData();
  structure.runtime_library_           = runtime_combo_.GetSelectedItemData();
  structure.use_exception_             = use_exception_check_.GetCheck();
  structure.use_precompile_header_     = pch_use_check_.GetCheck();
  structure.precompiled_header_source_ = pch_source_combo_.GetText();
  structure.precompiled_header_header_ = pch_header_combo_.GetText();
  structure.precompiled_header_force_include_ = pch_force_check_.GetCheck();
}


void
ProjectDialog::Compile1Panel::SetControlEnabled( void )
{
  bool flag = pch_use_check_.GetCheck();
  pch_source_label_.SetEnabled( flag );
  pch_source_combo_.SetEnabled( flag );
  pch_header_label_.SetEnabled( flag );
  pch_header_combo_.SetEnabled( flag );
  pch_force_check_.SetEnabled( flag );
}


// -- Compile2Panel -----
ProjectDialog::Compile2Panel::Compile2Panel( ProjectDialog& parent ) :
PanelBase( parent )
{
}

bool
ProjectDialog::Compile2Panel::CreatedInternal( void )
{
  this->TtPanel::CreatedInternal();

  include_label_.Create( {this} );
  include_list_.Create( {this} );
  include_add_button_.Create( {this, CommandID::IncludeAddButton} );
  include_edit_button_.Create( {this, CommandID::IncludeEditButton} );
  include_delete_button_.Create( {this, CommandID::IncludeDeleteButton} );
  include_convert_button_.Create( {this, CommandID::IncludeConvertButton} );
  defines_label_.Create( {this} );
  defines_list_.Create( {this, CommandID::DefinesList} );
  defines_edit_.Create( {this} );
  defines_add_button_.Create( {this, CommandID::DefinesAddButton} );
  defines_edit_button_.Create( {this, CommandID::DefinesEditButton} );
  defines_delete_button_.Create( {this, CommandID::DefinesDeleteButton} );

  this->RegisterWMSize( [this] ( int, int w, int ) -> WMResult {
    include_label_.SetPositionSize(                4,   8,      128,  16 );
    include_list_.SetPositionSize(                 4,  28,  w -  72, 150 );
    include_add_button_.SetPositionSize(     w -  64,  28,       60,  24 );
    include_edit_button_.SetPositionSize(    w -  64,  56,       60,  24 );
    include_delete_button_.SetPositionSize(  w -  64,  84,       60,  24 );
    include_convert_button_.SetPositionSize( w -  64, 112,       60,  36 );
    defines_label_.SetPositionSize(                4, 186,      128,  16 );
    defines_list_.SetPositionSize(                 4, 206,  w - 172, 128 );
    defines_edit_.SetPositionSize(           w - 164, 206,      148,  20 );
    defines_add_button_.SetPositionSize(     w - 164, 234,       60,  24 );
    defines_edit_button_.SetPositionSize(    w - 164, 262,       60,  24 );
    defines_delete_button_.SetPositionSize(  w - 164, 290,       60,  24 );
    return {WMResult::Done};
  } );

  include_label_.SetText( "インクルードパス(/I)" );
  include_add_button_.SetText( "追加" );
  include_edit_button_.SetText( "変更" );
  include_delete_button_.SetText( "削除" );
  include_convert_button_.SetText( "相対化\n絶対化" );
  defines_label_.SetText( "コンパイル条件(/D)" );
  defines_add_button_.SetText( "追加" );
  defines_edit_button_.SetText( "変更" );
  defines_delete_button_.SetText( "削除" );

  include_list_.SetItemHeight( 16 );
  defines_list_.SetItemHeight( 16 );

  this->AddCommandHandler( CommandID::IncludeAddButton, [this] ( int, HWND ) -> WMResult {
    TtFolderBrowserDialog dialog;
    dialog.SetDescription( "インクルードパスを選択してください。" );
    if ( dialog.ShowDialog( *this ) ) {
      include_list_.Push( dialog.GetSelectedPath() );
    }
    return {WMResult::Done};
  } );
  this->AddCommandHandler( CommandID::IncludeEditButton, [this] ( int, HWND ) -> WMResult {
    int index = include_list_.GetCurrent();
    if ( index != LB_ERR ) {
      TtFolderBrowserDialog dialog;
      dialog.SetDescription( "インクルードパスを選択してください。" );
      auto path = include_list_.GetCurrentText();
      if ( TtPath::IsRelative( path ) ) {
        dialog.SetSelectedPath( Utility::TogglePathFullRelative( path, TtPath::DirName( parent_.project_.path_ ) ) );
      }
      else {
        dialog.SetSelectedPath( include_list_.GetCurrentText() );
      }
      if ( dialog.ShowDialog( *this ) ) {
        include_list_.DeleteAt( index );
        include_list_.InsertAt( index, dialog.GetSelectedPath() );
      }
    }
    return {WMResult::Done};
  } );
  this->AddCommandHandler( CommandID::IncludeDeleteButton, [this] ( int, HWND ) -> WMResult {
    int index = include_list_.GetCurrent();
    if ( index != LB_ERR ) {
      include_list_.DeleteAt( index );
    }
    return {WMResult::Done};
  } );
  this->AddCommandHandler( CommandID::IncludeConvertButton, [this] ( int, HWND ) -> WMResult {
    int index = include_list_.GetCurrent();
    if ( index != LB_ERR ) {
      std::string new_path = Utility::TogglePathFullRelative( include_list_.GetTextAt( index ), TtPath::DirName( parent_.project_.path_ ) );
      include_list_.DeleteAt( index );
      include_list_.InsertAt( index, new_path );
      include_list_.SetCurrent( index );
    }
    return {WMResult::Done};
  } );

  this->AddCommandHandler( CommandID::DefinesList, [this] ( int code, HWND ) -> WMResult {
    if ( code == LBN_SELCHANGE ) {
      if ( defines_list_.GetCurrent() != LB_ERR ) {
        defines_edit_.SetText( defines_list_.GetCurrentText() );
      }
    }
    return {WMResult::Done};
  } );

  this->AddCommandHandler( CommandID::DefinesAddButton, [this] ( int, HWND ) -> WMResult {
    std::string tmp = defines_edit_.GetText();
    if ( tmp.empty() ) {
      return {WMResult::Done};
    }
    for ( int i = 0; i < defines_list_.GetCount(); ++i ) {
      if ( defines_list_.GetTextAt( i ) == tmp ) {
        return {WMResult::Done};
      }
    }
    defines_list_.Push( tmp );
    return {WMResult::Done};
  } );
  this->AddCommandHandler( CommandID::DefinesEditButton, [this] ( int, HWND ) -> WMResult {
    int index = defines_list_.GetCurrent();
    if ( index == LB_ERR ) {
      return {WMResult::Done};
    }
    std::string tmp = defines_edit_.GetText();
    if ( tmp.empty() ) {
      return {WMResult::Done};
    }
    for ( int i = 0; i < defines_list_.GetCount(); ++i ) {
      if ( defines_list_.GetTextAt( i ) == tmp ) {
        return {WMResult::Done};
      }
    }
    defines_list_.DeleteAt( index );
    defines_list_.InsertAt( index, tmp );
    return {WMResult::Done};
  } );
  this->AddCommandHandler( CommandID::DefinesDeleteButton, [this] ( int, HWND ) -> WMResult {
    int index = defines_list_.GetCurrent();
    if ( index != LB_ERR ) {
      defines_list_.DeleteAt( index );
    }
    return {WMResult::Done};
  } );

  include_label_.Show();
  include_list_.Show();
  include_add_button_.Show();
  include_edit_button_.Show();
  include_delete_button_.Show();
  include_convert_button_.Show();
  defines_label_.Show();
  defines_list_.Show();
  defines_edit_.Show();
  defines_add_button_.Show();
  defines_edit_button_.Show();
  defines_delete_button_.Show();

  return true;
}

void
ProjectDialog::Compile2Panel::SetFromStructure( const Project::Structure& structure )
{
  include_list_.Clear();
  for ( const std::string& path : structure.include_paths_ ) {
    include_list_.Push( path );
  }
  defines_list_.Clear();
  for ( const std::string& def : structure.defines_ ) {
    defines_list_.Push( def );
  }
}

void
ProjectDialog::Compile2Panel::SetToStructure( Project::Structure& structure )
{
  structure.include_paths_.clear();
  for ( int i = 0; i < include_list_.GetCount(); ++i ) {
    structure.include_paths_.push_back( include_list_.GetTextAt( i ) );
  }
  structure.defines_.clear();
  for ( int i = 0; i < defines_list_.GetCount(); ++i ) {
    structure.defines_.push_back( defines_list_.GetTextAt( i ) );
  }
}


// -- WarningsPanel -----
ProjectDialog::WarningsPanel::WarningsPanel( ProjectDialog& parent ) :
PanelBase( parent )
{
}

bool
ProjectDialog::WarningsPanel::CreatedInternal( void )
{
  this->TtPanel::CreatedInternal();

  level_label_.Create( {this} );
  level_combo_.Create( {this} );
  number_label_.Create( {this} );
  number_list_.Create( {this} );
  number_edit_.Create( {this} );
  add_button_.Create( {this, CommandID::WarningsAddButton} );
  delete_button_.Create( {this, CommandID::WarningsDeleteButton} );
  error_if_warning_check_.Create( {this} );

  this->RegisterWMSize( [this] ( int, int w, int ) -> WMResult {
    NOT_USE( w );
    level_label_.SetPositionSize(              4,   8,   98,  16 );
    level_combo_.SetPositionSize(            104,   4,  108,  96 );
    number_label_.SetPositionSize(             4,  32,  180,  16 );
    number_list_.SetPositionSize(              4,  48,  128, 280 );
    number_edit_.SetPositionSize(            144,  48,   60,  20 );
    add_button_.SetPositionSize(             144,  80,   60,  24 );
    delete_button_.SetPositionSize(          144, 108,   60,  24 );
    error_if_warning_check_.SetPositionSize( 260,  12,  200,  20 );
    return {WMResult::Done};
  } );

  level_label_.SetText( "警告レベル(/Wn)" );
  number_label_.SetText( "非表示にする警告番号(/wd)" );
  add_button_.SetText( "追加" );
  delete_button_.SetText( "削除" );
  error_if_warning_check_.SetText( "警告をエラーとして扱う (/WX)" );

  level_combo_.Push( "レベル0" );
  level_combo_.Push( "レベル1" );
  level_combo_.Push( "レベル2" );
  level_combo_.Push( "レベル3" );
  level_combo_.Push( "レベル4" );
  level_combo_.Push( "すべて表示" );

  number_list_.SetItemHeight( 16 );

  this->AddCommandHandler( CommandID::WarningsAddButton, [this] ( int, HWND ) -> WMResult {
    std::string tmp = number_edit_.GetText();
    if ( NOT( tmp.empty() ) ) {
      number_list_.Push( tmp );
    }
    return {WMResult::Done};
  } );
  this->AddCommandHandler( CommandID::WarningsDeleteButton, [this] ( int, HWND ) -> WMResult {
    int index = number_list_.GetCurrent();
    if ( index != LB_ERR ) {
      number_list_.DeleteAt( index );
    }
    return {WMResult::Done};
  } );

  level_label_.Show();
  level_combo_.Show();
  number_label_.Show();
  number_list_.Show();
  number_edit_.Show();
  add_button_.Show();
  delete_button_.Show();
  error_if_warning_check_.Show();

  return true;
}

void
ProjectDialog::WarningsPanel::SetFromStructure( const Project::Structure& structure )
{
  level_combo_.SetSelect( structure.warning_level_ );
  number_list_.Clear();
  for ( int number : structure.no_warnings_ ) {
    number_list_.Push( TtUtility::ToStringFrom( number ) );
  }
  error_if_warning_check_.SetCheck( structure.error_if_warning_ );
}

void
ProjectDialog::WarningsPanel::SetToStructure( Project::Structure& structure )
{
  structure.warning_level_ = level_combo_.GetSelectedIndex();
  structure.no_warnings_.clear();
  for ( int i = 0; i < number_list_.GetCount(); ++i ) {
    int tmp;
    if ( TtUtility::StringToInteger( number_list_.GetTextAt( i ), &tmp ) ) {
      structure.no_warnings_.push_back( tmp );
    }
  }
  structure.error_if_warning_ = error_if_warning_check_.GetCheck();
}


// -- LinkPanel -----
ProjectDialog::LinkPanel::LinkPanel( ProjectDialog& parent ) :
PanelBase( parent )
{
}

bool
ProjectDialog::LinkPanel::CreatedInternal( void )
{
  this->TtPanel::CreatedInternal();

  link_label_.Create( {this} );
  link_list_.Create( {this} );
  add_button_.Create( {this, CommandID::LinkAddButton} );
  edit_button_.Create( {this, CommandID::LinkEditButton} );
  delete_button_.Create( {this, CommandID::LinkDeleteButton} );
  convert_button_.Create( {this, CommandID::LinkConvertButton} );

  this->RegisterWMSize( [this] ( int, int w, int ) -> WMResult {
    link_label_.SetPositionSize(           4,   8,      128,  16 );
    link_list_.SetPositionSize(            4,  28,  w -  72, 300 );
    add_button_.SetPositionSize(     w -  64,  28,       60,  24 );
    edit_button_.SetPositionSize(    w -  64,  56,       60,  24 );
    delete_button_.SetPositionSize(  w -  64,  84,       60,  24 );
    convert_button_.SetPositionSize( w -  64, 112,       60,  36 );
    return {WMResult::Done};
  } );

  link_label_.SetText( "ライブラリファイルの指定" );
  add_button_.SetText( "追加" );
  edit_button_.SetText( "変更" );
  delete_button_.SetText( "削除" );
  convert_button_.SetText( "相対化\n絶対化" );

  link_list_.SetItemHeight( 16 );

  this->AddCommandHandler( CommandID::LinkAddButton, [this] ( int, HWND ) -> WMResult {
    TtOpenFileDialog dialog;
    dialog.GetFilters().push_back( {"ライブラリファイル(*.lib)", "*.lib"} );

    if ( dialog.ShowDialog( *this ) ) {
      link_list_.Push( dialog.GetFileName() );
    }
    return {WMResult::Done};
  } );
  this->AddCommandHandler( CommandID::LinkEditButton, [this] ( int, HWND ) -> WMResult {
    int index = link_list_.GetCurrent();
    if ( index != LB_ERR ) {
      TtOpenFileDialog dialog;
      dialog.GetFilters().push_back( {"ライブラリファイル(*.lib)", "*.lib"} );
      dialog.SetFileName( link_list_.GetCurrentText() );

      if ( dialog.ShowDialog( *this ) ) {
        link_list_.DeleteAt( index );
        link_list_.InsertAt( index, dialog.GetFileName() );
      }
    }
    return {WMResult::Done};
  } );
  this->AddCommandHandler( CommandID::LinkDeleteButton, [this] ( int, HWND ) -> WMResult {
    int index = link_list_.GetCurrent();
    if ( index != LB_ERR ) {
      link_list_.DeleteAt( index );
    }
    return {WMResult::Done};
  } );
  this->AddCommandHandler( CommandID::LinkConvertButton, [this] ( int, HWND ) -> WMResult {
    int index = link_list_.GetCurrent();
    if ( index != LB_ERR ) {
      std::string new_path = Utility::TogglePathFullRelative( link_list_.GetTextAt( index ), TtPath::DirName( parent_.project_.path_ ) );
      link_list_.DeleteAt( index );
      link_list_.InsertAt( index, new_path );
      link_list_.SetCurrent( index );
    }
    return {WMResult::Done};
  } );

  link_label_.Show();
  link_list_.Show();
  add_button_.Show();
  edit_button_.Show();
  delete_button_.Show();
  convert_button_.Show();

  return true;
}

void
ProjectDialog::LinkPanel::SetFromStructure( const Project::Structure& structure )
{
  link_list_.Clear();
  for ( const std::string& path : structure.library_paths_ ) {
    link_list_.Push( path );
  }
}

void
ProjectDialog::LinkPanel::SetToStructure( Project::Structure& structure )
{
  structure.library_paths_.clear();
  for ( int i = 0; i < link_list_.GetCount(); ++i ) {
    structure.library_paths_.push_back( link_list_.GetTextAt( i ) );
  }
}


// -- OthersPanel -----
ProjectDialog::OthersPanel::OthersPanel( ProjectDialog& parent ) :
PanelBase( parent )
{
}

bool
ProjectDialog::OthersPanel::CreatedInternal( void )
{
  this->TtPanel::CreatedInternal();

  no_logo_check_.Create( {this} );
  compiler_other_label_.Create( {this} );
  compiler_other_edit_.Create( {this} );
  resource_compiler_label_.Create( {this} );
  resource_compiler_edit_.Create( {this} );
  extension_script_label_.Create( {this} );
  extension_script_edit_.Create( {this} );
  extension_script_button_.Create( {this, CommandID::ExtensionScriptButton} );

  this->RegisterWMSize( [this] ( int, int w, int ) -> WMResult {
    no_logo_check_.SetPositionSize(                4,   8,      256,  16 );
    compiler_other_label_.SetPositionSize(         4,  38,      128,  16 );
    compiler_other_edit_.SetPositionSize(          4,  56,  w -  12,  20 );
    resource_compiler_label_.SetPositionSize(      4,  88,      160,  16 );
    resource_compiler_edit_.SetPositionSize(       4, 106,  w -  12,  20 );
    extension_script_label_.SetPositionSize(       4, 138,      160,  16 );
    extension_script_edit_.SetPositionSize(        4, 156,  w -  54,  20 );
    extension_script_button_.SetPositionSize( w - 46, 156,       42,  20 );
    return {WMResult::Done};
  } );

  no_logo_check_.SetText( "著作権メッセージを表示しない (/nologo)" );
  compiler_other_label_.SetText( "その他オプション" );
  resource_compiler_label_.SetText( "リソースコンパイラ用オプション" );
  extension_script_label_.SetText( "拡張スクリプトのパス" );
  extension_script_button_.SetText( "参照..." );

  this->AddCommandHandler( CommandID::ExtensionScriptButton, [this] ( int, HWND ) -> WMResult {
    TtOpenFileDialog dialog;
    dialog.GetFilters().push_back( {"スクリプトファイル(*.*)", "*.*"} );
    dialog.SetFileName( extension_script_edit_.GetText() );
    if ( dialog.ShowDialog( *this ) ) {
      auto path = dialog.GetFileName();
      if ( NOT( TtPath::IsRelative( path ) ) ) {
        path = Utility::TogglePathFullRelative( path, TtPath::DirName( parent_.project_.path_ ) );
      }
      extension_script_edit_.SetText( path );
    }
    return {WMResult::Done};
  } );

  no_logo_check_.Show();
  compiler_other_label_.Show();
  compiler_other_edit_.Show();
  resource_compiler_label_.Show();
  resource_compiler_edit_.Show();
  extension_script_label_.Show();
  extension_script_edit_.Show();
  extension_script_button_.Show();
  return true;
}

void
ProjectDialog::OthersPanel::SetFromStructure( const Project::Structure& structure )
{
  no_logo_check_.SetCheck( structure.no_logo_ );
  compiler_other_edit_.SetText( structure.compiler_other_option_ );
  resource_compiler_edit_.SetText( structure.resource_compiler_option_ );
  extension_script_edit_.SetText( structure.extension_script_path_ );
}

void
ProjectDialog::OthersPanel::SetToStructure( Project::Structure& structure )
{
  structure.no_logo_ = no_logo_check_.GetCheck();
  structure.compiler_other_option_ = compiler_other_edit_.GetText();
  structure.resource_compiler_option_ = resource_compiler_edit_.GetText();
  structure.extension_script_path_ = extension_script_edit_.GetText();
}


// -- CommonPanel -----
ProjectDialog::CommonPanel::CommonPanel( ProjectDialog& parent ) :
PanelBase( parent )
{
}

bool
ProjectDialog::CommonPanel::CreatedInternal( void )
{
  this->TtPanel::CreatedInternal();

  attention_label_.Create( {this} );
  use_icon_check_.Create( {this, CommandID::UseIconCheck} );
  icon_change_button_.Create( {this, CommandID::IconChangeButton} );

  this->RegisterWMSize( [this] ( int, int, int ) -> WMResult {
    attention_label_.SetPositionSize(      4,   8, 320,  16 );
    use_icon_check_.SetPositionSize(       8,  28, 256,  16 );
    icon_change_button_.SetPositionSize(  68,  68, 100,  20 );
    return {WMResult::Done};
  } );

  attention_label_.SetText( "※ここの設定はプロジェクト構成によらず共通で設定されます。" );
  use_icon_check_.SetText( "タイトルバーに指定したアイコンを使用する。" );
  icon_change_button_.SetText( "アイコン変更..." );

  this->AddCommandHandler( CommandID::UseIconCheck, [this] ( int code, HWND ) -> WMResult {
    if ( code == BN_CLICKED ) {
      this->SetControlEnabled();
    }
    return {WMResult::Done};
  } );

  this->RegisterWMPaint( [this] ( void ) -> WMResult {
    if ( use_icon_check_.GetCheck() && icon_small_ && icon_large_ ) {
      PAINTSTRUCT ps;
      HDC hdc = ::BeginPaint( handle_, &ps );
      ::DrawIconEx( hdc, 18, 54, icon_small_->GetHandle(), 16, 16, 0, NULL, DI_NORMAL );
      ::DrawIconEx( hdc, 18, 74, icon_large_->GetHandle(), 32, 32, 0, NULL, DI_NORMAL );
      ::EndPaint( handle_, &ps );
    }
    return {WMResult::Done};
  }, false );

  this->AddCommandHandler( CommandID::IconChangeButton, [this] ( int, HWND ) -> WMResult {
    TtPickIconDialog dialog( parent_.project_.ConvertAbsolutePath( icon_file_path_ ), icon_file_index_ );
    if ( dialog.ShowDialog( *this ) ) {
      icon_file_path_ = dialog.GetPath();
      icon_file_index_ = dialog.GetIndex();
      this->SetIcon();
    }
    return {WMResult::Done};
  } );

  attention_label_.Show();
  use_icon_check_.Show();
  icon_change_button_.Show();
  return true;
}

void
ProjectDialog::CommonPanel::SetFromProject( const Project& project )
{
  use_icon_check_.SetCheck( project.use_icon_ );
  icon_file_path_  = project.icon_file_path_;
  icon_file_index_ = project.icon_file_index_;
  this->SetControlEnabled();
}

void
ProjectDialog::CommonPanel::SetToProject( Project& project )
{
  project.use_icon_ = use_icon_check_.GetCheck();
  project.icon_file_path_ = project.ConvertRelativePath( icon_file_path_ );
  project.icon_file_index_ = icon_file_index_;
}

void
ProjectDialog::CommonPanel::SetControlEnabled( void )
{
  icon_change_button_.SetEnabled( use_icon_check_.GetCheck() );
  this->SetIcon();
}

void
ProjectDialog::CommonPanel::SetIcon( void )
{
  TtUtility::DestructorCall( [this] ( void ) { this->Invalidate(); } );
  if ( use_icon_check_.GetCheck() ) {
    try {
      icon_small_.emplace( TtIcon::CreateFromFileAsSmall( parent_.project_.ConvertAbsolutePath( icon_file_path_ ), icon_file_index_ ) );
      icon_large_.emplace( TtIcon::CreateFromFileAsLarge( parent_.project_.ConvertAbsolutePath( icon_file_path_ ), icon_file_index_ ) );
      return;
    }
    catch ( TtWindowsSystemCallException ) {
    }
  }
  icon_small_.reset();
  icon_large_.reset();
}

// -- StructuresEditDialog ---------------------------------------------
StructuresEditDialog::StructuresEditDialog( const std::vector<std::shared_ptr<Project::Structure>>& source ) :
structures_(),
icon_( Image::ICONS[Image::Index::ProjectSettings] ),

list_label_(),
list_(),
edit_label_(),
edit_(),
copy_button_(),
delete_button_(),
ok_button_(),
cancel_button_()
{
  for ( const std::shared_ptr<Project::Structure>& s : source ) {
    structures_.push_back( std::make_shared<Project::Structure>( *s ) );
  }
}

bool
StructuresEditDialog::Created( void )
{
  this->SetText( "構成の編集" );
  this->SetIconAsSmall( icon_ );

  struct CommandID {
    enum ID : int {
      Copy = 10001,
      Delete,
      Ok,
      Cancel,
    };
  };

  list_label_.Create( {this} );
  list_.Create( {this} );
  edit_label_.Create( {this} );
  edit_.Create( {this} );
  copy_button_.Create( {this, CommandID::Copy} );
  delete_button_.Create( {this, CommandID::Delete} );
  ok_button_.Create( {this, CommandID::Ok} );
  cancel_button_.Create( {this, CommandID::Cancel} );

  this->SetCenterRelativeToParent();
  this->SetClientSize( 300, 268, false );
  {
    int w = this->GetClientWidth();
    int h = this->GetClientHeight();
    list_label_.SetPositionSize(           4,       8,     128,      16 );
    list_.SetPositionSize(                 4,      28, w - 120, h -  56 );
    edit_label_.SetPositionSize(     w - 108,       8,     100,      16 );
    edit_.SetPositionSize(           w - 108,      28,     100,      20 );
    copy_button_.SetPositionSize(    w - 108,      56,     100,      24 );
    delete_button_.SetPositionSize(  w - 108,      84,     100,      24 );
    ok_button_.SetPositionSize(      w - 196, h -  26,      88,      22 );
    cancel_button_.SetPositionSize(  w -  96, h -  26,      88,      22 );
  }

  list_label_.SetText( "構成一覧" );
  edit_label_.SetText( "作成する構成名" );
  copy_button_.SetText( "コピーして作成" );
  delete_button_.SetText( "削除" );
  ok_button_.SetText( "OK" );
  cancel_button_.SetText( "キャンセル" );

  list_.SetItemHeight( 16 );

  this->AddCommandHandler( CommandID::Copy, [this] ( int, HWND ) -> WMResult {
    TtMessageBoxOk message_box( "", "エラー", TtMessageBox::Icon::ERROR );
    std::string name = edit_.GetText();
    if ( name.empty() ) {
      message_box.SetMessage( "作成する構成名を入力してください。" );
      message_box.ShowDialog( *this );
      return {WMResult::Done};
    }
    for ( auto& s : structures_ ) {
      if ( s->name_ == name ) {
        message_box.SetMessage( "構成名 " + name + " は既に存在します。" );
        message_box.ShowDialog( *this );
        return {WMResult::Done};
      }
    }
    int index = list_.GetCurrent();
    if ( index == LB_ERR ) {
      message_box.SetMessage( "コピー元となる構成を選んでください。" );
      message_box.ShowDialog( *this );
      return {WMResult::Done};
    }
    structures_.push_back( std::make_shared<Project::Structure>( *structures_[index] ) );
    structures_.back()->name_ = name;
    list_.Push( name );
    list_.UnsetCurrent();
    edit_.SetText( "" );
    return {WMResult::Done};
  } );
  this->AddCommandHandler( CommandID::Delete, [this] ( int, HWND ) -> WMResult {
    if ( structures_.size() >= 2 ) {
      int index = list_.GetCurrent();
      if ( index != LB_ERR ) {
        list_.DeleteAt( index );
        structures_.erase( structures_.begin() + index );
      }
    }
    return {WMResult::Done};
  } );
  this->AddCommandHandler( CommandID::Ok, [this] ( int, HWND ) -> WMResult {
    this->EndDialog( 1 );
    return {WMResult::Done};
  } );
  this->AddCommandHandler( CommandID::Cancel, [this] ( int, HWND ) -> WMResult {
    this->EndDialog( 0 );
    return {WMResult::Done};
  } );

  for ( auto& s : structures_ ) {
    list_.Push( s->name_ );
  }

  list_label_.Show();
  list_.Show();
  edit_label_.Show();
  edit_.Show();
  copy_button_.Show();
  delete_button_.Show();
  ok_button_.Show();
  cancel_button_.Show();

  return true;
}

std::vector<std::shared_ptr<Project::Structure>>
StructuresEditDialog::GetStructures()
{
  return structures_;
}
