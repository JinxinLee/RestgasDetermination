//--------------------------------------------------------------------------
// File and Version Information:
//      $Id: AstSTLJoinMaps.cc,v 1.1.1.1 2005/03/29 16:49:05 steinke Exp $
//
// Description:
//
//    Template object that joins two maps of types <T1, T2>
//    and <T2, T3> using the intermediate links as shortcuts.
//
//    Usage:
//      AstSTLMap2< T1, T2 > * m12; // get the maps ... 
//      AstSTLMap2< T2, T3 > * m23; // ... from somewhere
//      AstSTLJoinMaps< T1, T2, T3 > join;
//      AstSTLMap2< T1, T3 > * m13 = join( m12, m23 );
//
//    Pictorial view:
//
//      a1 <--> a2  <--> a3   ---\    a1 <--> a3
//      b1 <--> b2, <--> b3       \   b1 <--> b3,
//              c2  <--> c3       /           c3
//      c1 <--> d2  <--> d3,  ---/    c1 <--> d3,
//                       e3                   e3
//
// Author List:
//      Luca Lista  14 May 1999       
//
// Bertram Kopf (RUB)  migrated to PandaRoot
//------------------------------------------------------------------------

//-----------------------
// This Class's Header --
//-----------------------

#include "AstSTLJoinMaps.h"

//---------------
// C++ Headers --
//---------------

//-------------------------------
// Collaborating Class Headers --
//-------------------------------

#include <map>
using std::map;
#include <vector>
using std::vector;

#include "AstSTLMap2.h"

//-----------------------------------------------------------------------
// Local Macros, Typedefs, Structures, Unions and Forward Declarations --
//-----------------------------------------------------------------------


// Constructors

template<class T1, class T2, class T3>
AstSTLJoinMaps<T1, T2, T3>::AstSTLJoinMaps()
{}

// Destructor

template<class T1, class T2, class T3>
AstSTLJoinMaps<T1, T2, T3>::~AstSTLJoinMaps()
{}

// Operators

template<class T1, class T2, class T3>
AstSTLMap2<T1, T3>* 
AstSTLJoinMaps<T1, T2, T3>::operator()( AstSTLMap2<T1, T2>* m12,  AstSTLMap2<T2, T3>* m23 ) const
{
  AstSTLMap2<T1, T3> * m13;
  m13 = new AstSTLMap2<T1, T3>;
  map<T1*, vector<T2*> >& map12 = (map<T1*, vector<T2*> >& ) m12->map1();
  typename map<T1*, vector<T2*> >::iterator it12;

  for ( it12=map12.begin();it12!=map12.end();it12++ )
  {
    T1 * t1 = it12->first;

    vector<T2*> & v2 = map12[ t1 ];
    
    for ( size_t i = 0; i < v2.size(); i++ )
    {

      T2 * t2 = v2[ i ];
      vector<T3*> v3;
      m23->findValue1( t2, v3 );

      for ( size_t j = 0; j < v3.size(); j++ )
      {
	T3 * t3 = v3[ j ];
	if ( ! m13->contains( t1, t3 ) ){
	  m13->append( t1, t3 );
	}

      }
    }
  }
  
  return m13;
}


// Member functions

template<class T1, class T2, class T3>
AstSTLMap2<T1, T3>* 
AstSTLJoinMaps<T1, T2, T3>::joinMaps( AstSTLMap2<T1, T2>* m12,  AstSTLMap2<T2, T3>* m23 ) const
{
  return (*this)(m12, m23);
}
