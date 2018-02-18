// main_tree.h

#pragma once

#include "tt_form.h"
#include "tt_tree_view.h"

#include "common.h"
#include "project.h"
#include "settings.h"


namespace TTVCDeveloper {
  // -- MainTree ---------------------------------------------------------
  class MainTree : public TtWindowWithStyle<TtTreeView, TtTreeView::Style::HASBUTTONS | TtTreeView::Style::EDITLABELS | TtTreeView::Style::SHOWSELALWAYS | TtTreeView::Style::LINESATROOT> {
  public:
    using Item = TtTreeItemWith<Project::Element*>;

    explicit MainTree( Settings& settings );

    virtual bool CreatedInternal( void ) override;

    Item GetRoot( void );

    void Reset( Project::ProjectElement& project_element );
    void SetProject( Project::ProjectElement& project_element );

    void AddDirectory( Project::Directory& directory );
    void AddFile( Project::File& file );

    void UpSelected( void );
    void DownSelected( void );

    void EditSelected( void );

    Item InsertBySelectedItem( const std::string& text );
    void SetElementToItem( Item& item, Project::Element& element );

  private:
    Settings& settings_;
  };
}
