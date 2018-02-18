// tree_dragdrop.cpp

#pragma warning(push)
#  pragma warning(disable : 4005)
#  pragma warning(disable : 4917)
#  include <Shlobj.h>
#pragma warning(pop)

#include "tree_dragdrop.h"

using namespace TTVCDeveloper;


// -- TreeDropSource -----------------------------------------------------
TreeDropSource::TreeDropSource( void ) :
IUnknownImplement( IUnknownImplement::Table {
  { IID_IUnknown,    static_cast<IDropSource*>( this ) },
  { IID_IDropSource, static_cast<IDropSource*>( this ) }
} )
{
}

TreeDropSource::~TreeDropSource()
{
}


STDMETHODIMP
TreeDropSource::QueryInterface( REFIID riid, void **ppvObject )
{
  return this->IUnknownImplement::QueryInterface( riid, ppvObject );
}

STDMETHODIMP_( ULONG )
TreeDropSource::AddRef( void )
{
  return this->IUnknownImplement::AddRef();
}

STDMETHODIMP_( ULONG )
TreeDropSource::Release( void )
{
  return this->IUnknownImplement::Release( this );
}


STDMETHODIMP
TreeDropSource::QueryContinueDrag( BOOL fEscapePressed, DWORD grfKeyState )
{
  if ( fEscapePressed ) {
    return DRAGDROP_S_CANCEL;
  }
  if ( (grfKeyState & MK_LBUTTON) == 0 ) {
    return DRAGDROP_S_DROP;
  }
  return S_OK;
}

STDMETHODIMP
TreeDropSource::GiveFeedback( DWORD dwEffect )
{
  NOT_USE( dwEffect );
  return DRAGDROP_S_USEDEFAULTCURSORS;
}


// -- TreeDataObject -----------------------------------------------------
TreeDataObject::TreeDataObject( void ) :
IUnknownImplement( IUnknownImplement::Table {
  { IID_IUnknown,       static_cast<IDataObject*>( this ) },
  { IID_IDataObject,    static_cast<IDataObject*>( this ) },
  { IID_IEnumFORMATETC, static_cast<IEnumFORMATETC*>( this ) }
} ),
enumerate_counter_( 0 ),
global_memory_handle_( NULL ),
cf_tree_item_type_( ::RegisterClipboardFormat( "TTVC_TREE_ITEM" ) )
{
}

TreeDataObject::~TreeDataObject()
{
  if ( global_memory_handle_ ) {
    ::GlobalFree( global_memory_handle_ );
  }
}


void
TreeDataObject::SetDataAsPath( const std::string& path )
{
  FORMATETC formatetc = {CF_HDROP, NULL, DVASPECT_CONTENT, -1, TYMED_HGLOBAL};

  STGMEDIUM medium;
  medium.tymed          = TYMED_HGLOBAL;
  medium.hGlobal        = ::GlobalAlloc( GHND, sizeof( DROPFILES ) + path.size() + 2 );
  medium.pUnkForRelease = NULL;

  DROPFILES* p = static_cast<DROPFILES*>( ::GlobalLock( medium.hGlobal ) );
  p->pFiles = sizeof( DROPFILES );
  p->pt.x = 0;
  p->pt.y = 0;
  p->fNC = FALSE;
  p->fWide  = FALSE;
  ::CopyMemory( p + 1, path.c_str(), path.size() + 1 );
  ::GlobalUnlock( medium.hGlobal );

  this->SetData( &formatetc, &medium, TRUE );
}


void
TreeDataObject::ClearData( void )
{
  if ( global_memory_handle_ ) {
    ::GlobalFree( global_memory_handle_ );
    global_memory_handle_ = NULL;
  }
}


STDMETHODIMP
TreeDataObject::QueryInterface( REFIID riid, void **ppvObject )
{
  return this->IUnknownImplement::QueryInterface( riid, ppvObject );
}

STDMETHODIMP_( ULONG )
TreeDataObject::AddRef( void )
{
  return this->IUnknownImplement::AddRef();
}

STDMETHODIMP_( ULONG )
TreeDataObject::Release( void )
{
  return this->IUnknownImplement::Release( this );
}


STDMETHODIMP
TreeDataObject::GetData( FORMATETC *pformatetcIn, STGMEDIUM *pmedium )
{
  if ( global_memory_handle_ != NULL && pformatetcIn->cfFormat == CF_HDROP ) {
    pmedium->tymed = TYMED_HGLOBAL;
    pmedium->hGlobal = (HGLOBAL)::OleDuplicateData( global_memory_handle_, CF_HDROP, (UINT)NULL );
    pmedium->pUnkForRelease = NULL;
  }
  else if ( pformatetcIn->cfFormat == cf_tree_item_type_ ) {
    pmedium->tymed = TYMED_NULL;
    pmedium->pUnkForRelease = NULL;
  }
  else {
    return E_FAIL;
  }
  return S_OK;
}

STDMETHODIMP
TreeDataObject::GetDataHere( FORMATETC *pformatetc, STGMEDIUM *pmedium )
{
  NOT_USE( pformatetc );
  NOT_USE( pmedium );
  return E_NOTIMPL;
}

STDMETHODIMP
TreeDataObject::QueryGetData( FORMATETC *pformatetc )
{
  if ( global_memory_handle_ != NULL && pformatetc->cfFormat == CF_HDROP ) {
    return S_OK;
  }
  if ( pformatetc->cfFormat == cf_tree_item_type_ ) {
    return S_OK;
  }
  return S_FALSE;
}

STDMETHODIMP
TreeDataObject::GetCanonicalFormatEtc( FORMATETC *pformatectIn, FORMATETC *pformatetcOut )
{
  NOT_USE( pformatectIn );
  NOT_USE( pformatetcOut );
  return E_NOTIMPL;
}

STDMETHODIMP
TreeDataObject::SetData( FORMATETC *pformatetc, STGMEDIUM *pmedium, BOOL fRelease )
{
  if ( pformatetc->cfFormat == CF_HDROP ) {
    if ( global_memory_handle_ != NULL ) {
      ::GlobalFree( global_memory_handle_ );
      global_memory_handle_ = NULL;
    }

    global_memory_handle_ = (HGLOBAL)::OleDuplicateData( pmedium->hGlobal, CF_HDROP, (UINT)NULL );

    if ( fRelease ) {
      ::GlobalFree( pmedium->hGlobal );
    }
  }
  else if ( pformatetc->cfFormat == cf_tree_item_type_ ) {
    // “Á‚É‚â‚é‚±‚Æ–³‚µ
  }
  else {
    return E_FAIL;
  }

  return S_OK;
}

STDMETHODIMP
TreeDataObject::EnumFormatEtc( DWORD dwDirection, IEnumFORMATETC **ppenumFormatEtc )
{
  if ( dwDirection == DATADIR_GET ) {
    return this->QueryInterface( IID_PPV_ARGS( ppenumFormatEtc ) );
  }
  else {
    return E_NOTIMPL;
  }
}

STDMETHODIMP
TreeDataObject::DAdvise( FORMATETC *pformatetc, DWORD advf, IAdviseSink *pAdvSink, DWORD *pdwConnection )
{
  NOT_USE( pformatetc );
  NOT_USE( advf );
  NOT_USE( pAdvSink );
  NOT_USE( pdwConnection );
  return E_NOTIMPL;
}

STDMETHODIMP
TreeDataObject::DUnadvise( DWORD dwConnection )
{
  NOT_USE( dwConnection );
  return E_NOTIMPL;
}

STDMETHODIMP
TreeDataObject::EnumDAdvise( IEnumSTATDATA **ppenumAdvise )
{
  NOT_USE( ppenumAdvise );
  return E_NOTIMPL;
}


STDMETHODIMP
TreeDataObject::Next( ULONG celt, FORMATETC *rgelt, ULONG *pceltFetched )
{
  NOT_USE( celt );

  std::vector<FORMATETC> v = {
    {static_cast<CLIPFORMAT>( cf_tree_item_type_ ), NULL, DVASPECT_CONTENT, -1, TYMED_NULL}
  };
  if ( global_memory_handle_ != NULL ) {
    v.push_back( {CF_HDROP, NULL, DVASPECT_CONTENT, -1, TYMED_HGLOBAL} );
  }

  if ( enumerate_counter_ >= v.size() ) {
    return S_FALSE;
  }
  *rgelt = v[enumerate_counter_];
  if ( pceltFetched != NULL ) {
    *pceltFetched = 1;
  }

  enumerate_counter_++;
  return S_OK;
}

STDMETHODIMP
TreeDataObject::Skip( ULONG celt )
{
  NOT_USE( celt );
  return E_NOTIMPL;
}

STDMETHODIMP
TreeDataObject::Reset( void )
{
  enumerate_counter_ = 0;
  return S_OK;
}

STDMETHODIMP
TreeDataObject::Clone( IEnumFORMATETC **ppenum )
{
  NOT_USE( ppenum );
  return E_NOTIMPL;
}


// -- TreeDropTarget -----------------------------------------------------
TreeDropTarget::TreeDropTarget( MainTree& tree, MainTree::Item::DragHandler& drag_handler ) :
IUnknownImplement( IUnknownImplement::Table {
  { IID_IUnknown,    static_cast<IDropTarget*>( this ) },
  { IID_IDropTarget, static_cast<IDropTarget*>( this ) },
} ),
tree_( tree ),
drag_handler_( drag_handler ),
cf_tree_item_type_( ::RegisterClipboardFormat( "TTVC_TREE_ITEM" ) ),
drop_handler_( nullptr )
{
}

TreeDropTarget::~TreeDropTarget()
{
}

void
TreeDropTarget::SetDropHandler( std::function<void ( void )> handler )
{
  drop_handler_ = handler;
}


STDMETHODIMP
TreeDropTarget::QueryInterface(REFIID riid, void **ppvObject)
{
  return this->IUnknownImplement::QueryInterface( riid, ppvObject );
}

STDMETHODIMP_( ULONG )
TreeDropTarget::AddRef()
{
  return this->IUnknownImplement::AddRef();
}

STDMETHODIMP_(ULONG)
TreeDropTarget::Release()
{
  return this->IUnknownImplement::Release( this );
}


STDMETHODIMP
TreeDropTarget::DragEnter( IDataObject *pDataObj, DWORD grfKeyState, POINTL pt, DWORD *pdwEffect )
{
  NOT_USE( grfKeyState );

  FORMATETC formatetc = {static_cast<CLIPFORMAT>( cf_tree_item_type_ ), NULL, DVASPECT_CONTENT, -1, TYMED_NULL};
  if ( pDataObj->QueryGetData( &formatetc ) == S_OK && drag_handler_.IsBegun() ) {
    *pdwEffect = DROPEFFECT_MOVE;
    drag_handler_.Enter( tree_, pt.x, pt.y );
  }
  else {
    *pdwEffect = DROPEFFECT_NONE;
  }
  return S_OK;
}

STDMETHODIMP
TreeDropTarget::DragOver( DWORD grfKeyState, POINTL pt, DWORD *pdwEffect )
{
  NOT_USE( grfKeyState );

  POINT current = {pt.x - tree_.GetPoint().x, pt.y - tree_.GetPoint().y};
  if ( drag_handler_.IsEntered() ) {
    *pdwEffect = DROPEFFECT_MOVE;
    MainTree::Item hit_item = tree_.HitTest( current.x, current.y );
    if ( hit_item.IsValid() && NOT( hit_item.IsDropHilighted() ) ) {
      drag_handler_.ReEnter( current.x, current.y, [&] ( void ) {
        tree_.ClearDropTarget();
        hit_item.SetSelectAsDropTarget();
      } );
    }
    else {
      drag_handler_.Move( current.x, current.y );
    }
  }
  else {
    *pdwEffect = DROPEFFECT_NONE;
  }
  return S_OK;
}

STDMETHODIMP
TreeDropTarget::DragLeave( void )
{
  if ( drag_handler_.IsEntered() ) {
    drag_handler_.Leave();
  }
  return S_OK;
}

STDMETHODIMP
TreeDropTarget::Drop( IDataObject *pDataObj, DWORD grfKeyState, POINTL pt, DWORD *pdwEffect )
{
  NOT_USE( pt );
  NOT_USE( grfKeyState );

  FORMATETC formatetc = {static_cast<CLIPFORMAT>( cf_tree_item_type_ ), NULL, DVASPECT_CONTENT, -1, TYMED_NULL};
  STGMEDIUM medium;
  if ( FAILED( pDataObj->GetData( &formatetc, &medium ) ) ) {
    *pdwEffect = DROPEFFECT_NONE;
    return E_FAIL;
  }
  *pdwEffect = DROPEFFECT_MOVE;

  if ( drop_handler_ ) {
    drop_handler_();
  }
  return S_OK;
}

