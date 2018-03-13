// settings.h

#pragma once

#include <string>

#include "tt_ini_file.h"
#include "tt_form.h"

#include "common.h"

namespace TTVCDeveloper {
  class Settings {
  public:
    struct Compiler;

    class ExternalProgram {
    public:
      ExternalProgram( void );

      std::string name_;
      bool        for_open_file_;
      std::string path_;
      std::string argument_;
      std::string current_directory_;
      bool        use_output_edit_;
      bool        use_input_dialog_;
    };

    // -- constructor
    explicit Settings( void );

  public:
    Compiler& GetCompilerByPlatform( Platform platform );

  public:
    void ReadFromFile( void );
    void ReadFromFile( const std::string& filename );
  private:
    void ReadFromIniFile( TtIniFile& ini_file );

  public:
    void WriteToFile( void ) const;
    void WriteToFile( const std::string& filename ) const;
  private:
    void WriteToIniFile( TtIniFile& ini_file ) const;

  public:
    using ShortcutKey = TtForm::AcceleratorMap::ShortcutKey;

    struct Compiler {
      std::string GetLinkerCommandPath( void );
      std::string GetLibCommandPath( void );

      std::string make_command_path_;
      std::string compiler_path_;
      std::string resource_compiler_path_;
      std::string environment_variable_include_;
      std::string environment_variable_lib_;
      std::string environment_variable_libpath_;
    };

    // コンパイラ
    Compiler compiler_x86_;
    Compiler compiler_x64_;

    // エディタ
    std::string editor_path_;
    std::string editor_argument_;

    // 表示
    bool tree_use_original_icon_;

    // ショートカット
    ShortcutKey compile_key_;
    ShortcutKey build_key_;
    ShortcutKey rebuild_key_;
    ShortcutKey execute_key_;

    // 外部プログラム
    std::vector<ExternalProgram> external_programs_;

    // その他
    std::string extension_script_path_;

    // 隠し
    unsigned int project_history_count_max_;
  };
}
