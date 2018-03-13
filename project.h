// project.h

#pragma once

#include <vector>
#include <memory>

#include "tt_text_template.h"
#include "tt_enum.h"

#include "common.h"



namespace TTVCDeveloper {
  class MainTree;

  // -- Project ----------------------------------------------------------
  class Project {
  public:
    // -- Element --------------------------------------------------------
    class Element {
    public:
      explicit Element( Project& project, Image::Index::Value original_icon_index );
      virtual ~Element();
      virtual int         GetTreeViewIconIndex( bool use_original );
      virtual std::string GetName( void ) const = 0;
      virtual std::string GetDataForFile( void ) const = 0;

      template <class TYPE>
      bool TypeIs( void ) {
        return dynamic_cast<TYPE*>( this ) != nullptr;
      }
      template <class TYPE>
      bool TypeIsNot( void ) {
        return NOT( this->TypeIs<TYPE>() );
      }

    protected:
      Project&            project_;
      Image::Index::Value original_icon_index_;
    };

    // -- ProjectElement -------------------------------------------------
    class ProjectElement : public Element {
    public:
      explicit ProjectElement( Project& project );

      void SetName( const std::string& name );
      virtual std::string GetName( void ) const override;
      virtual std::string GetDataForFile( void ) const override;

    protected:
      std::string name_;
    };

    // -- Directory ------------------------------------------------------
    class Directory : public Element {
    public:
      explicit Directory( Project& project, const std::string& name );

      void SetName( const std::string& name );
      virtual std::string GetName( void ) const override;
      virtual std::string GetDataForFile( void ) const override;

    protected:
      std::string name_;
    };

    // -- File -----------------------------------------------------------
    class File : public Element {
    public:
      explicit File( Project& project, Image::Index::Value original_icon_index, const std::string& path );

      void SetPath( const std::string& path );
      const std::string& GetPath( void ) const;

      virtual int         GetTreeViewIconIndex( bool use_original );
      virtual std::string GetName( void ) const override;
      virtual std::string GetDataForFile( void ) const override;

      std::string GetFullPath( void );

    protected:
      std::string path_;
    };

    // -- tool -----
    template <class TYPE, Image::Index::Value original_icon_index>
    class WithOriginalIconIndex : public TYPE {
    public:
      explicit WithOriginalIconIndex( Project& project, const std::string& path ) : TYPE( project, original_icon_index, path ) {}
    };

    // -- OtherFile ------------------------------------------------------
    class OtherFile : public File {
    public:
      explicit OtherFile( Project& project, const std::string& path );

      virtual int GetTreeViewIconIndex( bool use_original );
    };

    using HeaderFile = WithOriginalIconIndex<File, Image::Index::HeaderFile>;

    // -- SourceFile -----------------------------------------------------
    class SourceFile : public File {
    public:
      explicit SourceFile( Project& project, Image::Index::Value original_icon_index, const std::string& path );

      virtual std::string GetObjectFilePath( const std::string& output_directory_path );
    };

    using CFile   = WithOriginalIconIndex<SourceFile, Image::Index::CFile>;
    using CppFile = WithOriginalIconIndex<SourceFile, Image::Index::CppFile>;

    // -- ResourceScriptFile ---------------------------------------------
    class ResourceScriptFile : public SourceFile {
    public:
      explicit ResourceScriptFile( Project& project, const std::string& path );

      virtual std::string GetObjectFilePath( const std::string& output_directory_path ) override;
    };

    // -- Enums -----
    enum class TargetKind {
      Windows,
      Console,
      DynamicLinkLibrary,
      StaticLibrary,
    };

    enum class OptimizeKind {
      NoSpecify,
      NoOptimize,
      Smallest,
      Fastest,
      Full,
    };

    enum class RuntimeLibrary {
      Static,
      StaticDebug,
      Dynamic,
      DynamicDebug,
    };

    // -- Structure ------------------------------------------------------
    struct Structure {
      explicit Structure( void );

      std::string              name_;
      // -- Application
      TtEnum<Platform>         platform_;
      TtEnum<TargetKind>       target_kind_;
      std::string              output_directory_name_;
      std::string              target_name_;
      std::string              target_argument_;
      std::string              target_current_directory_;
      bool                     target_use_output_edit_;
      bool                     target_use_input_dialog_;
      // -- Compile
      TtEnum<OptimizeKind>     optimize_kind_;
      TtEnum<RuntimeLibrary>   runtime_library_;
      bool                     use_exception_;
      // -- PrecompiledHeader
      bool                     use_precompile_header_;
      std::string              precompiled_header_source_;
      std::string              precompiled_header_header_;
      bool                     precompiled_header_force_include_;
      // -- Compile
      std::vector<std::string> include_paths_;
      std::vector<std::string> defines_;
      // -- Warnings
      unsigned int             warning_level_;
      std::vector<int>         no_warnings_;
      bool                     error_if_warning_;
      // -- Link
      std::vector<std::string> library_paths_;
      // -- Other
      bool                     no_logo_;
      std::string              compiler_other_option_;
      std::string              resource_compiler_option_;
      std::string              extension_script_path_;
    };

    // -- MakeCommandEntry -----------------------------------------------
    struct MakeCommandEntry {
      std::string              target_;
      bool                     included_in_build_;
      bool                     included_in_clean_;
      std::vector<std::string> depends_;
      std::vector<std::string> commands_;
    };

    // -- Project --------------------------------------------------------
    explicit Project( void );
    void Reset( const std::string& path );

    ProjectElement&                          GetProjectElement( void );
    std::vector<std::shared_ptr<Directory>>& GetDirectories( void );
    std::vector<std::shared_ptr<File>>&      GetFiles( void );
    std::vector<std::shared_ptr<Structure>>& GetStructures( void );

    std::string ConvertRelativePath( const std::string& target );
    std::string ConvertAbsolutePath( const std::string& target );

    std::string GetIconAbsolutePath( void );

    Directory& AddDirectory( const std::string& name );
    File& AddFile( const std::string& path );

    void EachFileWithIndex( std::function<void ( File& file, int index )> function );

  public:
    void SaveToFile( MainTree& tree );
    void SaveToFileAsDefault( void );
  private:
    void SaveToFileTree( MainTree& tree );
    void SaveToFileStructures( void );

  public:
    void LoadFromFile( MainTree& tree );
    void LoadFromFileAsDefault( void );
  private:
    void LoadFromFileTree( MainTree& tree );
    void LoadFromFileStructures( void );


  public:
    std::string path_;
    std::string name_;

    bool         use_icon_;
    std::string  icon_file_path_;
    unsigned int icon_file_index_;

  private:
    ProjectElement                          project_element_;
    std::vector<std::shared_ptr<Directory>> directories_;
    std::vector<std::shared_ptr<File>>      files_;

    std::vector<std::shared_ptr<Structure>> structures_;
  };
}
