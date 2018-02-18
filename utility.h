// utility.h

#pragma once

#include <functional>

#include "tt_process.h"
#include "tt_file_dialog.h"


namespace TTVCDeveloper::Utility {
  std::string GetDefaultProjectFilePath( void );
  void CreateDefaultProjectFile( void );

  std::string TogglePathFullRelative( const std::string& path, const std::string& base_dir );

  bool TouchFile( const std::string& path );

  bool MakeDirectoryOfFileIfNotExist( const std::string& path );

  void SetSourceFileFilters( TtFileDialog& dialog );

  std::string GetFirstProductVersionFromFile( const std::string& path );
}
