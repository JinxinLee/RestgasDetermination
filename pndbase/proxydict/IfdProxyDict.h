#ifndef IFDPROXYDICT_HH
#define IFDPROXYDICT_HH
//--------------------------------------------------------------------------
// File and Version Information:
//    $Id: IfdProxyDict.h,v 1.1.1.1 2005/03/29 17:13:30 steinke Exp $
//
// Description:
//
// Author List:
//    Ed Frank                        University of Pennsylvania
//
// History:
//    Ed Frank      17 Nov 96         Creation of first version
//                  07 Jan 97         Rename everything, disallow copy and
//                                    op=()
//    Ed Frank      24 Feb 97         Add invalidateProxyCaches().
//                  30 May 97        -change name of above to testProxyCaches()
//                                   -add markForStore, doStore(...), doStore()
//                                   -Add AbsArg& arg to get.
//                  02 Jul 97        -Make find(...) public so that
//                                   -Ifd<T>::keyUsed() can be written.
//    Ed Frank      28 Aug 97        -Remove IfdData* and use void* instead.
//
//                                   -
//
// Bugs:
//
//      Bertram Kopf (RUB) migrated to PandaRoot
//------------------------------------------------------------------------

// A class for gaining .cxxess to data objects *including* their full
// full interface, in a type-safe manner.  Agents put data into the
// dictionary as a specific type T, and then (perhaps other) agents
// fetch the data back out again, receiving it as a T.
//
// explain dictKey.
//
// Unwritten:  we're assuming everything's ptrs.  think hard about this.
//

#include <stdlib.h>

class IfdDictKey;
class IfdDataProxyIFace;
class AbsArg;

class IfdProxyDict {
public:
  IfdProxyDict() {}
  virtual ~IfdProxyDict();

  virtual void*  get( const IfdDictKey& k, AbsArg& a );
  virtual bool   put( const IfdDictKey& k, IfdDataProxyIFace* p );
     // p can't be const because we are taking ownership of it
     // and will eventually delete it.

  enum StoreFlagControl { clearStoreFlags, keepStoreFlags };

  // The following methods allow front-ends to post requests for
  // eventual dispatch to the proxies.  This is used, for example,
  // to store data stored in a proxy back to a storage pool.  markForStore()
  // is used to post the request.  storeAllMarked(), which is called some
  // time after all the markForStore()'s have been done, dispatches the
  // request to the proxies, sending any absargs given when marked.
  // The StoreFlagControl argument determines whether the storeAllMarked
  // clears the posted requests or not, e.g., if keepStoreFlags is
  // given, then the request is posted to the proxies, but the set of
  // "need to store" flags kept in the dictionary are *not* cleared, so
  // that another storeAllMarked will cause another store.  The method
  // storeItem() is not post/execute, but just causes an immediate 
  // request to be given to a proxy.

  virtual bool markForStore( const IfdDictKey& k, AbsArg& a)=0;
  virtual void storeAllMarked( const StoreFlagControl c = clearStoreFlags )=0;
  virtual void storeItem( const IfdDictKey& k, AbsArg& a);
  

  virtual void testProxyCaches(void)=0;
     // causes testCache() to be called on all proxies.  This means
     // that each Proxy will check the validity of its data (typically
     // for expiration/timeout).  It is up to the proxy to decide what
     // this means.  It is likely that the proxy will just set a flag
     // and defer the check until the next actual request, and even then
     // the proxy might decide thta the cache is ok afterall.

  virtual IfdDataProxyIFace* find(const IfdDictKey& k ) const          = 0;
     // Public for Ifd<T>::keyUsed().

protected:
  virtual void               add (const IfdDictKey& k, IfdDataProxyIFace* p)=0;
     // p can't be const because we are taking ownership of it
     // and will eventually delete it.

private:
  // copy and assignement do not make any sense for this class.  Too much
  // state in the proxies.
  
  IfdProxyDict( const IfdProxyDict& ) { ::abort(); }
  IfdProxyDict& operator=( const IfdProxyDict& )
     { if ( this != 0 ) ::abort(); return *this; }

};

//************************
// declare the front-end
//************************
//
#include "Ifd.h"

#endif /* IFDPROXYDICT_HH */
