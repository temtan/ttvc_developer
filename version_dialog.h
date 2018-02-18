// version_dialog.h

#pragma once

#include "tt_icon.h"
#include "tt_window.h"
#include "tt_dialog.h"
#include "tt_window_controls.h"

#include "settings.h"


namespace TTVCDeveloper {
  class VersionDialog : public TtDialog {
  public:
    explicit VersionDialog( const Settings& settings );

    virtual DWORD GetExtendedStyle( void );

    virtual bool Created( void );

  private:
    const Settings& settings_;

    TtIcon   icon_;
    TtStatic name_label_;
    TtStatic version_label_;
    TtStatic date_label_;
    TtButton close_button_;
    TtStatic compiler_label_;
    TtStatic x86_version_label_;
    TtStatic x64_version_label_;
  };
}
