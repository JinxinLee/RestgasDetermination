#ifndef IFDSIMPLEPROXYDICTENTRY_HH
#define IFDSIMPLEPROXYDICTENTRY_HH
//--------------------------------------------------------------------------
// File and Version Information:
// 	$Id: IfdSimpleProxyDictEntry.h,v 1.1.1.1 2005/03/29 17:13:30 steinke Exp $
//
// Description:
//    this class is internal to the implementation of IfdSimpleProxyDict
//    and should not be used elsewhere, i.e., call me if you need it.
//
//    class is just an ntuple of key,proxy,storeFlag,AbsArg for entry
//    into the dictionary.
//
// Environment:
//	Software developed for the BaBar Detector at the SLAC B-Factory.
//
// Author List:
//	Ed Frank		University of Pennsylvania
//
// Copyright Information:
//	Copyright (C) 1997
//
//      Bertram Kopf (RUB) migrated to PandaRoot
//------------------------------------------------------------------------

class IfdDictKey;
class IfdDataProxyIFace;
class AbsArg;

class IfdSimpleProxyDictEntry {
public:
  IfdSimpleProxyDictEntry( const IfdDictKey& k, IfdDataProxyIFace* p );
  ~IfdSimpleProxyDictEntry();

private:  
  // Warning:  it is dangerous to let anyone other than _deep_ ProxyDict
  // internals have .cxxess to an IfdDictKey as an IfdDictKey.

  IfdDictKey              *key;
  IfdDataProxyIFace       *proxy;
  bool                     storeFlag;
  AbsArg                  *arg;
  IfdSimpleProxyDictEntry *next;

  IfdSimpleProxyDictEntry *nextDelete;

  // do not add to this list w/o contacting me. -Ed
  friend class IfdSimpleProxyDict;
  friend class IfdNuclearProxyDict;
  friend class IfdSimpleProxyDictNucleus;
};

#endif //  IFDSIMPLEPROXYDICTENTRY_HH
