// utility_dialogs.cpp

#pragma warning(push)
#  pragma warning(disable : 4005)
#  pragma warning(disable : 4917)
#  include <Shlobj.h>
#pragma warning(pop)
#pragma comment(lib, "Shell32.lib")

#include "ttl_define.h"
#include "tt_file_dialog.h"
#include "tt_folder_browser_dialog.h"
#include "tt_message_box.h"
#include "tt_path.h"
#include "tt_clipboard.h"

#include "common.h"
#include "utility.h"
#include "utility_dialogs.h"

using namespace TTVCDeveloper;


// -- FileInputDialog ----------------------------------------------------
FileInputDialog::FileInputDialog( const std::string& default_path ) :
path_( default_path )
{
}

DWORD
FileInputDialog::GetStyle( void )
{
  return WS_DLGFRAME | WS_CAPTION | WS_SIZEBOX;
}

DWORD
FileInputDialog::GetExtendedStyle( void )
{
  return WS_EX_TOOLWINDOW;
}


bool
FileInputDialog::Created( void )
{
  this->SetText( "ファイルの追加" );

  struct CommandID {
    enum ID : int {
      Reference = 10001,
      Ok,
      Cancel,
    };
  };

  label_.Create( {this} );
  path_edit_.Create( {this} );
  ref_button_.Create( {this, CommandID::Reference} );
  ok_button_.Create( {this, CommandID::Ok} );
  cancel_button_.Create( {this, CommandID::Cancel} );

  this->RegisterWMSize( [this] ( int, int w, int h ) -> WMResult {
    label_.SetPositionSize(               8,      12,      88,  16 );
    path_edit_.SetPositionSize(         108,       8, w - 176,  20 );
    ref_button_.SetPositionSize(    w -  60,       8,      52,  20 );
    ok_button_.SetPositionSize(     w - 196,  h - 26,      88,  22 );
    cancel_button_.SetPositionSize( w -  96,  h - 26,      88,  22 );
    return {WMResult::Done};
  } );
  this->SetClientSize( 506, 70, false );
  this->RegisterWMSizing( this->MakeCanChangeOnlyHorizontalHandler(), false );

  label_.SetText( "ファイルパス(&F):" );
  ref_button_.SetText( "参照..." );
  ok_button_.SetText( "OK" );
  cancel_button_.SetText( "キャンセル" );

  path_edit_.SetText( path_ );

  this->AddCommandHandler( CommandID::Reference, [this] ( int, HWND ) -> WMResult {
    TtSaveFileDialog dialog;
    Utility::SetSourceFileFilters( dialog );
    std::string path = path_edit_.GetText();
    if ( TtPath::IsDirectory( path ) ) {
      dialog.SetInitialDirectory( path );
    }
    else {
      dialog.SetFileName( path );
    }

    if ( dialog.ShowDialog( *this ) ) {
      path_edit_.SetText( dialog.GetFileName() );
    }
    return {WMResult::Done};
  } );

  this->AddCommandHandler( CommandID::Ok, [this] ( int, HWND ) -> WMResult {
    if ( TtPath::IsDirectory( path_edit_.GetText() ) ) {
      TtMessageBoxOk::Show( *this, "そのパスはディレクトリです。", "エラー", TtMessageBox::Icon::ERROR );
    }
    else {
      path_ = path_edit_.GetText();
      this->EndDialog( 1 );
    }
    return {WMResult::Done};
  } );

  this->AddCommandHandler( CommandID::Cancel, [this] ( int, HWND ) -> WMResult {
    this->EndDialog( 0 );
    return {WMResult::Done};
  } );


  label_.Show();
  path_edit_.Show();
  ref_button_.Show();
  ok_button_.Show();
  cancel_button_.Show();

  cancel_button_.SetFocus();
  this->SetCenterRelativeToParent();

  return true;
}

const std::string&
FileInputDialog::GetPath( void )
{
  return path_;
}


// -- NewProjectDialog ---------------------------------------------------
NewProjectDialog::NewProjectDialog( void )
{
}

DWORD
NewProjectDialog::GetStyle( void )
{
  return WS_DLGFRAME | WS_CAPTION | WS_SIZEBOX;
}

DWORD
NewProjectDialog::GetExtendedStyle( void )
{
  return WS_EX_TOOLWINDOW;
}


bool
NewProjectDialog::Created( void )
{
  this->SetText( "新規プロジェクト" );

  struct CommandID {
    enum ID : int {
      FolderReference = 10001,
      Ok,
      Cancel,
    };
  };

  file_label_.Create( {this} );
  file_edit_.Create( {this} );
  folder_label_.Create( {this} );
  folder_edit_.Create( {this} );
  folder_ref_button_.Create( {this, CommandID::FolderReference} );
  ok_button_.Create( {this, CommandID::Ok} );
  cancel_button_.Create( {this, CommandID::Cancel} );

  this->RegisterWMSize( [this] ( int, int w, int h ) -> WMResult {
    file_label_.SetPositionSize(              8,      16,      280,  16 );
    file_edit_.SetPositionSize(             150,      12,  w - 158,  20 );
    folder_label_.SetPositionSize(            8,      54,      280,  16 );
    folder_edit_.SetPositionSize(           150,      50,  w - 220,  20 );
    folder_ref_button_.SetPositionSize( w -  60,      50,       52,  20 );
    ok_button_.SetPositionSize(         w - 196,  h - 26,       88,  22 );
    cancel_button_.SetPositionSize(     w -  96,  h - 26,       88,  22 );
    return {WMResult::Done};
  } );
  this->SetClientSize( 546, 120, false );
  this->RegisterWMSizing( this->MakeCanChangeOnlyHorizontalHandler(), false );

  file_label_.SetText( "プロジェクトのファイル名" );
  file_edit_.SetText( "project.tdp" );
  folder_label_.SetText( "プロジェクトフォルダ" );
  folder_ref_button_.SetText( "参照..." );
  ok_button_.SetText( "OK" );
  cancel_button_.SetText( "キャンセル" );

  this->AddCommandHandler( CommandID::FolderReference, [this] ( int, HWND ) -> WMResult {
    TtFolderBrowserDialog dialog;
    dialog.SetDescription( "プロジェクトフォルダを指定してください。" );
    if ( dialog.ShowDialog( *this ) ) {
      folder_edit_.SetText( dialog.GetSelectedPath() );
    }
    return {WMResult::Done};
  } );

  this->AddCommandHandler( CommandID::Ok, [this] ( int, HWND ) -> WMResult {
    path_ = folder_edit_.GetText() + "\\" + file_edit_.GetText();
    if ( NOT( TtString::EndWith( path_, ".tdp" ) ) ) {
      path_ = path_ + ".tdp";
    }
    if ( Utility::MakeDirectoryOfFileIfNotExist( path_ ) ) {
      this->EndDialog( 1 );
    }
    else {
      TtMessageBoxOk box;
      box.SetMessage( "プロジェクト用ディレクトリを作成出来ませんでした。\r\nファイル : " + path_  );
      box.SetCaption( "エラー" );
      box.SetIcon( TtMessageBox::Icon::ERROR );
      box.ShowDialog( *this );
    }
    return {WMResult::Done};
  } );

  this->AddCommandHandler( CommandID::Cancel, [this] ( int, HWND ) -> WMResult {
    this->EndDialog( 0 );
    return {WMResult::Done};
  } );


  file_label_.Show();
  file_edit_.Show();
  folder_label_.Show();
  folder_edit_.Show();
  folder_ref_button_.Show();
  ok_button_.Show();
  cancel_button_.Show();

  cancel_button_.SetFocus();
  this->SetCenterRelativeToParent();
  return true;
}

const std::string&
NewProjectDialog::GetPath( void )
{
  return path_;
}


// -- UserInputDialog ----------------------------------------------------
UserInputDialog::UserInputDialog( const std::string& title ) :
title_( title ),
input_()
{
}

DWORD
UserInputDialog::GetStyle( void )
{
  return WS_DLGFRAME | WS_CAPTION | WS_SIZEBOX;
}

DWORD
UserInputDialog::GetExtendedStyle( void )
{
  return WS_EX_TOOLWINDOW;
}

bool
UserInputDialog::Created( void )
{
  this->SetText( title_ );

  struct CommandID {
    enum ID : int {
      Edit = 10001,
      Ok,
    };
  };
  edit_.Create( {this, CommandID::Edit} );
  ok_button_.Create( {this, CommandID::Ok} );

  this->RegisterWMSize( [this] ( int, int w, int ) -> WMResult {
    edit_.SetPositionSize(           4, 4, w - 4 - 48, 20 );
    ok_button_.SetPositionSize( w - 44, 4,         40, 20 );
    return {WMResult::Done};
  } );
  this->SetClientSize( 392, 26, false );
  this->RegisterWMSizing( this->MakeCanChangeOnlyHorizontalHandler(), false );

  ok_button_.SetText( "OK" );

  edit_.SetFocus();

  this->AddCommandHandler( CommandID::Ok, [this] ( int, HWND ) -> WMResult {
    input_ = edit_.GetText();
    this->EndDialog( 0 );
    return {WMResult::Done};
  } );

  edit_.Show();
  ok_button_.Show();

  return true;
}


const std::string&
UserInputDialog::GetTitle( void )
{
  return title_;
}

void
UserInputDialog::SetTitle( const std::string& title )
{
  title_ = title;
}


const std::string&
UserInputDialog::GetInput( void )
{
  return input_;
}


// -- ShortcutKeyDialog --------------------------------------------------
ShortcutKeyDialog::ShortcutKeyDialog( const std::string& title ) :
title_( title )
{
}

DWORD
ShortcutKeyDialog::GetExtendedStyle( void )
{
  return WS_EX_TOOLWINDOW;
}

bool
ShortcutKeyDialog::Created( void )
{
  this->SetText( title_ );

  struct CommandID {
    enum ID : int {
      Edit = 10001,
      Clear,
      Ok,
    };
  };
  edit_.Create( {this, CommandID::Edit} );
  clear_button_.Create( {this, CommandID::Clear} );
  ok_button_.Create( {this, CommandID::Ok} );

  this->RegisterWMSize( [this] ( int, int w, int h ) -> WMResult {
    edit_.SetPositionSize(               4,      4, w - 36,  20 );
    clear_button_.SetPositionSize(  w - 26,      4,     24,  20 );
    ok_button_.SetPositionSize(     w - 44, h - 24,     40,  20 );
    return {WMResult::Done};
  } );
  this->SetClientSize( 174, 58, false );

  clear_button_.SetText( "消" );
  ok_button_.SetText( "OK" );

  edit_.SetFocus();
  edit_.SetText( key_.GetText() );

  edit_.OverrideWindowProcedureByTTL();
  auto key_down_handler = [this] ( WPARAM w_param, LPARAM ) -> WMResult {
    if ( w_param == VK_SHIFT || w_param == VK_CONTROL || w_param == VK_MENU ) {
      key_ = TtForm::AcceleratorMap::ShortcutKey();
    }
    else {
      key_ = TtForm::AcceleratorMap::ShortcutKey::GetMdifierFromKeyboard( static_cast<WORD>( w_param ) );
    }
    edit_.SetText( key_.GetText() );
    return {WMResult::Done};
  };
  edit_.RegisterSingleHandler( WM_KEYDOWN, key_down_handler, false );
  edit_.RegisterSingleHandler( WM_SYSKEYDOWN, key_down_handler, false );
  this->AddCommandHandler( CommandID::Clear, [this] ( int, HWND ) -> WMResult {
    key_ = TtForm::AcceleratorMap::ShortcutKey();
    edit_.SetText( key_.GetText() );
    return {WMResult::Done};
  } );
  this->AddCommandHandler( CommandID::Ok, [this] ( int, HWND ) -> WMResult {
    if ( key_.GetText().empty() ) {
      key_ = TtForm::AcceleratorMap::ShortcutKey();
    }
    this->EndDialog( 1 );
    return {WMResult::Done};
  } );

  edit_.Show();
  clear_button_.Show();
  ok_button_.Show();

  return true;
}

void
ShortcutKeyDialog::SetKey( TtForm::AcceleratorMap::ShortcutKey key )
{
  key_ = key;
}

TtForm::AcceleratorMap::ShortcutKey
ShortcutKeyDialog::GetKey( void )
{
  return key_;
}

// -- ProjectVariablesDialog ---------------------------------------------
ProjectVariablesDialog::ProjectVariablesDialog( void ) :
show_file_variable_( false ),
menu_( TtSubMenu::Create() )
{
}

void
ProjectVariablesDialog::SetShowFileVariable( bool flag )
{
  show_file_variable_ = flag;
}


DWORD
ProjectVariablesDialog::GetStyle( void )
{
  return WS_SYSMENU | WS_CAPTION | WS_SIZEBOX;
}

DWORD
ProjectVariablesDialog::GetExtendedStyle( void )
{
  return WS_EX_TOOLWINDOW;
}

bool
ProjectVariablesDialog::Created( void )
{
  struct CommandID {
    enum ID : int {
      List = 10001,
      MenuCopy,
    };
  };

  this->SetText( "使用可能変数一覧" );

  list_.Create( {this, CommandID::List} );

  this->RegisterWMSize( [this] ( int, int w, int h ) -> WMResult {
    list_.SetPositionSize( 2, 2, w - 4, h - 4 );
    return {WMResult::Done};
  } );
  this->SetClientSize( 592, 200, false );

  menu_.AppendNewItem( CommandID::MenuCopy, "変数名をコピー(&C)" );
  this->AddNotifyHandler( CommandID::List, [this] ( NMHDR* nmhdr ) -> WMResult {
    // アイテム右クリック
    if ( nmhdr->code == NM_RCLICK ) {
      POINT cursor_point = TtWindow::GetCursorPosition();
      TtListViewItem hit_item = list_.HitTest( cursor_point.x - list_.GetPoint().x, cursor_point.y - list_.GetPoint().y );
      if ( hit_item.IsValid() ) {
        hit_item.SetSelected( true );
      }
      TtListViewItem item = list_.GetSelected();
      if ( item.IsValid() ) {
        menu_.PopupAbsolute( *this, cursor_point.x, cursor_point.y );
      }
    }
    return {WMResult::Done};
  } );
  this->AddCommandHandler( CommandID::MenuCopy, [this] ( int, HWND ) -> WMResult {
    TtListViewItem item = list_.GetSelected();
    if ( item.IsValid() ) {
      TtClipboard::SetString( item.GetSubItemText( 0 ) );
    }
    return {WMResult::Done};
  } );

  this->RegisterWMClose( [this] ( void ) -> WMResult {
    this->Hide();
    // 閉じない
    return {WMResult::Done};
  }, false );

  // -- 文字列変換ヘルプリスト
  {
    list_.SetFullRowSelect( true );
    TtListViewColumn column_name = list_.MakeNewColumn();
    column_name.SetText( "変数名" );
    column_name.SetWidth( 150 );
    TtListViewColumn column_description = list_.MakeNewColumn();
    column_description.SetText( "説明" );
    column_description.SetWidth( 360 );

    auto add = [this] ( const std::string& var, const std::string& description ) {
      auto item = list_.MakeNewItem();
      item.SetSubItemText( 0, var );
      item.SetSubItemText( 1, description );
    };
    add( "@@user_input@@", "実行時にダイアログが一度だけ開き入力された値に置換されます。" );
    if ( show_file_variable_ ) {
      add( "@@file@@", "タイプが個別ファイルオープン時のみ有効。指定されたファイルパス。" );
    }
    add( "@@project_name@@", "プロジェクト名" );
    add( "@@project_folder_path@@", "プロジェクトフォルダのパス" );
    add( "@@project_folder_name@@", "プロジェクトフォルダの名前" );
    add( "@@target_path@@", "出力ファイルのパス" );
    add( "@@output_folder_path@@", "出力フォルダのパス" );
    add( "@@output_folder_name@@", "出力フォルダの名前" );
    add( "@@all_files@@", "プロジェクトに登録されている全ファイルの羅列" );
    add( "@@header_files@@", "プロジェクトに登録されているヘッダファイルの羅列" );
    add( "@@source_files@@", "プロジェクトに登録されているソースファイルの羅列" );
    add( "@@header_source_files@@", "プロジェクトに登録されているヘッダファイルとソースファイルの羅列" );
    // add( "@@@@", "" );
  }

  list_.Show();
  return {true};
}

// -- StandardInputDialog ------------------------------------------------
StandardInputDialog::StandardInputDialog( const std::string& title ) :
title_( title )
{
}


DWORD
StandardInputDialog::GetStyle( void )
{
  return WS_CAPTION | WS_SIZEBOX;
}

DWORD
StandardInputDialog::GetExtendedStyle( void )
{
  return WS_EX_TOOLWINDOW;
}

bool
StandardInputDialog::Created( void )
{
  struct CommandID {
    enum ID : int {
      LogEdit = 10001,
      InputEdit,
    };
  };

  this->SetText( title_ );

  log_edit_.Create( {this, CommandID::LogEdit} );
  input_edit_.Create( {this, CommandID::InputEdit} );

  log_edit_.SetFont( Font::OUTPUT_MONOSPACED );
  input_edit_.SetFont( Font::OUTPUT_MONOSPACED );

  this->RegisterWMSize( [this] ( int, int w, int h ) -> WMResult {
    log_edit_.SetPositionSize(   2,      2, w - 4, h - 30 );
    input_edit_.SetPositionSize( 2, h - 24, w - 4,     22 );
    return {WMResult::Done};
  } );
  this->SetClientSize( 300, 200, false );

  input_edit_.OverrideWindowProcedureByTTL();
  input_edit_.RegisterSingleHandler( WM_CHAR, [this] ( WPARAM w_param, LPARAM ) -> WMResult {
    if ( w_param == VK_RETURN ) {
      int tmp = log_edit_.GetTextLength();
      log_edit_.SendMessage( EM_SETSEL, tmp, tmp );

      std::string str = input_edit_.GetText();
      log_edit_.SendMessage( EM_REPLACESEL, 0, reinterpret_cast<WPARAM>( (str + "\r\n").c_str() ) );
      str.append( "\n" );

      input_edit_.SetText( "" );
      return {WMResult::Done};
    }
    return {WMResult::Incomplete};
  }, false );

  log_edit_.Show();
  input_edit_.Show();

  return true;
}
