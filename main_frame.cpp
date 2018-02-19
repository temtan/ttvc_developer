// main_frame.cpp

#include <algorithm>

#include "tt_windows_h_include.h"
#pragma warning(push)
#  pragma warning( disable: 4005 )
#  include <shellapi.h>
#pragma warning(pop)
#pragma comment(lib, "shell32.lib")


#include "tt_message_box.h"
#include "tt_path.h"
#include "tt_ini_file.h"
#include "tt_file_dialog.h"
#include "tt_time.h"
#include "tt_utility.h"

#include "exception.h"
#include "project_dialog.h"
#include "utility_dialogs.h"
#include "settings_property_sheet.h"
#include "squirrel_vm.h"
#include "version_dialog.h"
#include "utility.h"

#include "main_frame.h"

using namespace TTVCDeveloper;


// -- MainFrame ----------------------------------------------------------
MainFrame::MainFrame( void ) :
developer_(),

project_changed_( false ),
process_execute_lock_(),

vm_general_( std::nullopt ),
external_program_menu_normal_maker_(),
external_program_menu_open_file_maker_(),
external_program_menu_holder_(),

vm_build_( std::nullopt ),

title_bar_icon_large_( Image::ICONS[Image::Index::Main] ),
title_bar_icon_small_( Image::ICONS[Image::Index::MainSmall] ),

project_history_(),

main_menu_(),
main_tool_bar_(),
main_splitter_panel_( 4, 24, 24 ),

tree_panel_(),
tree_tool_bar_(),
tree_( developer_.GetSettings() ),

tab_(),

tree_popup_menu_(),

editing_item_( nullptr, TtTreeItem::INVALID_HANDLE ),
label_edit_( nullptr ),

drag_handler_(),
tree_data_object_( nullptr ),
tree_drop_source_( nullptr ),
tree_drop_target_( nullptr )
{
  ::OleInitialize( NULL );
  tree_data_object_ = new TreeDataObject();
  tree_drop_source_ = new TreeDropSource();
  tree_drop_target_ = new TreeDropTarget( tree_, drag_handler_ );
}


MainFrame::~MainFrame()
{
  tree_data_object_->Release();
  tree_drop_source_->Release();
  tree_drop_target_->Release();
  ::OleUninitialize();
}


DWORD
MainFrame::GetStyle( void )
{
  return WS_OVERLAPPEDWINDOW;
}

DWORD
MainFrame::GetExtendedStyle( void )
{
  return 0;
}


bool
MainFrame::Created( void )
{
  this->SetIconAsLarge( Image::ICONS[Image::Index::Main] );
  this->SetIconAsSmall( Image::ICONS[Image::Index::MainSmall] );
  this->SetText( APPLICATION_NAME );
  this->SetMenu( main_menu_ );

  // -- コントロール作成 -----
  main_tool_bar_.Create( {this, CommandID::Control::MainToolBar} );
  main_splitter_panel_.Create( {this} );

  tree_panel_.Create( {&main_splitter_panel_} );
  tab_.Create( {&main_splitter_panel_, CommandID::Control::MainTab} );
  tab_.RegisterTabChangeHandlersTo( *this );
  main_splitter_panel_.SetNorthWindow( &tree_panel_ );
  main_splitter_panel_.SetSouthWindow( &tab_ );
  main_splitter_panel_.SetSplitterPosition( IniFileOperation::LoadSplitterPosition() );

  tree_tool_bar_.Create( {&tree_panel_} );
  tree_.Create( {&tree_panel_, CommandID::Control::MainTree} );

  // -- サイズ変更時 -----
  this->RegisterWMSize( [this] ( int, int width, int height ) -> WMResult {
    const int tool_bar_height = 28;
    main_tool_bar_.SetPositionSize(        0,               0,     width, tool_bar_height );
    main_splitter_panel_.SetPositionSize(  1, tool_bar_height, width - 2, height - tool_bar_height - 1 );
    return {WMResult::Done};
  } );
  tree_panel_.RegisterWMSize( [this] ( int, int width, int height ) -> WMResult {
    const int tool_bar_height = 28;
    tree_tool_bar_.SetPositionSize(  0,               0, width, tool_bar_height );
    tree_.SetPositionSize(           0, tool_bar_height, width, height - tool_bar_height );
    return {WMResult::Done};
  } );

  this->RegisterHandlers();

  // -- コントロール表示 -----
  tree_tool_bar_.Show();
  tree_.Show();
  tree_panel_.Show();

  tab_.Show();

  main_tool_bar_.Show();
  main_splitter_panel_.Show();

  return true;
}


void
MainFrame::RegisterHandlers( void )
{
  // -- コマンド ID 対応 -----
  // ファイル
  this->AddCommandHandler( CommandID::Close, [this] ( int, HWND ) -> WMResult {
    this->Close();
    return {WMResult::Done};
  } );

  this->AddCommandHandler( CommandID::NewProject, [this] ( int, HWND ) -> WMResult {
    if ( NOT( this->SaveProjectConfirmation() ) ) {
      return {WMResult::Done};
    }

    NewProjectDialog dialog;
    if ( dialog.ShowDialog( *this ) ) {
      TtMessageBoxYesNo box;
      box.SetMessage( "以下の設定でプロジェクトを新規に作成します。よろしいですか？" );
      box.AppendMessage( "\r\nプロジェクトフォルダ : " + TtPath::DirName( dialog.GetPath() ) );
      box.AppendMessage( "\r\nプロジェクトファイル名 : " + TtPath::BaseName( dialog.GetPath() ) );
      box.SetCaption( "プロジェクト新規作成の確認" );
      box.SetIcon( TtMessageBox::Icon::INFORMATION );
      if ( box.ShowDialog( *this ) == TtMessageBox::Result::YES ) {
        this->ProjectSaveLoadErrorHandling( [this, &dialog] ( void ) {
          developer_.GetProject().path_ = Utility::GetDefaultProjectFilePath();
          developer_.GetProject().LoadFromFile( tree_ );
          developer_.GetProject().path_ = dialog.GetPath();
          developer_.GetProject().SaveToFile( tree_ );
        } );
        this->LoadProjectFile( dialog.GetPath() );
      }
    }
    return {WMResult::Done};
  } );

  this->AddCommandHandler( CommandID::OpenProject, [this] ( int, HWND ) -> WMResult {
    if ( NOT( this->SaveProjectConfirmation() ) ) {
      return {WMResult::Done};
    }
    TtOpenFileDialog dialog;
    dialog.GetFilters().push_back( {"TTVC Developer プロジェクトファイル(*.tdp)", "*.tdp"} );
    dialog.GetFilters().push_back( {"すべてのファイル(*.*)", "*.*"} );
    if ( developer_.GetProject().path_.empty() ) {
      dialog.SetInitialDirectory( TtPath::GetExecutingDirectoryPath() );
    }
    else {
      dialog.SetInitialDirectory( developer_.GetProjectDirectoryPath() );
    }

    if ( dialog.ShowDialog( *this ) ) {
      this->LoadProjectFile( dialog.GetFileName() );
    }
    return {WMResult::Done};
  } );

  this->AddCommandHandler( CommandID::SaveProject, [this] ( int, HWND ) -> WMResult {
    this->ProjectSaveLoadErrorHandling( [this] ( void ) {
      developer_.GetProject().SaveToFile( tree_ );
      project_changed_ = false;
    } );
    return {WMResult::Done};
  } );

  this->AddCommandHandler( CommandID::DeleteProjectHistory, [this] ( int, HWND ) -> WMResult {
    TtMessageBoxYesNo box( "プロジェクトの履歴を消去します。よろしいですか？" );
    box.SetIcon( TtMessageBox::Icon::WARNING );
    box.SetCaption( "消去確認" );
    box.SetDefaultButton( TtMessageBox::DefautButton::SECOND );
    if ( box.ShowDialog( *this ) == TtMessageBox::Result::YES ) {
      project_history_.clear();
      IniFileOperation::SaveProjectHistory( project_history_, developer_.GetSettings().project_history_count_max_ );
      this->SetProjectHistory();
    }
    return {WMResult::Done};
  } );

  // プロジェクト設定
  this->AddCommandHandler( CommandID::ProjectSettings, [this] ( int, HWND ) -> WMResult {
    ProjectDialog dialog( developer_.GetProject(), &developer_.GetCurrentStructure(), false );
    if ( dialog.ShowDialog( *this ) ) {
      developer_.GetProject().path_ = dialog.GetProject().path_;
      developer_.GetProject().name_ = dialog.GetProject().name_;
      developer_.GetProject().use_icon_        = dialog.GetProject().use_icon_;
      developer_.GetProject().icon_file_path_  = dialog.GetProject().icon_file_path_;
      developer_.GetProject().icon_file_index_ = dialog.GetProject().icon_file_index_;
      developer_.GetProject().GetStructures() = dialog.GetProject().GetStructures();
      developer_.SetCurrentStructure( dialog.GetCurrentStructure() );
      this->SetCurrentStructureMenu();
      this->SetTitleBar();
      developer_.GetProject().GetProjectElement().SetName( developer_.GetProject().name_ );
      tree_.SetProject( developer_.GetProject().GetProjectElement() );
      project_changed_ = true;
    }
    return {WMResult::Done};
  } );

  // デフォルトプロジェクト設定
  this->AddCommandHandler( CommandID::DefaultProjectSettings, [this] ( int, HWND ) -> WMResult {
    Project tmp_project;
    tmp_project.path_ = Utility::GetDefaultProjectFilePath();
    tmp_project.LoadFromFileAsDefault();
    ProjectDialog dialog( tmp_project, tmp_project.GetStructures().front().get(), true );
    if ( dialog.ShowDialog( *this ) ) {
      this->ProjectSaveLoadErrorHandling( [&dialog] ( void ) {
        dialog.GetProject().SaveToFileAsDefault();
      } );
    }
    return {WMResult::Done};
  } );

  // 編集
  this->AddCommandHandler( CommandID::OpenFile, [this] ( int, HWND ) -> WMResult {
    MainTree::Item item = tree_.GetSelected();
    if ( item.IsValid() && item.GetParameter()->TypeIs<Project::File>() ) {
      Project::File& file = *(dynamic_cast<Project::File*>( item.GetParameter() ));
      HINSTANCE ret = ::ShellExecute( NULL, NULL, file.GetPath().c_str(), NULL, developer_.GetProjectDirectoryPath().c_str(), SW_SHOWDEFAULT );
      if ( reinterpret_cast<int>( ret ) <= 32 ) {
        TtMessageBoxOk box;
        box.SetMessage( "ファイルを開けませんでした。\r\nファイル : " + file.GetPath() );
        box.SetCaption( "ファイルオープンエラー" );
        box.SetIcon( TtMessageBox::Icon::ERROR );
        box.ShowDialog( *this );
      }
    }
    return {WMResult::Done};
  } );

  this->AddCommandHandler( CommandID::AddNewFile, [this] ( int, HWND ) -> WMResult {
    FileInputDialog dialog( TtPath::DirName( developer_.GetProject().path_ ) );
    if ( dialog.ShowDialog( *this ) ) {
      auto new_path = developer_.GetProject().ConvertRelativePath( dialog.GetPath() );
      if ( NOT( TtPath::IsRelative( new_path ) ) || TtString::StartWith( new_path, "..\\" ) ) {
        TtMessageBoxOk box;
        box.SetMessage( "プロジェクトにはプロジェクトフォルダ配下のファイルのみ追加できます。\r\nファイル : " + dialog.GetPath() );
        box.SetCaption( "ファイル追加エラー" );
        box.SetIcon( TtMessageBox::Icon::ERROR );
        box.ShowDialog( *this );
      }
      else {
        Project::File& tmp = developer_.GetProject().AddFile( TtPath::Canonicalize( new_path ) );
        tree_.AddFile( tmp );
        Utility::TouchFile( developer_.GetProjectDirectoryPath() + "\\" + new_path );
        project_changed_ = true;
      }
    }
    return {WMResult::Done};
  } );

  this->AddCommandHandler( CommandID::AddExistingFile, [this] ( int, HWND ) -> WMResult {
    TtOpenFileDialog dialog;
    dialog.SetInitialDirectory( developer_.GetProjectDirectoryPath() );
    Utility::SetSourceFileFilters( dialog );
    dialog.SetMultiselect( true );
    if ( dialog.ShowDialog( *this ) ) {
      for ( auto& path : dialog.GetFileNames() ) {
        auto new_path = developer_.GetProject().ConvertRelativePath( path );
        if ( NOT( TtPath::IsRelative( new_path ) ) || TtString::StartWith( new_path, "..\\" ) ) {
          TtMessageBoxOk box;
          box.SetMessage( "プロジェクトにはプロジェクトフォルダ配下のファイルのみ追加できます。\r\nファイル : " + path );
          box.SetCaption( "ファイル追加エラー" );
          box.SetIcon( TtMessageBox::Icon::ERROR );
          box.ShowDialog( *this );
        }
        else {
          Project::File& tmp = developer_.GetProject().AddFile( TtPath::Canonicalize( new_path ) );
          tree_.AddFile( tmp );
          project_changed_ = true;
        }
      }
    }
    return {WMResult::Done};
  } );

  this->AddCommandHandler( CommandID::AddDirectory, [this] ( int, HWND ) -> WMResult {
    Project::Directory& tmp = developer_.GetProject().AddDirectory( "名称未設定" );
    tree_.AddDirectory( tmp );
    project_changed_ = true;
    return {WMResult::Done};
  } );

  this->AddCommandHandler( CommandID::DeleteElement, [this] ( int, HWND ) -> WMResult {
    MainTree::Item item = tree_.GetSelected();
    if ( item.IsValid() && item != tree_.GetRoot() && NOT( item.HasChild() ) ) {
      TtMessageBoxYesNo box( item.GetParameter()->GetName() + " をプロジェクトから削除します。よろしいですか？" );
      box.SetIcon( TtMessageBox::Icon::WARNING );
      box.SetCaption( "削除確認" );
      box.SetDefaultButton( TtMessageBox::DefautButton::SECOND );
      if ( box.ShowDialog( *this ) == TtMessageBox::Result::YES ) {
        if ( item.GetParameter()->TypeIs<Project::Directory>() ) {
          auto& dirs = developer_.GetProject().GetDirectories();
          dirs.erase( std::find_if( dirs.begin(), dirs.end(), [&item]  ( std::shared_ptr<Project::Directory>& it ) {
            return static_cast<Project::Element*>( it.get() ) == item.GetParameter();
          } ) );
          item.Remove();
        }
        else {
          auto& files = developer_.GetProject().GetFiles();
          files.erase( std::find_if( files.begin(), files.end(), [&item]  ( std::shared_ptr<Project::File>& it ) {
            return it.get() == item.GetParameter();
          } ) );
          item.Remove();
        }
        project_changed_ = true;
      }
    }
    return {WMResult::Done};
  } );

  this->AddCommandHandler( CommandID::DownElement, [this] ( int, HWND ) -> WMResult {
    if ( editing_item_.IsValid() ) {
      tree_.EndEditMode( true );
    }
    tree_.DownSelected();
    project_changed_ = true;
    return {WMResult::Done};
  } );

  this->AddCommandHandler( CommandID::UpElement, [this] ( int, HWND ) -> WMResult {
    if ( editing_item_.IsValid() ) {
      tree_.EndEditMode( true );
    }
    tree_.UpSelected();
    project_changed_ = true;
    return {WMResult::Done};
  } );

  this->AddCommandHandler( CommandID::RenameElement, [this] ( int, HWND ) -> WMResult {
    if ( editing_item_.IsValid() ) {
      tree_.EndEditMode( true );
    }
    tree_.EditSelected();
    return {WMResult::Done};
  } );

  // ビルド
  {
    // プロジェクト構成の選択
    auto select_structure = [this] ( int index ) {
      developer_.SetCurrentStructureByIndex( index );
      main_menu_.current_structure_menu_.CheckRadioItem( index );
      main_tool_bar_.structure_combo_box_.SetSelect( index );
    };

    this->AddMenuCommandHandler( main_menu_.current_structure_menu_, [select_structure] ( TtMenuItem item ) -> WMResult {
      select_structure( item.GetIndex() );
      return {WMResult::Done};
    } );

    // 親(MainFrame)に委譲する
    main_tool_bar_.RegisterSingleHandler( WM_COMMAND, [select_structure] ( WPARAM, LPARAM ) -> WMResult { return {WMResult::DelegateToParent}; }, false );
    this->AddCommandHandler( CommandID::SelectStructure, [this, select_structure] ( int code, HWND ) -> WMResult {
      if ( code == CBN_SELCHANGE ) {
        select_structure( main_tool_bar_.structure_combo_box_.GetSelectedIndex() );
      }
      return {WMResult::Done};
    } );

    // nmake コマンドを呼び出す TtProcess::CreateInfo を作る
    auto make_info = [this] ( const std::string& target ) {
      TtProcess::CreateInfo info( developer_.GetCurrentCompilerSettings().make_command_path_ );
      info.SetShowState( TtWindow::ShowState::HIDE );
      info.SetCurrentDirectory( developer_.GetProjectDirectoryPath() );
      info.SetArguments( "/f \"" + developer_.GetMakefilePath() + "\" " + target );
      return info;
    };

    // -- コンパイル
    this->AddCommandHandler( CommandID::Compile, [this, make_info] ( int, HWND ) -> WMResult {
      MainTree::Item item = tree_.GetSelected();
      if ( item.IsValid() && item.GetParameter()->TypeIs<Project::SourceFile>() ) {
        this->SquirrelErrorHandling( [this, make_info, &item] ( void ) {
          tab_.GetCompilerEdit().Clear();
          this->InitializeSquirrelVMBuild();

          auto& commands = tab_.GetCompilerEdit().GetProcessManager().GetCommands();
          commands.clear();
          std::string path = item.GetParameterAs<Project::SourceFile*>()->GetObjectFilePath( developer_.GetCurrentStructure().output_directory_name_ );
          commands.push_back( {make_info( "/a " + path )} );
          commands.back().before_ = [this, path] () {
            return this->IfVMIsUseableSquirrelErrorHandlingReturnErrorNotOccurred( [&] () {
              return vm_build_->CallBuildFunctionBeforeCompile( path );
            } );
          };
          commands.back().after_ = [this, path] ( DWORD exit_code ) {
            return this->IfVMIsUseableSquirrelErrorHandlingReturnErrorNotOccurred( [&] () {
              return vm_build_->CallBuildFunctionAfterCompile( path, exit_code );
            } );
          };

          this->Build( *vm_build_ );
        } );
      }
      return {WMResult::Done};
    } );

    // -- ビルド
    this->AddCommandHandler( CommandID::Build, [this, make_info] ( int, HWND ) -> WMResult {
      this->SquirrelErrorHandling( [&] ( void ) {
        tab_.GetCompilerEdit().Clear();
        this->InitializeSquirrelVMBuild();

        auto& commands = tab_.GetCompilerEdit().GetProcessManager().GetCommands();
        commands.clear();
        commands.push_back( {make_info( "build" )} );
        commands.back().before_ = [this] () {
          return this->IfVMIsUseableSquirrelErrorHandlingReturnErrorNotOccurred( [&] () {
            return vm_build_->CallBuildFunctionBeforeBuild();
          } );
        };
        commands.back().after_ = [this] ( DWORD exit_code ) {
          return this->IfVMIsUseableSquirrelErrorHandlingReturnErrorNotOccurred( [&] () {
            return vm_build_->CallBuildFunctionAfterBuild( exit_code );
          } );
        };

        this->Build( *vm_build_ );
      } );
      return {WMResult::Done};
    } );

    // -- 再構築
    this->AddCommandHandler( CommandID::Rebuild, [this, make_info] ( int, HWND ) -> WMResult {
      this->SquirrelErrorHandling( [&] ( void ) {
        tab_.GetCompilerEdit().Clear();
        this->InitializeSquirrelVMBuild();

        auto& commands = tab_.GetCompilerEdit().GetProcessManager().GetCommands();
        commands.clear();
        commands.push_back( {make_info( "clean" )} );
        commands.back().before_ = [this] () {
          return this->IfVMIsUseableSquirrelErrorHandlingReturnErrorNotOccurred( [&] () {
            return vm_build_->CallBuildFunctionBeforeClean();
          } );
        };
        commands.back().after_ = [this] ( DWORD exit_code ) {
          TtUtility::DestructorCall tmp( [this] () { tab_.GetCompilerEdit().AddText( "\n" ); } );
          return this->IfVMIsUseableSquirrelErrorHandlingReturnErrorNotOccurred( [&] () {
            return vm_build_->CallBuildFunctionAfterClean( exit_code );
          } );
        };

        commands.push_back( {make_info( "build" )} );
        commands.back().before_ = [this] () {
          return this->IfVMIsUseableSquirrelErrorHandlingReturnErrorNotOccurred( [&] () {
            return vm_build_->CallBuildFunctionBeforeBuild();
          } );
        };
        commands.back().after_ = [this] ( DWORD exit_code ) {
          return this->IfVMIsUseableSquirrelErrorHandlingReturnErrorNotOccurred( [&] () {
            return vm_build_->CallBuildFunctionAfterBuild( exit_code );
          } );
        };

        this->Build( *vm_build_ );
      } );
      return {WMResult::Done};
    } );

    // -- クリーン
    this->AddCommandHandler( CommandID::Clean, [this, make_info] ( int, HWND ) -> WMResult {
      this->SquirrelErrorHandling( [&] ( void ) {
        tab_.GetCompilerEdit().Clear();
        this->InitializeSquirrelVMBuild();

        auto& commands = tab_.GetCompilerEdit().GetProcessManager().GetCommands();
        commands.clear();
        commands.push_back( {make_info( "clean" )} );
        commands.back().before_ = [this] () {
          return this->IfVMIsUseableSquirrelErrorHandlingReturnErrorNotOccurred( [&] () {
            return vm_build_->CallBuildFunctionBeforeClean();
          } );
        };
        commands.back().after_ = [this] ( DWORD exit_code ) {
          return this->IfVMIsUseableSquirrelErrorHandlingReturnErrorNotOccurred( [&] () {
            return vm_build_->CallBuildFunctionAfterClean( exit_code );
          } );
        };

        this->Build( *vm_build_ );
      } );
      return {WMResult::Done};
    } );
  }

  // 実行
  this->AddCommandHandler( CommandID::Execute, [this] ( int, HWND ) -> WMResult {
    this->Execute();
    return {WMResult::Done};
  } );

  // ツール
  this->AddCommandHandler( CommandID::Settings, [this] ( int, HWND ) -> WMResult {
    SettingsPropertySheet sheet( developer_.GetSettings() );
    sheet.ShowDialog( *this );
    this->SetFromSettings();
    return {WMResult::Done};
  } );

  this->AddCommandHandler( CommandID::OpenProjectFolderByExplorer, [this] ( int, HWND ) -> WMResult {
    HINSTANCE ret = ::ShellExecute( NULL, "explore", developer_.GetProjectDirectoryPath().c_str(), NULL, NULL, SW_SHOWNORMAL );
    if ( reinterpret_cast<int>( ret ) <= 32 ) {
      TtMessageBoxOk box;
      box.SetMessage( "フォルダを開けませんでした。\r\nフォルダ : " + developer_.GetProjectDirectoryPath() );
      box.SetCaption( "フォルダオープンエラー" );
      box.SetIcon( TtMessageBox::Icon::ERROR );
      box.ShowDialog( *this );
    }
    return {WMResult::Done};
  } );
  this->AddCommandHandler( CommandID::OpenProjectFolderByConsole, [this] ( int, HWND ) -> WMResult {
    HINSTANCE ret = ::ShellExecute( NULL, NULL, "cmd.exe", NULL, developer_.GetProjectDirectoryPath().c_str(), SW_SHOWNORMAL );
    if ( reinterpret_cast<int>( ret ) <= 32 ) {
      TtMessageBoxOk box;
      box.SetMessage( "コマンドプロンプトを開けませんでした。\r\n実行ファイル : cmd.exe" );
      box.SetCaption( "コマンドプロンプトオープンエラー" );
      box.SetIcon( TtMessageBox::Icon::ERROR );
      box.ShowDialog( *this );
    }
    return {WMResult::Done};
  } );

  this->AddCommandHandler( CommandID::StopExternalProgram, [this] ( int, HWND ) -> WMResult {
    if ( tab_.GetExternalProgramEdit().GetProcessManager().GetCommands().size() != 0 ) {
      auto& process = tab_.GetExternalProgramEdit().GetProcessManager().GetCommands().front().process_;
      if( process.IsCreated() && NOT( process.HasExited() ) ) {
        process.Terminate( 0 );
      }
    }
    return {WMResult::Done};
  } );

  // ヘルプ
  this->AddCommandHandler( CommandID::VersionInformation, [this] ( int, HWND ) -> WMResult {
    VersionDialog dialog( developer_.GetSettings() );
    dialog.ShowDialog( *this );
    return {WMResult::Done};
  } );

  // -- ツールバー -----
  this->AddNotifyHandler( CommandID::Control::MainToolBar, [this] ( NMHDR* nmhdr ) -> WMResult {
    if ( nmhdr->code == TBN_DROPDOWN ) {
      NMTOOLBAR* nm = reinterpret_cast<NMTOOLBAR*>( nmhdr );
      RECT rect = main_tool_bar_.GetButton( nm->iItem ).GetRectangle();
      POINT point = main_tool_bar_.ConvertToScreenPoint( {rect.left, rect.bottom} );
      switch ( nm->iItem ) {
        // 外部プログラム選択
      case CommandID::ExternalProgram:
        main_menu_.external_program_menu_.PopupAbsolute( *this, point.x, point.y );
        break;

      default:
        ;
      }
      return {WMResult::Done};
    }
    return {WMResult::Incomplete};
  } );

  // -- ツリー操作 -----
  // 名前編集
  this->AddNotifyHandler( CommandID::Control::MainTree, [this] ( NMHDR* nmhdr ) -> WMResult {
    switch ( nmhdr->code ) {
    case TVN_BEGINLABELEDIT: {
      TV_DISPINFO* info = reinterpret_cast<TV_DISPINFO*>( nmhdr );
      editing_item_ = MainTree::Item( &tree_, info->item.hItem );

      label_edit_.emplace( TreeView_GetEditControl( tree_.GetHandle() ) );
      label_edit_->OverrideWindowProcedureByTTL();
      label_edit_->RegisterSingleHandler( WM_KEYUP, [this] ( WPARAM w_param, LPARAM ) -> WMResult {
        if ( w_param == VK_RETURN && editing_item_.IsValid() ) {
          tree_.EndEditMode( false );
        }
        return {WMResult::Incomplete};
      }, false );

      return {WMResult::Done, editing_item_.GetParameter()->TypeIsNot<Project::Directory>()};
    }

    case TVN_ENDLABELEDIT: {
      TV_DISPINFO* info = reinterpret_cast<TV_DISPINFO*>( nmhdr );
      MainTree::Item item( &tree_, info->item.hItem );
      if ( info->item.pszText != nullptr ) {
        if ( item.GetParameter()->TypeIs<Project::Directory>() ) {
          item.SetInfo( info->item );
          dynamic_cast<Project::Directory*>( item.GetParameter() )->SetName( info->item.pszText );
          project_changed_ = true;
        }
      }
      editing_item_ = MainTree::Item( nullptr, TtTreeItem::INVALID_HANDLE );
      label_edit_.reset();
      return {WMResult::Done};
    }
    }
    return {WMResult::Incomplete};
  } );

  // 要素ドラッグ&ドロップ
  this->AddNotifyHandler( CommandID::Control::MainTree, [this] ( NMHDR* nmhdr ) -> WMResult {
    if ( nmhdr->code == TVN_BEGINDRAG ) {
      NMTREEVIEW* nm = reinterpret_cast<NMTREEVIEW*>( nmhdr );
      MainTree::Item item = MainTree::Item( &tree_, nm->itemNew.hItem );
      if ( item.GetParameter()->TypeIs<Project::ProjectElement>() ) {
        return {WMResult::Done};
      }
      if ( item.GetParameter()->TypeIs<Project::File>() ) {
        std::string path = developer_.GetProjectDirectoryPath() + "\\" + item.GetParameterAs<Project::File*>()->GetPath();
        tree_data_object_->SetDataAsPath( TtPath::Canonicalize( path ) );
      }
      else {
        tree_data_object_->ClearData();
      }

      drag_handler_ = item.MakeDragImage();
      RECT rc = item.GetTextRectangle().rectangle;
      drag_handler_.Begin( 0, nm->ptDrag.x - rc.left + 20, nm->ptDrag.y - rc.top );

      DWORD dwEffect;
      ::DoDragDrop( tree_data_object_, tree_drop_source_, DROPEFFECT_MOVE | DROPEFFECT_COPY, &dwEffect );

      tree_.ClearDropTarget();
      if ( drag_handler_.IsEntered() ) {
        drag_handler_.Leave();
      }
      if ( drag_handler_.IsBegun() ) {
        drag_handler_.End();
      }
      return {WMResult::Done};
    }
    return {WMResult::Incomplete};
  } );

  ::RegisterDragDrop( tree_.GetHandle(), tree_drop_target_ );
  tree_drop_target_->SetDropHandler( [this] ( void ) {
    if ( drag_handler_.IsEntered() ) {
      drag_handler_.Leave();
      MainTree::Item to_item = tree_.GetDropHilight();
      MainTree::Item from_item = drag_handler_.GetItem();
      tree_.ClearDropTarget();

      if ( to_item != from_item && to_item.IsValid() ) {
        if ( to_item.IsDescendantOf( from_item ) ) {
          TtMessageBoxOk::Show( *this, "要素を自身の配下には転送できません。", "ERROR", TtMessageBox::Icon::ERROR );
          return;
        }
        this->StopRedraw( [&] ( void ) {
          if ( to_item.GetParameter()->TypeIs<Project::ProjectElement>() ||
               to_item.GetParameter()->TypeIs<Project::Directory>() ) {
            MainTree::Item moved = from_item.MoveToFirst( to_item );
            moved.SetSelect();
          }
          else {
            MainTree::Item moved = from_item.MoveToPrevious( to_item.GetParent(), to_item );
            moved.SetSelect();
          }
          project_changed_ = true;
        } );
      }
    }
  } );

  // 他ツリー操作
  this->AddNotifyHandler( CommandID::Control::MainTree, [this] ( NMHDR* nmhdr ) -> WMResult {
    static bool can_open_file = false;
    switch ( nmhdr->code ) {
      // 要素選択時
    case TVN_SELCHANGED: {
      NMTREEVIEW* nm = reinterpret_cast<NMTREEVIEW*>( nmhdr );
      MainTree::Item item = MainTree::Item( &tree_, nm->itemNew.hItem );
      main_menu_.TreeViewItemSelected( item );
      tree_tool_bar_.TreeViewItemSelected( item );
      tree_popup_menu_.TreeViewItemSelected( item );
      return {WMResult::Done};
    }

      // クリック時
    case NM_CLICK:
      can_open_file = true;
      break;

      // ダブルクリック時
    case NM_DBLCLK: {
      if ( can_open_file ) {
        return this->GetSingleHandler( WM_COMMAND )( MAKEWPARAM( CommandID::OpenFile, nmhdr->code ), reinterpret_cast<LPARAM>( nmhdr->hwndFrom ) );
      }
      return {WMResult::Done};
    }
      // 右クリック時
    case NM_RCLICK: {
      POINT cursor_point = TtWindow::GetCursorPosition();
      MainTree::Item hit_item = tree_.HitTest( cursor_point.x - tree_.GetPoint().x, cursor_point.y - tree_.GetPoint().y );
      if ( hit_item.IsValid() ) {
        hit_item.SetSelect();
      }
      MainTree::Item item = tree_.GetSelected();
      if ( item.IsValid() ) {
        tree_popup_menu_.PopupAbsolute( *this, cursor_point.x, cursor_point.y );
      }
      return {WMResult::Done};
    }

    case TVN_ITEMEXPANDING:
    case TVN_ITEMEXPANDED:
      can_open_file = false;
      break;
    }
    return {WMResult::Incomplete};
  } );

  // -- 出力エディットダブルクリック時 -----
  {
    OutputEdit::FilePathAndLineHandler handler = [this] ( const std::string& path, int line ) {
      if ( NOT( developer_.GetSettings().editor_path_.empty() ) ) {
        TtTextTemplate::Document document;
        document.ParseText( developer_.GetSettings().editor_argument_ );
        if ( document.HasKey( "file" ) ) {
          document["file"] = TtPath::QuoteIfHasSpaces(
            TtPath::IsRelative( path ) ? developer_.GetProjectDirectoryPath() + "\\" + path : path );
        }
        if ( document.HasKey( "line" ) ) {
          document["line"] = TtUtility::ToStringFrom( line );
        }

        TtProcess::CreateInfo info( developer_.GetSettings().editor_path_ );
        info.SetArguments( document.MakeText() );
        try {
          TtProcess process;
          process.Create( info );
        }
        catch ( TtWindowsSystemCallException& e ) {
          this->CreateProcessErrorMessageBox( ProcessCreateException( info, e.GetErrorNumber() ) );
        }
      }
    };
    tab_.GetCompilerEdit().SetFilePathAndLineHandler( handler );
    tab_.GetExecuteEdit().SetFilePathAndLineHandler( handler );
    tab_.GetExternalProgramEdit().SetFilePathAndLineHandler( handler );
  }
  // -- 出力エディットのプロセス起動関連
  {
    // -- コンパイル結果タブ
    tab_.GetCompilerEdit().GetProcessManager().SetErrorHandler( [this] ( ProcessCreateException& e ) {
      this->CreateProcessErrorMessageBox( e );
    } );
    tab_.GetCompilerEdit().GetProcessManager().SetInitializer( [this] ( void ) {
      main_menu_.SetBuildingMode();
      main_tool_bar_.SetBuildingMode();
    } );
    tab_.GetCompilerEdit().GetProcessManager().SetFinalizer( [this] ( void ) {
      main_menu_.UnsetBuildingMode();
      main_tool_bar_.UnsetBuildingMode();
    } );
    // -- 実行ファイル出力タブ
    tab_.GetExecuteEdit().GetProcessManager().SetErrorHandler( [this] ( ProcessCreateException& e ) {
      this->CreateProcessErrorMessageBox( e );
    } );
    // -- 外部プログラム出力タブ
    tab_.GetExternalProgramEdit().GetProcessManager().SetErrorHandler( [this] ( ProcessCreateException& e ) {
      this->CreateProcessErrorMessageBox( e );
    } );
    tab_.GetExternalProgramEdit().GetProcessManager().SetInitializer( [this] ( void ) {
      this->SetControlEnableExternalProgramExecuting();
    } );
    tab_.GetExternalProgramEdit().GetProcessManager().SetFinalizer( [this] ( void ) {
      this->SetControlEnableExternalProgramEnded();
    } );
  }

  // -- 終了時
  this->RegisterWMClose( [this] ( void ) -> WMResult {
    if ( NOT( this->SaveProjectConfirmation() ) ) {
      return {WMResult::Done};
    }
    IniFileOperation::SavePlacement( this->GetWindowPlacement() );
    IniFileOperation::SaveSplitterPosition( main_splitter_panel_.GetSplitterPosition() );
    IniFileOperation::SaveLastProject( developer_.GetProjectPath() );
    return {WMResult::Incomplete}; // 処理差し込みの為
  }, false );

  // -- テスト -----
  this->AddCommandHandler( CommandID::Test1, [this] ( int, HWND ) -> WMResult {
    return {WMResult::Done};
  } );

  this->AddCommandHandler( CommandID::Test2, [this] ( int, HWND ) -> WMResult {
    return {WMResult::Done};
  } );

  this->AddCommandHandler( CommandID::Test3, [this] ( int, HWND ) -> WMResult {
    return {WMResult::Done};
  } );

  this->AddCommandHandler( CommandID::Test4, [this] ( int, HWND ) -> WMResult {
    return {WMResult::Done};
  } );

  this->AddCommandHandler( CommandID::Test5, [this] ( int, HWND ) -> WMResult {
    return {WMResult::Done};
  } );
}


Developer&
MainFrame::GetDeveloper( void )
{
  return developer_;
}


MainTab&
MainFrame::GetMainTab( void )
{
  return tab_;
}


void
MainFrame::SetTitleBar( void )
{
  std::string tmp;
  if ( NOT( developer_.GetProject().name_.empty() ) ) {
    tmp.append( developer_.GetProject().name_ );
    tmp.append( " - " );
  }
  tmp.append( APPLICATION_NAME );

  this->SetIconAsLarge( Image::ICONS[Image::Index::Main] );
  this->SetIconAsSmall( Image::ICONS[Image::Index::MainSmall] );
  if ( developer_.GetProject().use_icon_ ) {
    try {
      title_bar_icon_large_ = TtIcon::CreateFromFileAsLarge( developer_.GetProject().GetIconAbsolutePath(), developer_.GetProject().icon_file_index_ );
      this->SetIconAsLarge( title_bar_icon_large_ );
      title_bar_icon_small_ = TtIcon::CreateFromFileAsSmall( developer_.GetProject().GetIconAbsolutePath(), developer_.GetProject().icon_file_index_ );
      this->SetIconAsSmall( title_bar_icon_small_ );
    }
    catch ( TtWindowsSystemCallException ) {
    }
  }

  this->SetText( tmp );
}


void
MainFrame::SetBeginningMode( void )
{
  main_menu_.SetBeginningMode();
  main_tool_bar_.SetBeginningMode();
  tree_tool_bar_.SetBeginningMode();
  tree_popup_menu_.SetBeginningMode();
}

void
MainFrame::SetNormalMode( void )
{
  main_menu_.SetNormalMode();
  main_tool_bar_.SetNormalMode();
  tree_tool_bar_.SetNormalMode();
  tree_popup_menu_.SetNormalMode();
}


void
MainFrame::SetControlEnableExternalProgramExecuting( void )
{
  auto tmp = [] ( TtSubMenu& menu ) {
    for ( int i = 0; i < menu.GetItemCount(); ++i ) {
      ExternalProgramMenuItem item = menu.GetItemAt( i );
      if ( item.GetParameter() && item.GetParameter()->use_output_edit_ ) {
        item.SetEnabled( false );
      }
    }
  };
  tmp( main_menu_.external_program_menu_ );
  tmp( tree_popup_menu_.external_program_menu_ );
  main_menu_.GetStopExternalProgramMenuItem().SetEnabled( true );
  main_tool_bar_.GetButton( CommandID::MainToolBar::StopExternalProgram ).Enable();
}

void
MainFrame::SetControlEnableExternalProgramEnded( void )
{
  auto tmp = [] ( TtSubMenu& menu ) {
    for ( int i = 0; i < menu.GetItemCount(); ++i ) {
      menu.GetItemAt( i ).SetEnabled( true );
    }
  };
  tmp( main_menu_.external_program_menu_ );
  tmp( tree_popup_menu_.external_program_menu_ );
  main_menu_.GetStopExternalProgramMenuItem().SetEnabled( false );
  main_tool_bar_.GetButton( CommandID::MainToolBar::StopExternalProgram ).Disable();
}


void
MainFrame::LoadPlacement( void )
{
  WINDOWPLACEMENT placement;
  if ( IniFileOperation::LoadPlacement( placement ) ) {
    this->SetWindowPlacement( placement );
  }
}


void
MainFrame::SetCurrentStructureMenu( void )
{
  main_menu_.current_structure_menu_.Clear();
  main_tool_bar_.structure_combo_box_.Clear();
  for ( auto& structure : developer_.GetProject().GetStructures() ) {
    main_menu_.current_structure_menu_.AppendNewItem( 0, structure->name_ );
    main_tool_bar_.structure_combo_box_.Push( structure->name_ );
    if ( structure.get() == &developer_.GetCurrentStructure() ) {
      main_menu_.current_structure_menu_.CheckRadioItem( main_menu_.current_structure_menu_.GetItemCount() - 1 );
      main_tool_bar_.structure_combo_box_.SetSelect( main_tool_bar_.structure_combo_box_.GetCount() - 1 );
    }
  }
}


void
MainFrame::SetFromSettings( void )
{
  this->InitializeSquirrelVMGeneral();
  this->SetAccelerator();
}

void
MainFrame::InitializeSquirrelVMGeneral( void )
{
  vm_general_.emplace( *this, developer_.IsOperable() );
  if ( vm_general_->use_ ) {
    bool ret = this->SquirrelErrorHandlingReturnErrorNotOccurred( [this] ( void ) -> bool {
      std::string path = developer_.GetSettings().extension_script_path_;
      if ( NOT( path.empty() ) ) {
        vm_general_->DoFile( path );
      }
      this->SetExternalProgramMenu();
      return true;
    } );
    // -- エラー時
    if ( NOT( ret ) ) {
      external_program_menu_holder_.clear();
      main_menu_.SetExternalProgramMenu( this->CreateEmptyExternalProgramMenu() );
      tree_popup_menu_.SetExternalProgramMenu( this->CreateEmptyExternalProgramMenu() );
    }
  }
}

void
MainFrame::SetExternalProgramMenu( void )
{
  if ( NOT( vm_general_->use_ ) ) {
    return;
  }

  auto register_menu_command_handler = [this] ( TtSubMenuCommand& menu ) {
    // ここキャプチャで menu を保持する
    external_program_menu_holder_.push_back( TtUtility::SharedDestructorCall( [this, menu] ( void ) mutable -> void {
      this->RemoveMenuCommandHandler( menu );
    } ) );

    this->AddMenuCommandHandler( menu, [this] ( ExternalProgramMenuItem item ) -> WMResult {
      auto& external_program = *item.GetParameter();
      std::string open_file;
      if ( external_program.for_open_file_ ) {
        MainTree::Item selected = tree_.GetSelected();
        if ( selected.IsValid() && selected.GetParameter()->TypeIs<Project::File>() ) {
          std::string path = developer_.GetProjectDirectoryPath() + "\\" + selected.GetParameterAs<Project::File*>()->GetPath();
          open_file = TtPath::Canonicalize( path );
        }
      }
      this->ExecuteExternalProgram( external_program, open_file );
      return {WMResult::Done};
    } );
  };

  external_program_menu_holder_.clear();
  external_program_menu_normal_maker_ = vm_general_->GetSubMenuCommandMakerOfExternalProgramNormal();
  external_program_menu_normal_maker_.SetAtMakeMenu( register_menu_command_handler );
  main_menu_.SetExternalProgramMenu( external_program_menu_normal_maker_.MakeMenu() );
  if ( external_program_menu_normal_maker_.GetRoot().empty() ) {
    main_menu_.SetExternalProgramMenu( this->CreateEmptyExternalProgramMenu() );
  }

  external_program_menu_open_file_maker_ = vm_general_->GetSubMenuCommandMakerOfExternalProgramOpenFile();
  external_program_menu_open_file_maker_.SetAtMakeMenu( register_menu_command_handler );
  tree_popup_menu_.SetExternalProgramMenu( external_program_menu_open_file_maker_.MakeMenu() );
  if ( external_program_menu_open_file_maker_.GetRoot().empty() ) {
    tree_popup_menu_.SetExternalProgramMenu( this->CreateEmptyExternalProgramMenu() );
  }
}

TtSubMenuCommand
MainFrame::CreateEmptyExternalProgramMenu( void )
{
  TtSubMenuCommand menu = TtSubMenuCommand::Create();
  TtMenuItem item = menu.AppendNewItem( 0, "(なし)" );
  item.SetEnabled( false );
  item.SetParameterAs<void*>( nullptr );
  return menu;
}

void
MainFrame::ExecuteExternalProgram( Settings::ExternalProgram& external_program, const std::string& open_file )
{
  TtProcess::CreateInfo info = developer_.MakeProcessCreateInfo( external_program, *this, open_file );

  if ( external_program.use_output_edit_ ) {
    TtCriticalSection::Lock lock_object( process_execute_lock_ );

    tab_.Select( MainTab::ExternalProgram );

    info.SetShowState( TtWindow::ShowState::HIDE );

    auto& manager = tab_.GetExternalProgramEdit().GetProcessManager();
    manager.SetFirst( [this] ( void ) {
      tab_.GetExternalProgramEdit().Clear();
      return true;
    } );

    manager.GetCommands().clear();
    manager.GetCommands().push_back( {info} );
    manager.ThreadStart();
  }
  else {
    try {
      TtProcess process;
      process.Create( info );
    }
    catch ( TtWindowsSystemCallException& e ) {
      this->CreateProcessErrorMessageBox( ProcessCreateException( info, e.GetErrorNumber() ) );
      return;
    }
  }
  return;
}


void
MainFrame::InitializeProjectHistory( void )
{
  IniFileOperation::LoadProjectHistory( project_history_, developer_.GetSettings().project_history_count_max_ );
  this->SetProjectHistory();
}

void
MainFrame::SetProjectHistory( void )
{
  auto& menu = main_menu_.project_history_menu_;
  menu.Clear();
  this->ClearMenuCommandHandler( menu );
  if ( project_history_.empty() ) {
    auto item = menu.AppendNewItem( 0, "(なし)" );
    item.SetEnabled( false );
    return;
  }

  for ( const auto& [project_name, file_path] : project_history_ ) {
    menu.AppendNewItem( 0, project_name + " : " + file_path );
    menu.GetLastItem().SetBmpImage( Image::BMPS[Image::Index::ProjectElement] );
  }
  this->AddMenuCommandHandler( menu, [this] ( TtMenuItem item ) mutable -> WMResult {
    if ( NOT( this->SaveProjectConfirmation() ) ) {
      return {WMResult::Done};
    }
    this->LoadProjectFile( project_history_[item.GetIndex()].second );
    return {WMResult::Done};
  } );
}

void
MainFrame::SetAccelerator( void )
{
  using Modifier = TtForm::AcceleratorMap::ShortcutKey::Modifier;
  TtForm::AcceleratorMap map;
  map.Register( {Modifier::Control, 'N'  }, CommandID::NewProject );
  map.Register( {Modifier::Control, 'O'  }, CommandID::OpenProject );
  map.Register( {Modifier::Control, 'S'  }, CommandID::SaveProject );
  map.Register( {Modifier::None,    VK_F5}, CommandID::Compile );
  map.Register( {Modifier::None,    VK_F6}, CommandID::Build );
  map.Register( {Modifier::None,    VK_F7}, CommandID::Rebuild );
  map.Register( {Modifier::None,    VK_F9}, CommandID::Execute );

  auto register_key = [&map] ( Settings::ShortcutKey& key, WORD command_id ) {
    if ( key.GetVirtualKey() != 0 ) {
      map.Register( {key.GetModifier(), key.GetVirtualKey()}, command_id );
    }
  };
  register_key( developer_.GetSettings().compile_key_, CommandID::Compile );
  register_key( developer_.GetSettings().build_key_,   CommandID::Build );
  register_key( developer_.GetSettings().rebuild_key_, CommandID::Rebuild );
  register_key( developer_.GetSettings().execute_key_, CommandID::Execute );
  TtForm::RegisterAccelerator( *this, map );
}


void
MainFrame::Build( SquirrelVM& vm )
{
  TtCriticalSection::Lock lock_object( process_execute_lock_ );

  tab_.Select( MainTab::Compiler );

  try {
    developer_.CreateMakefile( vm );
  }
  catch ( MakeFileCreateException& e ) {
    TtMessageBoxOk box;
    box.SetMessage( e.Message() );
    box.SetCaption( "メイクファイル作成エラー" );
    box.SetIcon( TtMessageBox::Icon::ERROR );
    box.ShowDialog( *this );
    return;
  }

  auto& manager = tab_.GetCompilerEdit().GetProcessManager();

  manager.SetFirst( [this] ( void ) {
    return this->IfVMIsUseableSquirrelErrorHandlingReturnErrorNotOccurred( [&] () {
      return vm_build_->CallBuildFunctionFirst();
    } );
  } );

  manager.SetLast( [this, start_time = TtTime::GetNow()] ( bool exist_error ) {
    if ( NOT( exist_error ) ) {
      exist_error = NOT( this->IfVMIsUseableSquirrelErrorHandlingReturnErrorNotOccurred( [&] () {
        return vm_build_->CallBuildFunctionLast( exist_error );
      } ) );
    }

    char buf[256];
    sprintf_s( buf, sizeof( buf ), "\r\n%s End. (%.2f sec)",
               exist_error ? "Error" : "Success", TtTime::GetNow() - start_time );
    tab_.GetCompilerEdit().AddText( buf );
  } );

  manager.ThreadStart();
}


void
MainFrame::Execute( void )
{
  if ( developer_.GetCurrentStructure().target_kind_ != Project::TargetKind::Windows &&
       developer_.GetCurrentStructure().target_kind_ != Project::TargetKind::Console ) {
    return;
  }

  TtCriticalSection::Lock lock_object( process_execute_lock_ );

  if ( tab_.GetExecuteEdit().GetProcessManager().GetCommands().size() != 0 ) {
    TtProcess& process = tab_.GetExecuteEdit().GetProcessManager().GetCommands().front().process_;
    if ( process.IsCreated() && NOT( process.HasExited() ) ) {
      process.Terminate( 0 );
      return;
    }
  }

  tab_.Select( MainTab::Execute );

  Developer::ProjectVariablesTranslator translator( developer_ );
  translator.SetUserInputHandlerAndParentOfDialog( developer_.GetCurrentStructure().target_name_ + "への入力", *this );

  TtProcess::CreateInfo info( developer_.GetTargetFilePath() );
  info.SetArguments( translator.Translate( developer_.GetCurrentStructure().target_argument_ ) );
  if ( NOT( developer_.GetCurrentStructure().target_current_directory_.empty() ) ) {
    info.SetCurrentDirectory( translator.Translate( developer_.GetCurrentStructure().target_current_directory_ ) );
  }
  info.SetPriority( TtProcess::Priority::BELOW_NORMAL );

  if ( developer_.GetCurrentStructure().target_use_output_edit_ ) {
    auto& manager = tab_.GetExecuteEdit().GetProcessManager();
    manager.SetFirst( [this] ( void ) {
      tab_.GetExecuteEdit().Clear();
      return true;
    } );

    manager.GetCommands().clear();
    manager.GetCommands().push_back( {info} );
    manager.ThreadStart();
  }
  else {
    try {
      TtProcess process;
      process.Create( info );
    }
    catch ( TtWindowsSystemCallException& e ) {
      this->CreateProcessErrorMessageBox( ProcessCreateException( info, e.GetErrorNumber() ) );
      return;
    }
  }
}


void
MainFrame::LoadProjectFile( const std::string& path )
{
  developer_.GetProject().path_ = path;
  this->ProjectSaveLoadErrorHandling( [this] ( void ) {
    developer_.GetProject().LoadFromFile( tree_ );
    developer_.SetCurrentStructureByIndex( 0 );
    project_changed_ = false;
    this->SetNormalMode();
    tree_.GetRoot().SetSelect();
    if ( tree_.GetRoot().HasChild() ) {
      tree_.GetRoot().Expand();
    }
    this->SetCurrentStructureMenu();
    this->InitializeSquirrelVMGeneral();
    this->SetTitleBar();

    IniFileOperation::LoadProjectHistory( project_history_, developer_.GetSettings().project_history_count_max_ );
    // 同じプロジェクトは履歴から削除
    project_history_.erase(
      std::remove_if( project_history_.begin(), project_history_.end(),
                      [this] ( std::pair<std::string, std::string>& one ) { return one.second == developer_.GetProject().path_; } ),
      project_history_.end() );
    project_history_.push_front( std::make_pair( developer_.GetProject().name_, developer_.GetProject().path_ ) );
    IniFileOperation::SaveProjectHistory( project_history_, developer_.GetSettings().project_history_count_max_ );
    this->SetProjectHistory();
  } );
}


void
MainFrame::CreateProcessErrorMessageBox( ProcessCreateException& e )
{
  TtMessageBoxOk box;
  box.SetMessage( e.Message() );
  box.SetCaption( "実行ファイルの起動エラー" );
  box.SetIcon( TtMessageBox::Icon::ERROR );
  box.ShowDialog( *this );
}

void
MainFrame::ProjectSaveLoadErrorHandling( std::function<void ( void )> function )
{
  try {
    function();
  }
  catch ( FileNotExistException& e ) {
    TtMessageBoxOk box;
    box.SetCaption( "プロジェクトファイル存在エラー" );
    box.SetMessage( e.Message() );
    box.SetIcon( TtMessageBox::Icon::ERROR );
    box.ShowDialog( *this );
  }
  catch ( ProjectFileFormatException& e ) {
    TtMessageBoxOk box;
    box.SetCaption( "プロジェクトファイルフォーマットエラー" );
    box.SetMessage( e.Message() );
    box.SetIcon( TtMessageBox::Icon::ERROR );
    box.ShowDialog( *this );
  }
  catch ( TtWindowsSystemCallException& e ) {
    TtMessageBoxOk box;
    box.SetCaption( "システムコールエラー" );
    box.AppendMessage( "プロジェクトファイルの読み書き中にエラーが発生しました。\r\n" );
    box.AppendMessage( "メッセージ : " + e.GetSystemErrorMessage() );
    box.SetIcon( TtMessageBox::Icon::ERROR );
    box.ShowDialog( *this );
  }
}

bool
MainFrame::SaveProjectConfirmation( void )
{
  if ( NOT( project_changed_ ) ) {
    return true;
  }
  TtMessageBoxYesNoCancel box;
  box.SetCaption( "プロジェクトの上書き保存の確認" );
  box.AppendMessage() << "プロジェクトは変更されています。上書き保存しますか？";
  box.SetIcon( TtMessageBox::Icon::WARNING );
  switch ( box.ShowDialog( *this ) ) {
  case TtMessageBox::Result::CANCEL:
    return false;

  case TtMessageBox::Result::YES: {
    bool ret = false;
    this->ProjectSaveLoadErrorHandling( [this, &ret] ( void ) {
      developer_.GetProject().SaveToFile( tree_ );
      project_changed_ = false;
      ret = true;
    } );
    return ret;
  }

  case TtMessageBox::Result::NO:
  default :
    return true;
  }
}


void
MainFrame::InitializeSquirrelVMBuild( void )
{
  vm_build_.emplace( *this, NOT( developer_.GetCurrentStructure().extension_script_path_.empty() ) );
  if ( vm_build_->use_ ) {
    vm_build_->output_edit_ = &tab_.GetCompilerEdit();
    std::string path = developer_.GetCurrentStructure().extension_script_path_;
    if ( TtPath::IsRelative( path ) ) {
      path = developer_.GetProjectDirectoryPath() + "\\" + path;
    }
    vm_build_->DoFile( path );
  }
}


void
MainFrame::SquirrelErrorHandling( std::function<void ( void )> function )
{
  this->SquirrelErrorHandlingReturnErrorNotOccurred( [&] ( void ) {
    function();
    return true;
  } );
}

bool
MainFrame::SquirrelErrorHandlingReturnErrorNotOccurred( std::function<bool ( void )> function )
{
  try {
    return function();
  }
  catch ( TtSquirrel::Exception& ex ) {
    TtMessageBoxOk box;
    std::string tmp = "拡張スクリプトでエラーがありました。";
    std::string message = ex.GetStandardMessage();
    if ( NOT( message.empty() ) ) {
      tmp.append( "\r\n\r\n" );
      tmp.append( message );
    }
    box.SetMessage( tmp );
    box.SetCaption( "拡張スクリプトのエラー" );
    box.SetIcon( TtMessageBox::Icon::ERROR );
    box.ShowDialog( *this );
    return false;
  }
}

bool
MainFrame::IfVMIsUseableSquirrelErrorHandlingReturnErrorNotOccurred( std::function<bool ( void )> function )
{
  if ( vm_build_->use_ ) {
    return this->SquirrelErrorHandlingReturnErrorNotOccurred( function );
  }
  return true;
}
