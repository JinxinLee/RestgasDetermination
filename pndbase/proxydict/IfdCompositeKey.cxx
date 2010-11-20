//--------------------------------------------------------------------------
// File and Version Information:
//    $Id: IfdCompositeKey.cxx,v 1.1.1.1 2005/03/29 17:13:30 steinke Exp $
//
// Description:
//
// Author List:
//    Ed Frank                        University of Pennsylvania
//
// History:
//    Ed Frank      20 Nov 96         Creation of first version
//    Ed Frank      29 Nov 96         Add NO_INLINE_COPY_CTOR
//    Ed Frank      13 Feb 96         Add NO_INLINE_COPY_CTOR removed
//
// CPP MACROS
//    NO_INLINE_COPY_CTOR      Turns off inlining of copy ctor.  HP can not
//                             support the for() in an inline right now.
//
//      Bertram Kopf (RUB) migrated to PandaRoot
//------------------------------------------------------------------------


#include <stdlib.h>
#include "IfdCompositeKey.h"

IfdCompositeKey::IfdCompositeKey() 
   : IfdKey( compositeKey )
{
  // _hashVal init'd to 0 by the base class.
  // _myCardinality init'd to 0 by base class.

}


IfdCompositeKey::~IfdCompositeKey() {
   // Composite Keys own all of their children.  Also, all of the
   // children were set via Key::clone(), so we *know* all of the
   // children live in the heap vs., say, the stack or bss.  Thus,
   // we *know* we can call delete on them.  Or, at least we think
   //  we know that.
   //

   // WARNING:  if you change this to not key off of _myCardinality,
   // then you must clean up the hack in IfdDictKey::~IfdDictKey.

   for( int i=0; i < _myCardinality; i++ ) {
      delete const_cast<IfdKey*>(  kid[i] );
   }
}


int
IfdCompositeKey::operator==( const IfdKey& k ) const {	
  // This is a little tricky.  Unlike the normal composite pattern
  // where the composite class passes a message on to each of it's
  // children, op==() is a *binary* op and we must apply it pairwise,
  // i.e., op==(our kids, k's kids).  We'll have to do a cast, to gain
  // .cxxess to k's kids.  This is because k is passed as a Key, and
  // not as a IfdCompositeKey.  The cast is made safe via getkeyKind().
  // An alternative to casting is to move the composite machinery up
  // into the base Key class, but Key's shouldn't know about this
  // stuff, so I chose to cast.  Should be safe.  Maybe.  Hah!
  //

   if ( _hashVal != k._hashVal )           return 0;
   if ( compositeKey != k.getKeyKind() )   return 0;
   if ( _myCardinality != k.cardinality()) return 0;
       // Part of the *definition* of equality for composite keys.

       
   const IfdKey** itsKid = ((IfdCompositeKey*)&k)->kid;
   for( int i=0; i<_myCardinality; i++ ) {       // The rest of the definiton

       if ( *kid[i] != *itsKid[i] ) return 0;  // of equality.

       // NB:  I tried unrolling this loop by hand.  It was
       // no faster than the for loop, surprisingly.  Time must
       // be in the ptr arithmetic?
   }

   return 1;
}

void
IfdCompositeKey::print( ostream &o ) const {
    o << "IfdCompositeKey[ ";
    for( int i=0; i<cardinality(); i++) {
	kid[i]->print( o );
	o << " ";
    }
    o << " ]";
}

IfdCompositeKey::IfdCompositeKey( const IfdCompositeKey& k )
   // This copy ctor is private for no good reason
   // except that I can't decide what the right thing to do
   // is.  Note that this copy ctor exists to make clone()
   // easier.
   //

   : IfdKey( compositeKey )

{
   _myCardinality =  k.cardinality();

   int i;
   for( i=0; i < cardinality(); i++) {
      kid[i] = k.kid[i]->clone();
   }
   _hashVal = k._hashVal;
}

void
IfdCompositeKey::add( const IfdKey& k ) {
   // The Key::clone() method makes add() possible.  Since we will take
   // ownership of the cloned objects, we require clone() to produce them
   // on the heap so that ~IfdCompositeKey can safely call their dtors;
   // otherwise,
   // memory management is hopeless.
   //

   // If we have room left to take another child...
   //
   if ( _myCardinality > maxChildren ) {
     cerr << "IfdCompositeKey::add() no more room.  Go shoot Ed."
          << endl;
     ::exit( 1 );
   }

   // Then make a copy and keep it
   //
   kid[ _myCardinality ] = k.clone();
   if ( ! kid[ _myCardinality ] ) {
      cerr << "IfdCompositeKey::add() failed to allocate a key (clone failed)"
          << endl;
      ::exit( 1 );
   }

   // Update the hash value.

   _hashVal = ( (_hashVal << 8 ) + k._hashVal ) % _nHashBuckets;

   _myCardinality++;
   return;
}


IfdKey*
IfdCompositeKey::car(void ) const {
  // 1st element of the list

  if ( cardinality() == 0 ) {
    return 0;
  } else {
    return kid[0]->clone();
  }
}

IfdCompositeKey*
IfdCompositeKey::cdr(void ) const {
  // 2nd through last elements of the list.

  IfdCompositeKey *c = new IfdCompositeKey();
  int i;
  for( i=1; i < cardinality(); i++) {
    c->add( *kid[i] );
  }

  return c;
}

IfdKey*
IfdCompositeKey::tail(void ) const {
  // last element of the list

  if ( cardinality() == 0 ) {
    return 0;
  } else {
    return kid[ cardinality()-1 ]->clone();
  }
}


