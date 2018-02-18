// version_dialog.cpp

#include "common.h"

#include "version_dialog.h"

using namespace TTVCDeveloper;


VersionDialog::VersionDialog( void ) :
icon_( Image::ICONS[Image::Index::Main] ),
name_label_(),
version_label_(),
date_label_(),
close_button_()
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

  this->SetText( "ƒo[ƒWƒ‡ƒ“î•ñ" );

  this->SetClientSize( 254, 68, false );

  name_label_.SetPositionSize(     60, 12, 100, 24 );
  version_label_.SetPositionSize(  60, 28, 100, 24 );
  date_label_.SetPositionSize(     60, 44, 200, 24 );
  close_button_.SetPositionSize(  180, 12,  40, 24 );

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
  close_button_.SetText( "•Â" );

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

  close_button_.SetFocus();

  this->SetCenterRelativeToParent();

  return true;
}
