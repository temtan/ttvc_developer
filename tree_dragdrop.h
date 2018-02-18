// tree_dragdrop.h

#pragma once

#include <vector>
#include <algorithm>
#include <string>

#include "tt_windows_h_include.h"

#include "ttl_define.h"

#include "main_tree.h"


namespace TTVCDeveloper {
  // -- IUnknownImplement ----------------------------------------------
  class IUnknownImplement {
  protected:
    using Table = std::vector<std::pair<IID, void*>>;

    explicit IUnknownImplement( Table& table ) : reference_counter_( 1 ), pointer_table_( table ) {}

    STDMETHODIMP QueryInterface( REFIID riid, void **ppvObject ) {
      *ppvObject = NULL;
      auto it = std::find_if( pointer_table_.begin(), pointer_table_.end(), [&riid] ( std::pair<IID, void*>& tmp ) -> bool {
        return ::IsEqualIID( riid, tmp.first ) ? true : false;
      } );
      if ( it != pointer_table_.end() ) {
        *ppvObject = it->second;
      }
      else {
        return E_NOINTERFACE;
      }
      this->AddRef();
      return S_OK;
    }

    STDMETHODIMP_( ULONG ) AddRef( void ) {
      return ::InterlockedIncrement( &reference_counter_ );
    }

    template <class PARENT>
    STDMETHODIMP_( ULONG ) Release( PARENT* self ) {
      if ( ::InterlockedDecrement( &reference_counter_ ) == 0 ) {
        delete self;
        return 0;
      }
      return reference_counter_;
    }

  protected:
    LONG  reference_counter_;
    Table pointer_table_;
  };

  // -- TreeDropSource -------------------------------------------------
  class TreeDropSource : public IDropSource,
                         private IUnknownImplement {
  public:
    explicit TreeDropSource( void );
    ~TreeDropSource();

    STDMETHODIMP QueryInterface( REFIID riid, void **ppvObject );
    STDMETHODIMP_( ULONG ) AddRef( void );
    STDMETHODIMP_( ULONG ) Release( void );

    STDMETHODIMP QueryContinueDrag( BOOL fEscapePressed, DWORD grfKeyState );
    STDMETHODIMP GiveFeedback( DWORD dwEffect );
  };

  // -- TreeDataObject -------------------------------------------------
  class TreeDataObject : public IDataObject,
                         public IEnumFORMATETC,
                         private IUnknownImplement {
  public:
    explicit TreeDataObject( void );
    ~TreeDataObject();

    void SetDataAsPath( const std::string& path );
    void ClearData( void );

    STDMETHODIMP QueryInterface( REFIID riid, void **ppvObject );
    STDMETHODIMP_( ULONG ) AddRef( void );
    STDMETHODIMP_( ULONG ) Release( void );

    STDMETHODIMP GetData( FORMATETC *pformatetcIn, STGMEDIUM *pmedium );
    STDMETHODIMP GetDataHere( FORMATETC *pformatetc, STGMEDIUM *pmedium );
    STDMETHODIMP QueryGetData( FORMATETC *pformatetc );
    STDMETHODIMP GetCanonicalFormatEtc( FORMATETC *pformatectIn, FORMATETC *pformatetcOut );
    STDMETHODIMP SetData( FORMATETC *pformatetc, STGMEDIUM *pmedium, BOOL fRelease );
    STDMETHODIMP EnumFormatEtc( DWORD dwDirection, IEnumFORMATETC **ppenumFormatEtc );
    STDMETHODIMP DAdvise( FORMATETC *pformatetc, DWORD advf, IAdviseSink *pAdvSink, DWORD *pdwConnection );
    STDMETHODIMP DUnadvise( DWORD dwConnection );
    STDMETHODIMP EnumDAdvise( IEnumSTATDATA **ppenumAdvise );

    STDMETHODIMP Next( ULONG celt, FORMATETC *rgelt, ULONG *pceltFetched );
    STDMETHODIMP Skip( ULONG celt );
    STDMETHODIMP Reset( void );
    STDMETHODIMP Clone( IEnumFORMATETC **ppenum );

  private:
    ULONG   enumerate_counter_;
    HGLOBAL global_memory_handle_;
    UINT    cf_tree_item_type_;
  };

  // -- TreeDropTarget -------------------------------------------------
  class TreeDropTarget : public IDropTarget,
                         private IUnknownImplement {
  public:
    explicit TreeDropTarget( MainTree& tree, MainTree::Item::DragHandler& drag_handler );
    ~TreeDropTarget();

    void SetDropHandler( std::function<void ( void )> handler );

    STDMETHODIMP QueryInterface( REFIID riid, void **ppvObject );
    STDMETHODIMP_( ULONG ) AddRef( void );
    STDMETHODIMP_( ULONG ) Release( void );

    STDMETHODIMP DragEnter( IDataObject *pDataObj, DWORD grfKeyState, POINTL pt, DWORD *pdwEffect );
    STDMETHODIMP DragOver( DWORD grfKeyState, POINTL pt, DWORD *pdwEffect );
    STDMETHODIMP DragLeave( void );
    STDMETHODIMP Drop( IDataObject *pDataObj, DWORD grfKeyState, POINTL pt, DWORD *pdwEffect );

  private:
    MainTree&                    tree_;
    MainTree::Item::DragHandler& drag_handler_;
    UINT                         cf_tree_item_type_;
    std::function<void ( void )> drop_handler_;
  };
}
