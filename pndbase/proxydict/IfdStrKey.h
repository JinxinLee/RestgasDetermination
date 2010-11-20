#ifndef IFDSTRKEY_HH
#define IFDSTRKEY_HH
//--------------------------------------------------------------------------
// File and Version Information:
//    $Id: IfdStrKey.h,v 1.1.1.1 2005/03/29 17:13:30 steinke Exp $
//
// Description:
//    IfdKeys based on string values.  Implementation is not in
//    terms of a string class, i.e., is in terms of char*, because
//    the package's usage is spreading and not everyone uses the
//    same standard String class.
//
// Author List:
//    Ed Frank                        University of Pennsylvania
//
// History:
//    Ed Frank      17 Nov 96         Creation of first version
//
// Copyright Information:
//      Copyright (C) 1997
//
// Bugs:
//
//      Bertram Kopf (RUB) migrated to PandaRoot
//------------------------------------------------------------------------

#include "IfdKey.h"
#include <iostream>
#include <stdlib.h>
#include <string.h>

#ifndef VXWORKS
#include <string>
#endif

using namespace std;


class IfdStrKey : public IfdKey {
public:
   enum { IFDKEY_BUFSIZE=32 };  // See comments for _string

   IfdStrKey( const char *s );
#ifndef VXWORKS
   IfdStrKey( const std::string& s );
#endif
   virtual ~IfdStrKey();

   virtual int operator==( const IfdKey& k ) const;
   virtual int operator< ( const IfdKey& k ) const;
      // colation as determined by strcmp.  Be careful.

   virtual IfdKey* clone(void) const;
      // clone this key.  caller owns returned key.

   const char* asString(void) const { return strVal; }
   virtual void print( ostream &o) const;


protected:
//   virtual keyKind getKeyKind( void ) const { return strKey; }

private:
   IfdStrKey( const char *s, unsigned int hashVal );
      // Used for clone() to avoid cost of hash function.

   // Copy ctor and assignemnt op are not allowed.  This keeps
   // the class behavior like its base, IfdKey.  See that class for
   // more info.
   IfdStrKey( const IfdStrKey &);
   IfdStrKey& operator=( IfdStrKey &);

   // We allocate space for strings up to 32 bytes (including termination)
   // and, if a string longer than that is needed, we allocate on the heap.
   // The intention is to provide faster service for the most often used
   // cases, although at a penalty in size.

   char _stringBuf[ IFDKEY_BUFSIZE ];
   bool _onHeap;

};


#endif /*  IFDSTRKEY_HH */
