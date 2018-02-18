// main_tree.cpp

#include "ttl_define.h"
#include "tt_message_box.h"

#include "main_tree.h"

using namespace TTVCDeveloper;


// -- MainTree -----------------------------------------------------------
MainTree::MainTree( Settings& settings ) :
settings_( settings )
{
}


bool
MainTree::CreatedInternal( void )
{
  this->SetImageList( *Image::LIST );
  this->SetItemHeight( this->GetItemHeight() + 2 );
  return true;
}

MainTree::Item
MainTree::GetRoot( void )
{
  return this->GetOrigin().GetFirstChild();
}


void
MainTree::Reset( Project::ProjectElement& project_element )
{
  this->Clear();
  this->GetOrigin().AddChildFirst();
  this->SetProject( project_element );
}

void
MainTree::SetProject( Project::ProjectElement& project_element )
{
  this->SetElementToItem( this->GetRoot(), project_element );
}


void
MainTree::AddDirectory( Project::Directory& directory )
{
  MainTree::Item target = this->InsertBySelectedItem( directory.GetName() );
  this->SetElementToItem( target, directory );
  target.SetSelect();
}

void
MainTree::AddFile( Project::File& file )
{
  MainTree::Item target = this->InsertBySelectedItem( file.GetName() );
  this->SetElementToItem( target, file );
  target.SetSelect();
}


void
MainTree::UpSelected( void )
{
  MainTree::Item selected = this->GetSelected();
  if ( selected.IsInvalid() || selected == this->GetRoot() ) {
    return;
  }

  MainTree::Item prev = selected.GetPrevSibling();
  if ( prev.IsInvalid() ) {
    if ( selected.GetParent() == this->GetRoot() ) {
      return;
    }
    MainTree::Item moved = selected.MoveToPrevious( selected.GetParent().GetParent(), selected.GetParent() );
    moved.SetSelect();
    return;
  }
  else {
    if ( prev.GetParameter()->TypeIs<Project::Directory>() && prev.IsExpanded() ) {
      MainTree::Item moved = selected.MoveToLast( prev );
      moved.SetSelect();
      return;
    }
    MainTree::Item moved = selected.MoveToPrevious( selected.GetParent(), prev );
    moved.SetSelect();
    return;
  }
}


void
MainTree::DownSelected( void )
{
  MainTree::Item selected = this->GetSelected();
  if ( selected.IsInvalid() || selected == this->GetRoot() ) {
    return;
  }

  MainTree::Item next = selected.GetNextSibling();
  if ( next.IsInvalid() ) {
    if ( selected.GetParent() == this->GetRoot() ) {
      return;
    }
    MainTree::Item moved = selected.MoveToAfter( selected.GetParent().GetParent(), selected.GetParent() );
    moved.SetSelect();
    return;
  }
  else {
    if ( next.GetParameter()->TypeIs<Project::Directory>() && next.IsExpanded() ) {
      MainTree::Item moved = selected.MoveToFirst( next );
      moved.SetSelect();
      return;
    }
    MainTree::Item moved = selected.MoveToAfter( selected.GetParent(), next );
    moved.SetSelect();
    return;
  }
}


void
MainTree::EditSelected( void )
{
  MainTree::Item selected = this->GetSelected();
  if ( selected.IsInvalid() || selected == this->GetRoot() ) {
    return;
  }
  selected.SetEditMode();
}



MainTree::Item
MainTree::InsertBySelectedItem( const std::string& text )
{
  MainTree::Item selected = this->GetSelected();
  if ( selected.IsInvalid() || selected.IsOrigin() || selected == this->GetRoot() ) {
    return this->GetRoot().AddChildFirst( text );
  }
  else if ( selected.GetParameter()->TypeIs<Project::Directory>() ) {
    return selected.AddChildFirst( text );
  }
  return selected.GetParent().AddChildAfter( selected );
}


void
MainTree::SetElementToItem( MainTree::Item& item, Project::Element& element )
{
  item.SetParameter( &element );
  item.SetText( element.GetName() );
  item.SetBothImageIndex( element.GetTreeViewIconIndex( settings_.tree_use_original_icon_ ) );
}
