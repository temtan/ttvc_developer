// utility_dialogs.h

#pragma once

#include "tt_form.h"
#include "tt_dialog.h"
#include "tt_window_controls.h"
#include "tt_list_view.h"


namespace TTVCDeveloper {
  // -- FileInputDialog --------------------------------------------------
  class FileInputDialog : public TtDialog {
  public:
    explicit FileInputDialog( const std::string& default_path );

    virtual DWORD GetStyle( void ) override;
    virtual DWORD GetExtendedStyle( void ) override;

    virtual bool Created( void ) override;

    const std::string& GetPath( void );

  private:
    std::string path_;

    TtStatic label_;
    TtEdit   path_edit_;
    TtButton ref_button_;
    TtButton ok_button_;
    TtButton cancel_button_;
  };

  // -- NewProjectDialog -------------------------------------------------
  class NewProjectDialog : public TtDialog {
  public:
    explicit NewProjectDialog( void );

    virtual DWORD GetStyle( void ) override;
    virtual DWORD GetExtendedStyle( void ) override;

    virtual bool Created( void ) override;

    const std::string& GetPath( void );

  private:
    std::string path_;

    TtStatic file_label_;
    TtEdit   file_edit_;
    TtStatic folder_label_;
    TtEdit   folder_edit_;
    TtButton folder_ref_button_;
    TtButton ok_button_;
    TtButton cancel_button_;
  };

  // -- UserInputDialog --------------------------------------------------
  class UserInputDialog : public TtDialog {
  public:
    explicit UserInputDialog( const std::string& title = "" );

    virtual DWORD GetStyle( void ) override;
    virtual DWORD GetExtendedStyle( void ) override;

    virtual bool Created( void ) override;

    const std::string& GetTitle( void );
    void SetTitle( const std::string& title );

    const std::string& GetInput( void );

  private:
    std::string title_;
    std::string input_;

    TtEdit   edit_;
    TtButton ok_button_;
  };

  // -- ShortcutKeyDialog ------------------------------------------------
  class ShortcutKeyDialog : public TtDialog {
  public:
    explicit ShortcutKeyDialog( const std::string& title );

    virtual DWORD GetExtendedStyle( void ) override;

    virtual bool Created( void ) override;

    void SetKey( TtForm::AcceleratorMap::ShortcutKey key );
    TtForm::AcceleratorMap::ShortcutKey GetKey( void );

  private:
    std::string title_;
    TtForm::AcceleratorMap::ShortcutKey key_;

    TtEditWithStyle<TtEdit::Style::READONLY> edit_;
    TtButton    clear_button_;
    TtButton    ok_button_;
  };

  // -- ProjectVariablesDialog -------------------------------------------
  class ProjectVariablesDialog : public TtDialogModeless {
  public:
    explicit ProjectVariablesDialog( void );

    void SetShowFileVariable( bool flag );

  private:
    virtual DWORD  GetStyle( void ) override;
    virtual DWORD  GetExtendedStyle( void ) override;
    virtual bool Created( void ) override;

  private:
    bool show_file_variable_;

    using HelpList = TtWindowWithStyle<TtListViewReport, LVS_SINGLESEL | LVS_NOSORTHEADER | LVS_SHOWSELALWAYS>;
    HelpList list_;

    TtSubMenu menu_;
  };

  // -- StandardInputDialog ----------------------------------------------
  class StandardInputDialog : public TtDialogModeless {
  public:
    explicit StandardInputDialog( const std::string& title );

    void SetHandle( HANDLE input );

  private:
    virtual DWORD  GetStyle( void ) override;
    virtual DWORD  GetExtendedStyle( void ) override;
    virtual bool Created( void ) override;

  private:
    using LogEdit = TtEditWithStyle<TtEdit::Style::READONLY | TtEdit::Style::MULTILINE | WS_VSCROLL | WS_HSCROLL>;
    using InputEdit  = TtEdit;

    std::string title_;
    HANDLE      input_;

    LogEdit   log_edit_;
    InputEdit input_edit_;
  };
}
