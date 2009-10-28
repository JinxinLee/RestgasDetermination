#ifndef IFDDATAPROXYIFACE_HH
#define IFDDATAPROXYIFACE_HH
//--------------------------------------------------------------------------
// File and Version Information:
//    $Id: IfdDataProxyIFace.h,v 1.1.1.1 2005/03/29 17:13:30 steinke Exp $
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
//    Ed Frank      24 Feb 97         add invalidateCache(void)
//                  30 May 97         change name of above to testCache.
//    Ed Frank      28 Aug 97         Replace IfdData* with void*
//
// Bugs:
//
//      Bertram Kopf (RUB) migrated to PandaRoot
//------------------------------------------------------------------------

// There are two places where the user communicates with the ProxyDict-
// getting data and putting data.  Putting is done by making a Proxy and
// putting the proxy into the dictionary.  CRUCIAL:  the process of making
// a Proxy and associating it with a typekey must be strongly typed!
// For this reason, IfdDataProxyIFace, DictDataProxy<T>, and put<T>
// implement a very definite policy that should give us type safety.
// It is somewhat restrictive, but we will return to that later if needed.
//
// IfdDataProxyIFace:
//    The ProxyDict thinks in terms of these.  They just return DictData*
//    in response to a get( Key &) method.
// DictDataProxy<T>:
//    The user has to make one of these and hand it to put<T>.  It does the
//    real work of the proxy.  Right now, the policy is that DictDataProxy<T>
//    has a ctor that takes a T* and another ctor that takes a T* (*)(Key&)
//    (a function of Key& returning T*).  The first ctor builds a trival
//    proxy that just stores the T passed in in the ctor and rturns it
//    when get(Key&) is called.  The second ctor is .cxxepting a fault handler.
//    This ctor sets an internal T* _myT to NULL and records the handler.
//    get() returns the _myT if non-null, otherwise it sets _myT via the
//    handler and returns it.  The handler can do anything under the sun,
//    except scratch its left elbow with its left hand.
//
// In summary, we are doing the following:  put<T> is strongly typed
// to take a DictDataProxy<T> which has ctors strongly typed so that a T*
// *must* be stored and returned by the code.  put<T> then makes a TypeKey<T>
// and stores the whole mess in the ProxyDict.  get<T> is strongly typed,
// makes a TypeKey<T> and pulls out something that, by construction, must
// have a T.
//

#include <stdlib.h>


class AbsArg;
class IfdProxyDict;
class IfdKey;

class IfdDataProxyIFace {
public:
  IfdDataProxyIFace();
  virtual ~IfdDataProxyIFace();

  virtual void* get( IfdProxyDict*,  const IfdKey&, AbsArg& ) 
    {  if ( this != 0 ) ::abort(); return 0; }
     // In general, this may not leave the proxy const, e.g., a faulting
     // proxy that sets a local cache.   (Why isn't this pure virtual???)

  virtual void store( IfdProxyDict*, const IfdKey&, AbsArg& )=0;

  virtual void testCache( void ) = 0;
     // indicates that the Proxy will check the validity of its data (typically
     // for expiration/timeout).  It is up to the Proxy to decide what this
     // means and when to do it.  The proxy is free to set a flag and
     // defer the actual check until the next time a get() is actually
     // attempted.

  virtual void make_vtab( void ) const;
     // aw foo.

private:
  // Copy and assignment do not make any sense for this class.
  //
  IfdDataProxyIFace( const IfdDataProxyIFace& ) {::abort();}
  IfdDataProxyIFace& operator=( const IfdDataProxyIFace& )
     { if ( this != 0 ) ::abort(); return *this; }

};
#endif /*  IFDDATAPROXYIFACE_HH */
