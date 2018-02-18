// custom_control.cpp

#include "ttl_define.h"
#include "tt_string.h"
#include "tt_message_box.h"

#include "custom_control.h"

using namespace TTVCDeveloper;


// -- MainMenu -----------------------------------------------------------
MainMenu::MainMenu( void ) :
TtMenuBar( false ),
file_menu_( TtSubMenu::Create() ),
project_menu_( TtSubMenu::Create() ),
build_menu_( TtSubMenu::Create() ),
execute_menu_( TtSubMenu::Create() ),
tool_menu_( TtSubMenu::Create() ),
help_menu_( TtSubMenu::Create() ),
prototype_menu_( TtSubMenu::Create() ),

tool_project_folder_menu_( TtSubMenu::Create() ),

project_history_menu_( TtSubMenuCommand::Create() ),
current_structure_menu_( TtSubMenuCommand::Create() ),
external_program_menu_( TtSubMenuCommand::Create() )
{
  auto add_menu = [] ( TtMenu& menu, int command_id, Image::Index::Value image_index, const std::string& str ) {
    menu.AppendNewItem( command_id, str );
    if ( image_index != Image::Index::None ) {
      menu.GetLastItem().SetBmpImage( Image::BMPS[image_index] );
    }
  };
  using Index = Image::Index;

  this->AppendMenu( file_menu_, "ファイル(&F)" );
  add_menu( file_menu_, CommandID::NewProject,  Index::NewProject,  "新規プロジェクト(&N)...\bCtrl+N" );
  add_menu( file_menu_, CommandID::OpenProject, Index::OpenProject, "プロジェクトを開く(&O)...\bCtrl+O" );
  add_menu( file_menu_, CommandID::SaveProject, Index::SaveProject, "プロジェクトを上書き保存する(&S)\bCtrl+S" );
  file_menu_.AppendSeparator();
  file_menu_.AppendMenu( project_history_menu_, "履歴(&H)" );
  file_menu_.GetLastItem().SetBmpImage( Image::BMPS[Index::ProjectHistory] );
  add_menu( file_menu_, CommandID::DeleteProjectHistory, Index::None, "履歴を消去(&D)" );
  file_menu_.AppendSeparator();
  add_menu( file_menu_, CommandID::Close,       Index::None,        "終了(&X)" );

  this->AppendMenu( project_menu_, "プロジェクト(&P)" );
  add_menu( project_menu_, CommandID::AddNewFile,      Index::AddNewFile,      "プロジェクトに新規ファイルを追加(&N)..." );
  add_menu( project_menu_, CommandID::AddExistingFile, Index::AddExistingFile, "プロジェクトに既存ファイルを追加(&E)..." );
  add_menu( project_menu_, CommandID::DeleteElement,   Index::DeleteElement,   "プロジェクトから削除(&D)" );
  add_menu( project_menu_, CommandID::AddDirectory,    Index::AddDirectory,    "管理ディレクトリの追加(&F)" );
  project_menu_.AppendSeparator();
  add_menu( project_menu_, CommandID::ProjectSettings,        Index::ProjectSettings,        "プロジェクト設定(&P)..." );

  this->AppendMenu( build_menu_, "ビルド(&B)" );
  build_menu_.AppendMenu( current_structure_menu_, "現在のプロジェクト構成(&P)" );
  build_menu_.AppendSeparator();
  add_menu( build_menu_, CommandID::Compile, Index::Compile, "コンパイル(&C)\bF5" );
  add_menu( build_menu_, CommandID::Build,   Index::Build,   "メイク(&M)\bF6" );
  add_menu( build_menu_, CommandID::Rebuild, Index::Rebuild, "再構築(&R)\bF7" );
  add_menu( build_menu_, CommandID::Clean,   Index::Clean,   "クリーン(&C)\bF8" );

  this->AppendMenu( execute_menu_, "実行(&E)" );
  add_menu( execute_menu_, CommandID::Execute, Index::Execute, "実行(&E)\bF9" );

  this->AppendMenu( tool_menu_, "ツール(&T)" );
  add_menu( tool_menu_, CommandID::Settings,               Index::None,                   "環境設定(&S)..." );
  add_menu( tool_menu_, CommandID::DefaultProjectSettings, Index::DefaultProjectSettings, "デフォルトプロジェクトの編集(&D)..." );
  tool_menu_.AppendSeparator();
  {
    tool_menu_.AppendMenu( tool_project_folder_menu_, "プロジェクトフォルダを開く(&O)" ) ;
    add_menu( tool_project_folder_menu_, CommandID::OpenProjectFolderByExplorer, Index::None, "ウィンドウ(&W)" );
    add_menu( tool_project_folder_menu_, CommandID::OpenProjectFolderByConsole,  Index::None, "コンソール(&C)" );
  }
  tool_menu_.AppendMenu( external_program_menu_, "外部プログラム(&E)" );
  tool_menu_.GetLastItem().SetBmpImage( Image::BMPS[Index::ExternalProgram] );
  add_menu( tool_menu_, CommandID::StopExternalProgram, Index::StopExternalProgram, "外部プログラムを終了させる(&X)" );

  this->AppendMenu( help_menu_, "ヘルプ(&H)" );
  add_menu( help_menu_, CommandID::VersionInformation, Index::None, "バージョン情報(&A)..." );

  if ( IniFileOperation::LoadTestMode() ) {
    this->AppendMenu( prototype_menu_, "開発中" );
    add_menu( prototype_menu_, CommandID::Test1, Index::None, "Test1" );
    add_menu( prototype_menu_, CommandID::Test2, Index::None, "Test2" );
    add_menu( prototype_menu_, CommandID::Test3, Index::None, "Test3" );
    add_menu( prototype_menu_, CommandID::Test4, Index::None, "Test4" );
    add_menu( prototype_menu_, CommandID::Test5, Index::None, "Test5" );
  }
}

void
MainMenu::SetBeginningMode( void )
{
  this->SetMainMode( false );
}

void
MainMenu::SetNormalMode( void )
{
  this->SetMainMode( true );
}

void
MainMenu::SetMainMode( bool flag )
{
  file_menu_.GetItemAt( 2 ).SetEnabled( flag );

  project_menu_.GetItemAt( 0 ).SetEnabled( flag );
  project_menu_.GetItemAt( 1 ).SetEnabled( flag );
  project_menu_.GetItemAt( 2 ).SetEnabled( flag );
  project_menu_.GetItemAt( 3 ).SetEnabled( flag );
  project_menu_.GetItemAt( 5 ).SetEnabled( flag );

  build_menu_.GetItemAt( 2 ).SetEnabled( flag );
  build_menu_.GetItemAt( 3 ).SetEnabled( flag );
  build_menu_.GetItemAt( 4 ).SetEnabled( flag );
  build_menu_.GetItemAt( 5 ).SetEnabled( flag );

  execute_menu_.GetItemAt( 0 ).SetEnabled( flag );

  tool_project_folder_menu_.GetItemAt( 0 ).SetEnabled( flag );
  tool_project_folder_menu_.GetItemAt( 1 ).SetEnabled( flag );
  tool_menu_.GetItemAt( 4 ).SetEnabled( flag );
  tool_menu_.GetItemAt( 5 ).SetEnabled( false );
}


void
MainMenu::SetBuildingMode( void )
{
  this->SetBuildingModeFlag( false );
}

void
MainMenu::UnsetBuildingMode( void )
{
  this->SetBuildingModeFlag( true );
}

void
MainMenu::SetBuildingModeFlag( bool flag )
{
  build_menu_.GetItemAt( 2 ).SetEnabled( flag );
  build_menu_.GetItemAt( 3 ).SetEnabled( flag );
  build_menu_.GetItemAt( 4 ).SetEnabled( flag );
  build_menu_.GetItemAt( 5 ).SetEnabled( flag );
}


void
MainMenu::TreeViewItemSelected( MainTree::Item& item )
{
  if ( item.GetParameter()->TypeIs<Project::ProjectElement>() ) {
    project_menu_.GetItemAt( 2 ).SetEnabled( false );
  }
  else if ( item.GetParameter()->TypeIs<Project::Directory>() && item.HasChild() ) {
    project_menu_.GetItemAt( 2 ).SetEnabled( false );
  }
  else {
    project_menu_.GetItemAt( 2 ).SetEnabled( true );
  }
}


TtMenuItem
MainMenu::GetStopExternalProgramMenuItem( void )
{
  return tool_menu_.GetItemAt( 5 );
}


void
MainMenu::SetExternalProgramMenu( TtSubMenuCommand& menu )
{
  const int ExternalElement = 4;
  tool_menu_.InsertMenu( ExternalElement, menu, tool_menu_.GetItemAt( ExternalElement ).GetText() );
  tool_menu_.DeleteAt( ExternalElement + 1 );
  tool_menu_.GetItemAt( ExternalElement ).SetBmpImage( Image::BMPS[Image::Index::ExternalProgram] );
  external_program_menu_ = menu;
}


// -- OutputEdit ---------------------------------------------------------
TtFont
OutputEdit::DEFAUT_FONT = TtFont( ::CreateFont( 14, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
                                                SHIFTJIS_CHARSET, OUT_DEFAULT_PRECIS,
                                                CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
                                                DEFAULT_PITCH | FF_MODERN, nullptr ), true );


OutputEdit::OutputEdit( void ) :
file_path_and_line_handler_( nullptr ),
process_manager_()
{
  process_manager_.SetOutputReceiver( [this] ( const std::string& data ) {
    this->AddText( data );
  } );
}


static
struct ParseOutputLineResult {
  bool        success;
  std::string path;
  int         line;
}
parse_line_as_file_path_and_line( const std::string& str )
{
  auto tmp = [&str] ( char start, char end ) -> ParseOutputLineResult {
    enum class Status {
      Drive,
      Path,
      SpaceBeforeLine,
      Line,
    } status = Status::Drive;
    std::string path;
    std::string line;
    for ( const char* cp = str.c_str(); *cp != '\0'; ++cp ) {
      switch ( status ) {
      case Status::Drive:
        if ( isalpha( *cp ) && ( *(cp + 1) == ':' ) ) {
          path.append( cp, 2 );
          cp += 1;
        }
        else {
          path.append( cp, 1 );
        }
        status = Status::Path;
        break;

      case Status::Path:
        if ( *cp == start ) {
          status = Status::SpaceBeforeLine;
        }
        else {
          path.append( cp, 1 );
        }
        break;

      case Status::SpaceBeforeLine:
        if ( *cp != ' ' ) {
          if ( isdigit( *cp ) ) {
            status = Status::Line;
            --cp;
          }
          else {
            return {false};
          }
        }
        break;

      case Status::Line:
        if ( *cp == end ) {
          int tmp;
          if ( NOT( TtUtility::StringToInteger( line.c_str(), &tmp ) ) ) {
            return {false};
          }
          return {true, path, tmp};
        }
        else if ( NOT( isdigit( *cp ) ) ) {
          return {false};
        }
        else {
          line.append( cp, 1 );
        }
        break;

      default:
        return {false};
      }
    }
    return {false};
  };
  auto result = tmp( '(', ')' );
  if ( NOT( result.success ) ) {
    return tmp( ':', ':' );
  }
  return result;
}


bool
OutputEdit::CreatedInternal( void )
{
  this->SetFont( OutputEdit::DEFAUT_FONT );
  this->SetTextLimit( 1024 * 1024 * 2 );

  this->OverrideWindowProcedureByTTL();

  this->RegisterSingleHandler( WM_LBUTTONDBLCLK, [this] ( WPARAM, LPARAM l_param ) -> WMResult {
    if ( file_path_and_line_handler_ ) {
      int result = this->SendMessage( EM_CHARFROMPOS, 0, l_param );
      int char_index = LOWORD( result );
      int line_index = HIWORD( result );
      int length = this->SendMessage( EM_LINELENGTH, char_index );
      TtString::UniqueString buf( length );
      *(reinterpret_cast<WORD*>( buf.GetPointer() )) = static_cast<WORD>( length );
      this->SendMessage( EM_GETLINE, line_index, reinterpret_cast<LPARAM>( buf.GetPointer() ) );
      (buf.GetPointer())[length] = '\0';
      auto parse_result = parse_line_as_file_path_and_line( TtString::Strip( buf.ToString() ) );
      if ( parse_result.success ) {
        file_path_and_line_handler_( parse_result.path, parse_result.line );
      }
    }
    return {WMResult::Done};
  }, false );

  return true;
}


void
OutputEdit::SetFilePathAndLineHandler( FilePathAndLineHandler handler )
{
  file_path_and_line_handler_ = handler;
}


void
OutputEdit::Clear( void )
{
  this->SetText( "" );
}

void
OutputEdit::AddText( const std::string& data )
{
  int tmp = this->GetTextLength();
  this->SendMessage( EM_SETSEL, tmp, tmp );
  this->SendMessage( EM_REPLACESEL, 0, reinterpret_cast<WPARAM>( data.c_str() ) );
}

ProcessManager&
OutputEdit::GetProcessManager( void )
{
  return process_manager_;
}


// -- OutputEditPanel ----------------------------------------------------
OutputEditPanel::OutputEditPanel( void ) :
output_edit_(),
background_brush_( static_cast<HBRUSH>( ::GetStockObject( WHITE_BRUSH ) ) )
{
}

bool
OutputEditPanel::CreatedInternal( void )
{
  output_edit_.Create( {this} );
  this->RegisterWMSize( [this] (int, int width, int height ) -> WMResult {
    output_edit_.SetPositionSize( 1, 1, width - 2, height - 2 );
    return {WMResult::Done};
  } );
  this->RegisterSingleHandler( WM_CTLCOLORSTATIC, [this] ( WPARAM w_param, LPARAM l_param ) -> WMResult {
    if ( reinterpret_cast<HWND>( l_param ) == output_edit_.GetHandle() ) {
      ::SetBkColor( reinterpret_cast<HDC>( w_param ), RGB( 255, 255, 255 ) );
      return {WMResult::Done, reinterpret_cast<LRESULT>( background_brush_ )};
    }
    return {WMResult::Incomplete};
  }, true );
  output_edit_.Show();

  return this->TtPanel::CreatedInternal();
}


OutputEdit&
OutputEditPanel::GetOutputEdit( void )
{
  return output_edit_;
}


// -- MainTab ------------------------------------------------------------
MainTab::MainTab( void )
{
}


bool
MainTab::CreatedInternal( void )
{
  this->TtTab::CreatedInternal();

  this->Append<OutputEditPanel>( "コンパイル結果" );
  this->Append<OutputEditPanel>( "実行ファイル出力" );
  this->Append<OutputEditPanel>( "外部プログラム出力" );
  return true;
}


OutputEdit&
MainTab::GetCompilerEdit( void )
{
  return dynamic_cast<OutputEditPanel&>( this->GetPanelAt( MainTab::Compiler ) ).GetOutputEdit();
}

OutputEdit&
MainTab::GetExecuteEdit( void )
{
  return dynamic_cast<OutputEditPanel&>( this->GetPanelAt( MainTab::Execute ) ).GetOutputEdit();
}

OutputEdit&
MainTab::GetExternalProgramEdit( void )
{
  return dynamic_cast<OutputEditPanel&>( this->GetPanelAt( MainTab::ExternalProgram ) ).GetOutputEdit();
}


// -- MainToolBar --------------------------------------------------------
MainToolBar::MainToolBar( void )
{
}

bool
MainToolBar::CreatedInternal( void )
{
  this->TtToolBar::CreatedInternal();

  this->SetExtendedStyle( TtToolBar::ExtendedStyle::DrawDropDwonArrows | TtToolBar::ExtendedStyle::MixedButtons );
  this->SetImageList( *Image::LIST );

  namespace ID = CommandID::MainToolBar;
  using Index = Image::Index;

  auto add_standard_button = [this] ( int command_id, int image_index, const std::string& str ) {
    this->AddButtonWithString( command_id, image_index + Image::OffsetOfList, str );
  };
  auto add_drop_down_button = [this] ( int command_id, int image_index, const std::string& str, bool is_whole ) {
    this->AddButtonWithString( command_id, image_index + Image::OffsetOfList, str,
                               TtToolBar::Button::Style::Standard | (is_whole ? TtToolBar::Button::Style::WholeDropDown : TtToolBar::Button::Style::DropDown ) );
  };

  add_standard_button( ID::NewProject,  Index::NewProject,  "新規プロジェクト" );
  add_standard_button( ID::OpenProject, Index::OpenProject, "プロジェクトを開く" );
  add_standard_button( ID::SaveProject, Index::SaveProject, "プロジェクトを保存する" );
  this->AddSeparator();
  add_standard_button( ID::ProjectSettings, Index::ProjectSettings, "プロジェクト設定" );

  {
    const int separator_width = 128;

    // For ComboBox
    this->AddSeparator( ID::SelectStructure );
    // セパレータに対しては幅変更になる
    this->GetButton( ID::SelectStructure ).SetBmpImageIndex( separator_width );

    structure_combo_box_.Create( {this, CommandID::ID::SelectStructure} );
    structure_combo_box_.SetPositionSize( 110, 2, separator_width - 8, 120 );
    structure_combo_box_.Show();
  }

  this->AddSeparator();
  add_standard_button( ID::Compile, Index::Compile, "コンパイル" );
  add_standard_button( ID::Build,   Index::Build,   "メイク" );
  add_standard_button( ID::Rebuild, Index::Rebuild, "再構築" );
  add_standard_button( ID::Clean,   Index::Clean,   "クリーン" );
  this->AddSeparator();
  add_standard_button( ID::Execute, Index::Execute, "実行" );
  this->AddSeparator();
  add_drop_down_button( ID::ExternalProgram, Index::ExternalProgram, "外部プログラム", true );
  add_standard_button( ID::StopExternalProgram, Index::StopExternalProgram, "外部プログラムを終了させる" );
  return true;
}

void
MainToolBar::SetBeginningMode( void )
{
  this->GetButton( CommandID::MainToolBar::SaveProject         ).Disable();
  this->GetButton( CommandID::MainToolBar::ProjectSettings     ).Disable();
  this->GetButton( CommandID::MainToolBar::Compile             ).Disable();
  this->GetButton( CommandID::MainToolBar::Build               ).Disable();
  this->GetButton( CommandID::MainToolBar::Rebuild             ).Disable();
  this->GetButton( CommandID::MainToolBar::Clean               ).Disable();
  this->GetButton( CommandID::MainToolBar::Execute             ).Disable();
  this->GetButton( CommandID::MainToolBar::ExternalProgram     ).Disable();
  this->GetButton( CommandID::MainToolBar::StopExternalProgram ).Disable();
}

void
MainToolBar::SetNormalMode( void )
{
  this->GetButton( CommandID::MainToolBar::SaveProject         ).Enable();
  this->GetButton( CommandID::MainToolBar::ProjectSettings     ).Enable();
  this->GetButton( CommandID::MainToolBar::Compile             ).Enable();
  this->GetButton( CommandID::MainToolBar::Build               ).Enable();
  this->GetButton( CommandID::MainToolBar::Rebuild             ).Enable();
  this->GetButton( CommandID::MainToolBar::Clean               ).Enable();
  this->GetButton( CommandID::MainToolBar::Execute             ).Enable();
  this->GetButton( CommandID::MainToolBar::ExternalProgram     ).Enable();
  this->GetButton( CommandID::MainToolBar::StopExternalProgram ).Disable();
}


void
MainToolBar::SetBuildingMode( void )
{
  this->GetButton( CommandID::MainToolBar::Compile ).Disable();
  this->GetButton( CommandID::MainToolBar::Build   ).Disable();
  this->GetButton( CommandID::MainToolBar::Rebuild ).Disable();
  this->GetButton( CommandID::MainToolBar::Clean   ).Disable();
}

void
MainToolBar::UnsetBuildingMode( void )
{
  this->GetButton( CommandID::MainToolBar::Compile ).Enable();
  this->GetButton( CommandID::MainToolBar::Build   ).Enable();
  this->GetButton( CommandID::MainToolBar::Rebuild ).Enable();
  this->GetButton( CommandID::MainToolBar::Clean ).Enable();
}


// -- TreeToolBar --------------------------------------------------------
TreeToolBar::TreeToolBar( void )
{
}


bool
TreeToolBar::CreatedInternal( void )
{
  this->TtToolBar::CreatedInternal();

  this->SetExtendedStyle( TtToolBar::ExtendedStyle::DrawDropDwonArrows | TtToolBar::ExtendedStyle::MixedButtons );
  this->SetImageList( *Image::LIST );

  namespace ID = CommandID::TreeToolBar;
  using Index = Image::Index;

  auto add_standard_button = [this] ( int command_id, int image_index, const std::string& str ) {
    this->AddButtonWithString( command_id, image_index + Image::OffsetOfList, str );
  };

  add_standard_button( ID::AddNewFile,      Index::AddNewFile,      "プロジェクトに新規ファイルを追加" );
  add_standard_button( ID::AddExistingFile, Index::AddExistingFile, "プロジェクトに既存ファイルを追加" );
  add_standard_button( ID::AddDirectory,    Index::AddDirectory,    "管理ディレクトリの追加" );
  add_standard_button( ID::DeleteElement,   Index::DeleteElement,   "プロジェクトから削除" );
  add_standard_button( ID::DownElement,     Index::DownElement,     "下へ移動" );
  add_standard_button( ID::UpElement,       Index::UpElement,       "上へ移動" );
  add_standard_button( ID::RenameElement,   Index::RenameElement,   "フォルダ名の編集" );
  return true;
}


void
TreeToolBar::SetBeginningMode( void )
{
  this->GetButton( CommandID::TreeToolBar::AddNewFile      ).Disable();
  this->GetButton( CommandID::TreeToolBar::AddExistingFile ).Disable();
  this->GetButton( CommandID::TreeToolBar::AddDirectory    ).Disable();
  this->GetButton( CommandID::TreeToolBar::DeleteElement   ).Disable();
  this->GetButton( CommandID::TreeToolBar::DownElement     ).Disable();
  this->GetButton( CommandID::TreeToolBar::UpElement       ).Disable();
  this->GetButton( CommandID::TreeToolBar::RenameElement   ).Disable();
}

void
TreeToolBar::SetNormalMode( void )
{
  this->GetButton( CommandID::TreeToolBar::AddNewFile      ).Enable();
  this->GetButton( CommandID::TreeToolBar::AddExistingFile ).Enable();
  this->GetButton( CommandID::TreeToolBar::AddDirectory    ).Enable();
  this->GetButton( CommandID::TreeToolBar::DeleteElement   ).Enable();
  this->GetButton( CommandID::TreeToolBar::DownElement     ).Enable();
  this->GetButton( CommandID::TreeToolBar::UpElement       ).Enable();
  this->GetButton( CommandID::TreeToolBar::RenameElement   ).Enable();
}


void
TreeToolBar::TreeViewItemSelected( MainTree::Item& item )
{
  auto proc = [this] ( int command_id, bool flag ) {
    if ( flag ) {
      this->GetButton( command_id ).Enable();
    }
    else {
      this->GetButton( command_id ).Disable();
    }
  };

  if ( item.GetParameter()->TypeIs<Project::ProjectElement>() ) {
    proc( CommandID::TreeToolBar::DeleteElement, false );
    proc( CommandID::TreeToolBar::DownElement,   false );
    proc( CommandID::TreeToolBar::UpElement,     false );
    proc( CommandID::TreeToolBar::RenameElement, false );
  }
  else if ( item.GetParameter()->TypeIs<Project::Directory>() ) {
    proc( CommandID::TreeToolBar::DeleteElement, NOT( item.HasChild() ) );
    proc( CommandID::TreeToolBar::DownElement,   true );
    proc( CommandID::TreeToolBar::UpElement,     true );
    proc( CommandID::TreeToolBar::RenameElement, true );
  }
  else {
    proc( CommandID::TreeToolBar::DeleteElement, true );
    proc( CommandID::TreeToolBar::DownElement,   true );
    proc( CommandID::TreeToolBar::UpElement,     true );
    proc( CommandID::TreeToolBar::RenameElement, false );
  }
}

// -- TreePopupMenu ------------------------------------------------------
TreePopupMenu::TreePopupMenu( void ) :
TtSubMenuCommand( TtSubMenuCommand::Create() ),
external_program_menu_( TtSubMenuCommand::Create() )
{
  auto add_menu = [this] ( int command_id, Image::Index::Value image_index, const std::string& str ) {
    this->AppendNewItem( command_id, str );
    if ( image_index != Image::Index::None ) {
      this->GetLastItem().SetBmpImage( Image::BMPS[image_index] );
    }
  };
  using Index = Image::Index;

  add_menu( CommandID::OpenFile,      Index::None,          "ファイルを開く(&O)" );
  add_menu( CommandID::UpElement,     Index::UpElement,     "上に移動(&U)" );
  add_menu( CommandID::DownElement,   Index::DownElement,   "下に移動(&D)" );
  add_menu( CommandID::RenameElement, Index::RenameElement, "フォルダ名の編集(&R)" );
  this->AppendSeparator();
  add_menu( CommandID::AddNewFile,      Index::AddNewFile,      "プロジェクトに新規ファイルを追加(&N)..." );
  add_menu( CommandID::AddExistingFile, Index::AddExistingFile, "プロジェクトに既存ファイルを追加(&E)..." );
  add_menu( CommandID::DeleteElement,   Index::DeleteElement,   "プロジェクトから削除(&D)" );
  add_menu( CommandID::AddDirectory,    Index::AddDirectory,    "管理ディレクトリの追加(&F)" );
  this->AppendSeparator();
  this->AppendMenu( external_program_menu_, "外部プログラムで開く(&P)" );
  this->GetLastItem().SetBmpImage( Image::BMPS[Index::ExternalProgram] );
}


void
TreePopupMenu::SetBeginningMode( void )
{
  int count = this->GetItemCount();
  for ( int i = 0; i < count; ++i ) {
    this->GetItemAt( i ).SetEnabled( false );
  }
}

void
TreePopupMenu::SetNormalMode( void )
{
  int count = this->GetItemCount();
  for ( int i = 0; i < count; ++i ) {
    this->GetItemAt( i ).SetEnabled( true );
  }
}


void
TreePopupMenu::TreeViewItemSelected( MainTree::Item& item )
{
  const int OpenFile        = 0;
  const int UpElement       = 1;
  const int DownElement     = 2;
  const int RenameElement   = 3;
  const int DeleteElement   = 7;
  const int ExternalElement = 10;

  if ( item.GetParameter()->TypeIs<Project::ProjectElement>() ) {
    this->GetItemAt( OpenFile        ).SetEnabled( false );
    this->GetItemAt( UpElement       ).SetEnabled( false );
    this->GetItemAt( DownElement     ).SetEnabled( false );
    this->GetItemAt( RenameElement   ).SetEnabled( false );
    this->GetItemAt( DeleteElement   ).SetEnabled( false );
    this->GetItemAt( ExternalElement ).SetEnabled( false );
  }
  else if ( item.GetParameter()->TypeIs<Project::Directory>() ) {
    this->GetItemAt( OpenFile        ).SetEnabled( false );
    this->GetItemAt( UpElement       ).SetEnabled( true );
    this->GetItemAt( DownElement     ).SetEnabled( true );
    this->GetItemAt( RenameElement   ).SetEnabled( true );
    this->GetItemAt( DeleteElement   ).SetEnabled( NOT( item.HasChild() ) );
    this->GetItemAt( ExternalElement ).SetEnabled( false );
  }
  else {
    this->GetItemAt( OpenFile        ).SetEnabled( true );
    this->GetItemAt( UpElement       ).SetEnabled( true );
    this->GetItemAt( DownElement     ).SetEnabled( true );
    this->GetItemAt( RenameElement   ).SetEnabled( false );
    this->GetItemAt( DeleteElement   ).SetEnabled( true );
    this->GetItemAt( ExternalElement ).SetEnabled( true );
  }
}

void
TreePopupMenu::SetExternalProgramMenu( TtSubMenuCommand& menu )
{
  const int ExternalElement = 10;
  this->InsertMenu( ExternalElement, menu, this->GetItemAt( ExternalElement ).GetText() );
  this->DeleteAt( ExternalElement + 1 );
  this->GetItemAt( ExternalElement ).SetBmpImage( Image::BMPS[Image::Index::ExternalProgram] );
  external_program_menu_ = menu;
}
