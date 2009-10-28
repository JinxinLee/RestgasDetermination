#ifndef IFDCOMPOSITEKEY_HH
#define IFDCOMPOSITEKEY_HH
//--------------------------------------------------------------------------
// File and Version Information:
//    $Id: IfdCompositeKey.h,v 1.1.1.1 2005/03/29 17:13:30 steinke Exp $
//
// Description:
//
// Author List:
//    Ed Frank                        University of Pennsylvania
//
// History:
//    Ed Frank      17 Nov 96         Creation of first version
//    Ed Frank      29 Nov 96         NO_INLINE_COPY_CTOR added
//    Ed Frank      13 Feb 96         NO_INLINE_COPY_CTOR removed.
//
// CPP Macros:
//    NO_INLINE_COPY_CTOR      Turns off inlining of copy ctor.  HP can not
//                             support the for() in an inline right now.
// 
//      Bertram Kopf (RUB) migrated to PandaRoot
//
//------------------------------------------------------------------------

#include <stdlib.h>
#include "IfdKey.h"
#include <iostream>

using namespace std;

class IfdCompositeKey : public IfdKey {
public:
   IfdCompositeKey();
   virtual ~IfdCompositeKey();

   virtual int  operator==( const IfdKey &k ) const;
   virtual void add( const IfdKey& k );

   virtual IfdKey* clone( void ) const { return new IfdCompositeKey( *this );}
      //  caller owns returned copy


   // NOTE:  The caller _OWNS_ the items reurned by car, cdr, and tail.
   //
   IfdKey*          car()  const;     // 1st element of the list
   IfdCompositeKey* cdr()  const;     // 2nd through last elements of the list.
   IfdKey*          tail() const;     // last element of the  list

   virtual void print( ostream &o) const;


   // The next two methods are provisional. Contact efrank@upenn5.hep.upenn.edu
   // before using them.

   // virtual unsigned int hash( void );
   //   Rather than overriding hash(), we .cxxumulate the hash value
   //   as add()'s are done.  Otherwise repeated calls on hash() would
   //   be expensive.

   inline const IfdKey* operator[] (unsigned int u) { return kid[ u ]; }
      // caller does NOT own what is returned

private:
   IfdCompositeKey( const IfdCompositeKey& k );


protected:
   // Fragile, non-paramentric and dirty, but fast.  
   enum { maxChildren=4 };
   const IfdKey* kid[ maxChildren ];

   // virtual keyKind getKeyKind( void ) const { return compositeKey; }

private:
  // No assignment allowed.  Ownership problems result when
  // copying the owned ptrs.  Also, this keeps this class like
  // the base IfdKey class.

  IfdCompositeKey& operator=( const IfdCompositeKey& ) 
     { if (this != 0 ) ::abort(); return *this;}

};

//#include " IfdCompositeKey..cxx"

#endif /*  IFDCOMPOSITEKEY_HH */
