// version_dialog.cpp

#include "common.h"
#include "utility.h"

#include "version_dialog.h"

using namespace TTVCDeveloper;


VersionDialog::VersionDialog( const Settings& settings ) :
settings_( settings ),
icon_( Image::ICONS[Image::Index::Main] )
{
}


DWORD
VersionDialog::GetExtendedStyle( void )
{
  return WS_EX_TOOLWINDOW;
}


bool
VersionDialog::Created( void )
{
  const int ButtonCommandID = 10001;

  name_label_.Create( {this} );
  version_label_.Create( {this} );
  date_label_.Create( {this} );
  close_button_.Create( {this, ButtonCommandID} );
  compiler_label_.Create( {this} );
  x86_version_label_.Create( {this} );
  x64_version_label_.Create( {this} );

  this->SetText( "バージョン情報" );

  this->SetClientSize( 254, 128, false );

  name_label_.SetPositionSize(         60,  12, 100,  24 );
  version_label_.SetPositionSize(      60,  28, 100,  24 );
  date_label_.SetPositionSize(         60,  44, 200,  24 );
  close_button_.SetPositionSize(      180,  12,  40,  24 );
  compiler_label_.SetPositionSize(     60,  66, 200,  24 );
  x86_version_label_.SetPositionSize(  60,  84, 200,  24 );
  x64_version_label_.SetPositionSize(  60, 100, 200,  24 );

  name_label_.SetText( TTVCDeveloper::APPLICATION_NAME );
  version_label_.SetText( std::string( "version " ) + TTVCDeveloper::VERSION );
  {
    std::string date_str;
    date_str.append( "Build at : " );
    date_str.append( TTL_DATE_STRING );
    date_str.append( " " );
    date_str.append( __TIME__ );
    date_label_.SetText( date_str );
  }
  close_button_.SetText( "閉" );
  compiler_label_.SetText( "コンパイラのバージョン" );
  x86_version_label_.SetText( "x86 : " + Utility::GetFirstProductVersionFromFile( settings_.compiler_x86_.compiler_path_ ) );
  x64_version_label_.SetText( "x64 : " + Utility::GetFirstProductVersionFromFile( settings_.compiler_x64_.compiler_path_ ) );

  this->AddCommandHandler( ButtonCommandID, [this] ( int, HWND ) -> WMResult {
    this->Close();
    return {WMResult::Done};
  } );

  this->RegisterWMPaint( [this] ( void ) -> WMResult {
    PAINTSTRUCT ps;
    HDC hdc = ::BeginPaint( handle_, &ps );
    ::DrawIcon( hdc, 10, 10, icon_.GetHandle() );
    ::EndPaint( handle_, &ps );
    return {WMResult::Done};
  }, false );

  name_label_.Show();
  version_label_.Show();
  date_label_.Show();
  close_button_.Show();
  compiler_label_.Show();
  x86_version_label_.Show();
  x64_version_label_.Show();

  close_button_.SetFocus();

  this->SetCenterRelativeToParent();

  return true;
}
