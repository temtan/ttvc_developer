// version_dialog.h

#pragma once

#include "tt_icon.h"
#include "tt_window.h"
#include "tt_dialog.h"
#include "tt_window_controls.h"


namespace TTVCDeveloper {
  class VersionDialog : public TtDialog {
  public:
    explicit VersionDialog( void );

    virtual DWORD GetExtendedStyle( void );

    virtual bool Created( void );

  private:
    TtIcon   icon_;
    TtStatic name_label_;
    TtStatic version_label_;
    TtStatic date_label_;
    TtButton close_button_;
  };
}
