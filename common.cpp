// common.cpp

#include "ttl_define.h"
#include "tt_path.h"
#include "tt_ini_file.h"
#include "tt_time.h"

#include "exception.h"

#include "common.h"

using namespace TTVCDeveloper;


// -- Enums --------------------------------------------------------------
template <>
TtEnumTable<Platform>::TtEnumTable( void )
{
#define REGISTER( NAME ) this->Register( Platform::NAME, #NAME )
  REGISTER( X86 );
  REGISTER( X64 );
#undef REGISTER
}
template <>
TtEnumTable<Platform>
TtEnumBase<Platform>::TABLE;


// -- IniFileOperation ---------------------------------------------------
namespace Tag {
#define DEFINE_PARAMETER_NAME_STRING( name ) static const std::string name( #name )
  DEFINE_PARAMETER_NAME_STRING( TTVCDeveloper );
  DEFINE_PARAMETER_NAME_STRING( ErrorLog );
  DEFINE_PARAMETER_NAME_STRING( ProjectHistory );

  DEFINE_PARAMETER_NAME_STRING( Placement );
  DEFINE_PARAMETER_NAME_STRING( SplitterPosition );
  DEFINE_PARAMETER_NAME_STRING( LastProject );
  DEFINE_PARAMETER_NAME_STRING( Dump );
  DEFINE_PARAMETER_NAME_STRING( TestMode );
  DEFINE_PARAMETER_NAME_STRING( SecretShortcutKey );
  DEFINE_PARAMETER_NAME_STRING( Time );
  DEFINE_PARAMETER_NAME_STRING( History );
  DEFINE_PARAMETER_NAME_STRING( Count );
}

void
IniFileOperation::SavePlacement( WINDOWPLACEMENT& placement )
{
  TtIniFile ini;
  TtIniSection section = ini[Tag::TTVCDeveloper];
  section.SetString( Tag::Placement, TtUtility::Serialize( placement ) );
}

bool
IniFileOperation::LoadPlacement( WINDOWPLACEMENT& placement )
{
  TtIniFile ini;
  TtIniSection section = ini[Tag::TTVCDeveloper];
  auto keys = section.GetKeys();
  if ( std::find( keys.begin(), keys.end(), Tag::Placement ) == keys.end() ) {
    return false;
  }
  return TtUtility::Deserialize( section.GetString( Tag::Placement ), placement );
}


void
IniFileOperation::SaveSplitterPosition( unsigned int width )
{
  TtIniFile ini;
  TtIniSection section = ini[Tag::TTVCDeveloper];
  section.SetInteger( Tag::SplitterPosition, width );
}

unsigned int
IniFileOperation::LoadSplitterPosition( void )
{
  TtIniFile ini;
  TtIniSection section = ini[Tag::TTVCDeveloper];
  return section.GetInteger( Tag::SplitterPosition, 180 );
}


void
IniFileOperation::SaveLastProject( const std::string& path )
{
  TtIniFile ini;
  TtIniSection section = ini[Tag::TTVCDeveloper];
  section.SetString( Tag::LastProject, path );
}

std::string
IniFileOperation::LoadLastProject( void )
{
  TtIniFile ini;
  TtIniSection section = ini[Tag::TTVCDeveloper];
  return section.GetString( Tag::LastProject, "" );
}


void
IniFileOperation::SaveProjectHistory( const ProjectHistory& history, unsigned int max )
{
  TtIniFile ini;
  TtIniSection section = ini[Tag::ProjectHistory];
  unsigned int i = 0;
  for ( const auto& [project_name, file_path] : history ) {
    section.SetString( Tag::History + TtUtility::ToStringFrom( i ), project_name + "\a" + file_path );
    ++i;
    if ( i >= max ) {
      break;
    }
  }
  section.SetInteger( Tag::Count, i );
}

void
IniFileOperation::LoadProjectHistory( ProjectHistory& history, unsigned int max )
{
  history.clear();
  TtIniFile ini;
  TtIniSection section = ini[Tag::ProjectHistory];
  unsigned int count = section.GetInteger( Tag::Count, 0 );
  for ( unsigned int i = 0; i < count && i <= max; ++i ) {
    std::string data = section.GetString( Tag::History + TtUtility::ToStringFrom( i ) );
    if ( data.empty() ) {
      continue;
    }
    unsigned int p = data.find( '\a', 0 );
    if ( p == std::string::npos ) {
      continue;
    }
    history.push_back( std::make_pair( data.substr( 0, p ), data.substr( p + 1 ) ) );
  }
}


void
IniFileOperation::SaveErrorLogDump( TtException& e )
{
  TtIniFile ini;
  TtIniSection section = ini[Tag::ErrorLog];
  section.SetString( Tag::Time, TtTime::GetNow().GetDateTimeString() );
  section.SetString( Tag::Dump, e.Dump() );
}


bool
IniFileOperation::LoadTestMode( void )
{
  TtIniFile ini;
  TtIniSection section = ini[Tag::TTVCDeveloper];
  return section.GetBoolean( Tag::TestMode, false );
}


// -- Image --------------------------------------------------------------
#include "resource.h"

std::vector<TtIcon>
Image::ICONS;

std::vector<TtBmpImage>
Image::BMPS;

std::optional<TtImageList>
Image::LIST( std::nullopt );

int
Image::OffsetOfList = 0;

void
Image::Initialize( void )
{
  Image::LIST.emplace( TtImageList::GetSmallSystemImageList() );
  Image::OffsetOfList = Image::LIST->GetCount();

  auto add = [] ( DWORD id ) {
    TtIcon icon = TtIcon::CreateFromResourceID( id );
    Image::ICONS.push_back( icon );
    Image::BMPS.push_back( icon.GetARGBBitmapAsSmall() );
    Image::LIST->AddIcon( icon );
  };

  add( RSID_ICON_MAIN );
  add( RSID_ICON_MAIN_SMALL );
  add( RSID_ICON_NEW_PROJECT );
  add( RSID_ICON_OPEN_PROJECT );
  add( RSID_ICON_SAVE_PROJECT );
  add( RSID_ICON_PROJECT_HISTORY );
  add( RSID_ICON_PROJECT_SETTINGS );
  add( RSID_ICON_DEFAULT_PROJECT_SETTINGS );
  add( RSID_ICON_COMPILE );
  add( RSID_ICON_BUILD );
  add( RSID_ICON_REBUILD );
  add( RSID_ICON_CLEAN );
  add( RSID_ICON_EXECUTE );
  add( RSID_ICON_EXTERNAL_PROGRAM );
  add( RSID_ICON_STOP_EXTERNAL_PROGRAM );
  add( RSID_ICON_ADD_NEW_FILE );
  add( RSID_ICON_ADD_EXISTING_FILE );
  add( RSID_ICON_ADD_DIRECTORY );
  add( RSID_ICON_DELETE_ELEMENT );
  add( RSID_ICON_DOWN_ELEMENT );
  add( RSID_ICON_UP_ELEMENT );
  add( RSID_ICON_RENAME_ELEMENT );
  add( RSID_ICON_PROJECT_ELEMENT );
  add( RSID_ICON_DIRECTORY );
  add( RSID_ICON_C_FILE );
  add( RSID_ICON_CPP_FILE );
  add( RSID_ICON_RESOURCE_SCRIPT_FILE );
  add( RSID_ICON_HEADER_FILE );
  add( RSID_ICON_OTHER_FILE );
}


// -- Font ---------------------------------------------------------------
TtFont
Font::OUTPUT_MONOSPACED = TtFont( ::CreateFont( 14, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
                                                SHIFTJIS_CHARSET, OUT_DEFAULT_PRECIS,
                                                CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
                                                DEFAULT_PITCH | FF_MODERN, nullptr ), true );
