#ifndef IFDINTKEY_HH
#define IFDINTKEY_HH
//--------------------------------------------------------------------------
// File and Version Information:
//    $Id: IfdIntKey.h,v 1.1.1.1 2005/03/29 17:13:30 steinke Exp $
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
#include <iostream>
#include <stdlib.h>

using namespace std;

class IfdIntKey : public IfdKey {
public:
   IfdIntKey( const int i );
      // See also protected:IfdIntKey()

   virtual int operator==( const IfdKey& k ) const {
      return ( intKey == k.getKeyKind() ) && ( intVal == k.intVal );
   }

   virtual ~IfdIntKey() {}

   virtual IfdKey* clone(void) const { return new IfdIntKey( intVal ); }
      // Caller owns returned copy of this key.

   virtual void print( ostream &o) const { o << "IfdIntKey(" << intVal << ")"; }

protected:

private:
  // Copy ctor and assignemnt op are not allowed.  This keeps
  // the class behavior like its base, IfdKey.  See that class for
  // more info.
  IfdIntKey( const IfdIntKey &);
  IfdIntKey& operator=( IfdIntKey &);


};


#endif /*  IFDINTKEY_HH */
