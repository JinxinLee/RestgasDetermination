#ifndef IFD_HH
#define IFD_HH
//--------------------------------------------------------------------------
// File and Version Information:
//    $Id: Ifd.h,v 1.1.1.1 2005/03/29 17:13:30 steinke Exp $
//
// Description:
//    Front end for IfdProxyDict.  The implementation is in this file
//    not because I want it inline, but because some compilers
//    seem to have trouble finding the implementation in order to
//    instantiate the templates in client code when I put it in a
//    separate .cxx file.
//
// Author List:
//    Ed Frank                        University of Pennsylvania
//
// History:
//    Ed Frank      17 Nov 96         Creation of first version
//    Ed Frank      2x Nov 96         AIX xlC 1.0.1 does not support default
//                                    args in template fns.  Ifdef around it.
//    Ed Frank      29 Nov 96         HP C++ HPCPLUSPLUS  A.10.22 does not
//                                    allow default args in template fns eithr.
//                                    Introduce macro  NO_TEMPLATE_DFLT_ARGS
//                                    in stead of EFXLC macro (now gone.)
//    Ed Frank      24 Jan 97         -Remove CPP macros above.
//                                    -Add Ifd<T> class.
//                                    -Move implementations to .cxx rather than
//                                     having them inline.
//    Ed Frank      30 May 97         -extend interface
//    Ed Frank      02 Jul 97         -add keyUsed()
//    Ed Frank      28 Aug 97         -Allow put of T* (not proxy<T>) by
//                                     creating a simple proxy for user.
//    R. Bartoldus  17 Apr 99         Make char* argument of put() const.
//               
// Bugs:
//------------------------------------------------------------------------


#include "IfdProxyDictFrontEnd.h"
#include "AbsArgVal.h"

class IfdProxyDict;
class AbsArg;
class IfdKey;
template <class T> class IfdDataProxyTemplate;


//**********************************************************************
template <class T>
class Ifd : public IfdProxyDictFrontEnd {
public:
//   virtual ~Ifd(){}
//   Ifd();
  // In all of these methods, the user retains ownership of the 
  // secondary key, when provided.  The dictionary will clone a copy
  // of these keys, as needed.
  //
  // There are NxM more of these methids than you'd want because we
  // can not use default arguments.  This is because some compilers
  // do not support default args in templated classes.

  static T*   get( IfdProxyDict *d);
  static T*   get( IfdProxyDict *d, AbsArg& a);
  static T*   get( IfdProxyDict *d, const IfdKey &k);
  static T*   get( IfdProxyDict *d, const IfdKey &k, AbsArg& a);
  static T*   get( IfdProxyDict *d, const char *s);
  static T*   get( IfdProxyDict *d, const char *s, AbsArg& a);
    //
    // Get an item from ProxyDict d using only type info for key, or
    // using a secondary key, or using a string that is converted to
    // a secondary key.

  static bool put(IfdProxyDict *d, IfdDataProxyTemplate<T> *p );
  static bool put(IfdProxyDict *d, IfdDataProxyTemplate<T> *p,const IfdKey &k);
  static bool put(IfdProxyDict *d, IfdDataProxyTemplate<T> *p,const char *s);
    //
    // Put a Proxy into the dictionary.

  static bool put(IfdProxyDict *d, T* t);
  static bool put(IfdProxyDict *d, T* t, const IfdKey &k);
  static bool put(IfdProxyDict *d, T* t, const char *s);
    //
    // Create a proxy for a transient T* and put that proxy into
    // the event.  We do not allow AbsArg's here because the proxy
    // we create for the object does not use them.  DIRE WARNING: the
    // proxy created for you will delete your data T via "delete T."
    // If you put in a container, like HepALists, for which this does
    // not delete owned objects, you will get a big memory leak.  Handle
    // this by writing a real proxy and using the standard 
    // IfdDataProxyTemplate<T> interface.


  static bool markForStore(IfdProxyDict *d);
  static bool markForStore(IfdProxyDict *d, AbsArg& a );
  static bool markForStore(IfdProxyDict *d, const IfdKey& k );
  static bool markForStore(IfdProxyDict *d, const IfdKey& k, AbsArg& a );
  static bool markForStore(IfdProxyDict *d, const char *s);
  static bool markForStore(IfdProxyDict *d, const char *s, AbsArg& a );
  //
    // Ask the dictionary to post a request for a store() operation
    // on a particular proxy some time in the future.  The store() will
    // be done when IfdProxyDict::store() is called.  DO NOT use
    // storeItem() on an item that has had markForStore called on it.

  static void storeItem(IfdProxyDict *d );
  static void storeItem(IfdProxyDict *d, AbsArg& a );
  static void storeItem(IfdProxyDict *d, const IfdKey& k );
  static void storeItem(IfdProxyDict *d, const IfdKey& k, AbsArg& a );
  static void storeItem(IfdProxyDict *d, const char* s );
  static void storeItem(IfdProxyDict *d, const char* s, AbsArg& a );
    //
    // Cause store() to be called on a specific proxy immediately.
    // Do NOT use markForStore() beforehand because storeItem() ignores
    // the lists built up by markForStore() and you don't want to corrupt
    // those lists.

  static bool keyUsed(IfdProxyDict *d );
  static bool keyUsed(IfdProxyDict *d, const IfdKey& k );
    //
    // Answers the question, "Could I get this data if I asked for it?"
    // Determine if data have been placed in the dictionary with
    // the given key.  This test will not cause they proxy to
    // take any action (it won't fault), and _by design_ does not
    // involve the proxy.  This will allow us to support dictionaries
    // in the future that do not use proxies.
    //

private:
  static AbsArg& nullArg();
    //
    // This method must be static because we want to call it from
    // other static methods in the class (no this pointer).

};

#include  "Ifd.cxx"


#endif /*  IFD_HH */
