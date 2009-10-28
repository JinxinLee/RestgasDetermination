#ifndef IFDDICTKEY_HH
#define IFDDICTKEY_HH
//--------------------------------------------------------------------------
// File and Version Information:
//    $Id: IfdDictKey.h,v 1.1.1.1 2005/03/29 17:13:30 steinke Exp $
//
// Description:
//    A key for .cxxessing a dictionary.  Only selected classes can
//    create these:  this is central to type safety in the dictionary.
//    Also, only selected, core classes in ProxyDict should ever have to
//    handle one of these:  when passed to the outside world it should
//    be passed as a const IfdKey.  As described below, the const is important
//    for two reasons.  First, the key _should_ be const by the time it
//    is given to an outside agent, e.g., a proxy, since the key has been
//    fully formulated by then.  Second, for very awful reasons, Liskoff's
//    substitution principle is violated for non-const IfdDictKeys (see below)
//    but satisfied for const IfdDictKeys.  This lets us do some speed 
//    optimizations that we badly need.  The price is that the few classes
//    that handle DictKeys  (the core ProxyDict class and front ends) must
//    do so with care.
//
//    Another violation of Liskoff is that you should not add a dictKey to
//    a composite key or another DictKey.
//
//    or does lifetime fix all this???
//
// Author List://
//    Ed Frank                        University of Pennsylvania
//
// History:
//    Ed Frank      18 Jan 00         Break Liskoff, as described above
//    Ed Frank      17 Nov 96         Creation of first version
//
// Bugs:
//    - Right now we inherit from IfdCompositeKey and thus  break the Liskoff
//      rule because of the add() method.  We'll clean this up some day.  Do
//      not cast/promote to a CompositKey!
//
//      Bertram Kopf (RUB) migrated to PandaRoot
//------------------------------------------------------------------------

// This is just a key for looking things up in the type-safe
// dictionary.  It is nothing more than a CompositeKey object, but we
// make its ctor prviate (unlike a regular key).  The dictionary
// font-end, namely, get<T>(ProxyDict *d, Key k)  and 
// put(ProxyDict *d, DictEntryProxy<T> *p, Key k), are declared friends
// of IfdDictKey so that they, and only they, may make keys for seraching the
// dictionary.  It is the job of get<T>() and put<T>() to handle the
// type-safety issues.
//

#include <stdlib.h>
#include "IfdCompositeKey.h"
#include "IfdTypeKey.h"


template <class T> class Ifd;
class IfdProxyDictFrontEnd;

class IfdDictKey : public IfdCompositeKey {
public:
   friend class IfdSimpleProxyDictEntry;
   friend class IfdProxyDictFrontEnd;

   virtual ~IfdDictKey();

   virtual void add( const IfdKey& k );
     //  Here is where we break Liskoffs principle.  We change add() so that,
     //  unlike in IfdCompositeKey, we do not clone k and do not take 
     //  ownership of it.

   inline void reset( void );
     //  Remove all keys previously added from the list but without deleting
     //  them; however, do NOT remove the initial IfdTypeKeyIFace (see ctor).

   void deleteKeys( void );
     //  Delete all the keys that have been added, including the initial
     //  IfdTypeKeyIFace.


private:
   IfdDictKey( const IfdTypeKeyIFace& k);
     // This ctor (1) requires that the IfdDictKey start with a TypeKey
     // (2) actually CompositeKey::add()'s the TypeKey in.

   IfdDictKey* cloneDictKey() const { return (IfdDictKey*)  clone(); }
     //  This is just clone, but we return the result as a DictKey, not IfdKey
     //  $$ fix this...the cast is not needed. 


   IfdDictKey& operator=( IfdDictKey &) { if ( this != 0 ) ::abort(); return *this; }
   IfdDictKey( IfdDictKey & ) :  IfdCompositeKey()  { ::abort(); }
     //  The if(k) just stifles the "k not used" warning" and the 
     // IfdCompositeKey ctor stifles another warning. 

   unsigned int  _hashValCache; 
     // See comments in the .cxx file @ in the ctor code.
};



//*****************************************************************************
void
IfdDictKey::reset( void ) {
//*****************************************************************************
   _myCardinality = 1;         //That leaves the IfdTypeKeyIFace from the ctor.
   _hashVal = _hashValCache;   //Give hash val consistent with that.
   return;
}

//*****************************************************************************
#endif /*  IFDDICTKEY_HH */
