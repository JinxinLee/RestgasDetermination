//--------------------------------------------------------------------------
// File and Version Information:
//    $Id: IfdKey.cxx,v 1.1.1.1 2005/03/29 17:13:30 steinke Exp $
//
// Description:
//
// Author List:
//    Ed Frank                        University of Pennsylvania
//
// History:
//    Ed Frank      17 Nov 96         Creation of first version
//
// Bugs:
//
//      Bertram Kopf (RUB) migrated to PandaRoot
//------------------------------------------------------------------------
#include "IfdKey.h"
using std::ostream;

IfdKey::IfdKey( keyKind kind )
  : _hashVal( 0 )
  , _myKeyKind( kind ) 
  , _myCardinality( 0 )
{}

IfdKey::~IfdKey()
{}


void
IfdKey::add( const IfdKey& ) {
  // only makes sense for the composite class.

  assert( 0 );
}

void
IfdKey::make_vtab( void ) const {
  // bogus virtual non-inline routine to get vtab emitted.
}

ostream& operator<<( ostream &o, const IfdKey & k ) {
    k.print(o);
    return o;
}
