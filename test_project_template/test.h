// test.h
#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string>
#include <memory>
#include <functional>

#pragma warning(push)
#  pragma warning( disable: 4005 )
#  define NOMINMAX
#  include <windows.h>
#pragma warning(pop)

#include <time.h>

#define FILENAME__       (__FILE__ + std::string( __FILE__ ).find_last_of( '\\' ) + 1)
#define PCD()            (printf( "%s(%d) : gone.\n", FILENAME__, __LINE__ ), fflush( stdout ))
#define PCA( type, obj ) (printf( "%s(%d) : %s = " type "\n", FILENAME__, __LINE__, #obj, (obj) ), fflush( stdout ))
#define PCC( c )         PCA( "%c", c )
#define PCI( i )         PCA( "%d", i )
#define PCP( p )         PCA( "%p", p )
#define PCS( s )         PCA( "%s", s )
#define PCF( f )         PCA( "%f", f )
#define PCSS( s )        (printf( "%s(%d) : %s = %s\n", FILENAME__, __LINE__, #s, (s).c_str() ), fflush( stdout ))


// -- PerformanceCounter -------------------------------------------------
class PerformanceCounter {
public:
  using Function = std::function<void ( void )>;

  static LONGLONG Count( Function function ) { return PerformanceCounter::Count( 1, function ); }
  static LONGLONG Count( unsigned int loop_count, Function function ) {
    PerformanceCounter counter;
    counter.Start();
    for ( unsigned int i = 0; i < loop_count; ++i ) {
      function();
    }
    return counter.GetElapsed();
  }

public:
  explicit PerformanceCounter( bool auto_start = false ) : start_( {0} ) {
    if ( auto_start ) {
      this->Start();
    }
  }

  void Start( void ) {
    ::QueryPerformanceCounter( &start_ );
  }

  LONGLONG GetElapsed( void ) {
    LARGE_INTEGER tmp;
    ::QueryPerformanceCounter( &tmp );
    return tmp.QuadPart - start_.QuadPart;
  }

private:
  LARGE_INTEGER start_;
};
