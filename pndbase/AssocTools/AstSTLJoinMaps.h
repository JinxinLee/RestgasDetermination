//--------------------------------------------------------------------------
// File and Version Information:
//      $Id: AstSTLJoinMaps.hh,v 1.1.1.1 2005/03/29 16:49:05 steinke Exp $
//
// Description:
//
//    Template object that joins two maps of types <T1, T2>
//    and <T2, T3> using the intermediate links as shortcuts.
//
//    Usage:
//      AstMap2< T1, T2 > * m12; // get the maps ... 
//      AstMap2< T2, T3 > * m23; // ... from somewhere
//      AstSTLJoinMaps< T1, T2, T3 > join;
//      AstMap2< T1, T3 > * m13 = join( m12, m23 );
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
#ifndef ASTJOINMAPS_HH
#define ASTJOINMAPS_HH

//-------------
// C Headers --
//-------------
extern "C" {
}

//---------------
// C++ Headers --
//---------------

//----------------------
// Base Class Headers --
//----------------------

//-------------------------------
// Collaborating Class Headers --
//-------------------------------

//------------------------------------
// Collaborating Class Declarations --
//------------------------------------

template<class T1, class T2>
class AstSTLMap2;

//		---------------------
// 		-- Class Interface --
//		---------------------

template<class T1, class T2, class T3>
class AstSTLJoinMaps
{

public:

  // Constructors
  
  AstSTLJoinMaps();

  // Destructor

  virtual ~AstSTLJoinMaps();
  
  // Member function
  // Has same functionality as operator()
  
  AstSTLMap2<T1, T3>* joinMaps(AstSTLMap2<T1, T2>*, AstSTLMap2<T2, T3>* ) const;

  // Operator 

  AstSTLMap2<T1, T3>* operator()( AstSTLMap2<T1, T2>*, AstSTLMap2<T2, T3>* ) const;

private:

};

#include  "AstSTLJoinMaps.cxx"

#endif
