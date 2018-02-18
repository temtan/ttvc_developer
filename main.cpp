// main.cpp

#include "ttl_define.h"
#include "tt_window.h"
#include "tt_message_box.h"
#include "tt_command_line_analyzer.h"
#include "tt_path.h"

#include "common.h"
#include "main_frame.h"
#include "settings_property_sheet.h"
#include "exception.h"

#include <commoncontrols.h>


// -----------------------------------------------------------------------
int WINAPI
WinMain( HINSTANCE h_instance,
         HINSTANCE h_prev_instance,
         PSTR lp_cmd_line,
         int n_cmd_show )
{
  NOT_USE( h_instance );
  NOT_USE( h_prev_instance );
  NOT_USE( n_cmd_show );

  TtWindow::InitializeCommonControls();

  try {
    TTVCDeveloper::Image::Initialize();
    TTVCDeveloper::MainFrame frame;
    frame.Create();
    frame.SetBeginningMode();
    if ( NOT( TtPath::FileExists( TtPath::GetExecutingFilePathCustomExtension( "ini" ) ) ) ) {
      TTVCDeveloper::SettingsPropertySheet sheet( frame.GetDeveloper().GetSettings() );
      sheet.ShowDialog( frame );
    }
    if ( NOT( TtPath::FileExists( TTVCDeveloper::Utility::GetDefaultProjectFilePath() ) ) ) {
      TTVCDeveloper::Utility::CreateDefaultProjectFile();
    }
    frame.LoadPlacement();
    if ( frame.IsMinimized() ) {
      frame.Restore();
    }
    frame.GetDeveloper().GetSettings().ReadFromFile();
    frame.SetFromSettings();

    // �q�X�g����ǂݍ���
    frame.InitializeProjectHistory();

    // �v���W�F�N�g�t�@�C�����J��
    std::string project_file_path = TTVCDeveloper::IniFileOperation::LoadLastProject();
    try {
      TtCommandLine::Analyzer analyzer;
      auto rest = analyzer.AnalyzeForWindows( lp_cmd_line );
      if ( rest.size() > 0 ) {
        project_file_path = rest.front();
      }
    }
    catch ( TtCommandLine::Exception& e ) {
      TtMessageBoxOk box;
      ( box.AppendMessage() << "�R�}���h���C�������ŃG���[���������܂����B\r\n" <<
        "��� : " << e.Dump() << "\r\n\r\n" <<
        "�R�}���h���C�������͖�������܂��B" );
      box.SetCaption( "�R�}���h���C���G���[" );
      box.SetIcon( TtMessageBox::Icon::ERROR );
      box.ShowDialog();
    }
    if ( NOT( project_file_path.empty() ) ) {
      frame.LoadProjectFile( project_file_path );
    }

    frame.Show();
    return TtForm::LoopDispatchMessage();
  }
  catch ( TTVCDeveloper::Exception& e ) {
    TTVCDeveloper::IniFileOperation::SaveErrorLogDump( e );
    TtMessageBoxOk box;
    box.SetMessage( e.Message() );
    box.SetCaption( "�G���[" );
    box.SetIcon( TtMessageBox::Icon::ERROR );
    box.ShowDialog();
  }
  catch ( TtException& e ) {
    TTVCDeveloper::IniFileOperation::SaveErrorLogDump( e );
    TtMessageBoxOk box;
    box.SetMessage( "�\�����Ȃ��G���[���������܂����B�A�v���P�[�V�������I�������܂��B" );
    box.SetCaption( "�\�����Ȃ��G���[" );
    box.SetIcon( TtMessageBox::Icon::ERROR );
    box.ShowDialog();
  }
  return 0;
}


#include "tt_string.h"
int
main( int argc, char** argv )
{
  std::string args;
  for ( int i = 1; i < argc; ++i ) {
    args.append( argv[i] );
    args.append( " " );
  }
  TtString::SharedString tmp( args );
  return WinMain( ::GetModuleHandle( NULL ), NULL, tmp.GetPointer(), 0 );
}
