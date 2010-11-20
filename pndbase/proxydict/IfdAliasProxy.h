//--------------------------------------------------------------------------
// File and Version Information:
// 	$Id: IfdAliasProxy.h,v 1.1.1.1 2005/03/29 17:13:30 steinke Exp $
//
// Description:
//	This proxy acts as an alias for another proxy.  Suppose you
//      have a proxy for type T thta will be put in with a secondary
//      key of k.  Create an IfdAliasProxy<T>( k ) and put it in the
//      dictionary w/ key k2.  Now any call Ifd<T>::get(d, d2) will
//      be dispatched to Ifd<T>::get(d,k), thus achieving the aliasing.
//      The get() is dispatched, but store() and checkCache() are ignored.
//      
//
// Environment:
//	Software developed for the BaBar Detector at the SLAC B-Factory.
//
// Author List:
//	Ed Frank		University of Pennsylvania
//
// History:
//    Ed Frank      28 Aug 97         Replace IfdData* with void*
//
// Copyright Information:
//	Copyright (C) 1997
//
//      Bertram Kopf (RUB) migrated to PandaRoot
//------------------------------------------------------------------------

#ifndef IFDALIASPROXY_HH
#define IFDALIASPROXY_HH

//---------------
// C++ Headers --
//---------------

//----------------------
// Base Class Headers --
//----------------------

#include "IfdDataProxyTemplate.h"

//-------------------------------
// Collaborating Class Headers --
//-------------------------------


//------------------------------------
// Collaborating Class Declarations --
//------------------------------------

class AbsArg;
class IfdKey;
class IfdProxyDict;

//		---------------------
// 		-- Class Interface --
//		---------------------

template<class T> 
class IfdAliasProxy : public IfdDataProxyTemplate<T>  {

//--------------------
// Instance Members --
//--------------------

public:

  //  Note: The workaround to a VxWorks template instantiation bug 
  // requires that the destructor be the first function defined in a
  // templated class.  Please do not move this or put functions above it!

  // Destructor
  virtual ~IfdAliasProxy( ) {delete _targetKey;}

  // Constructors

  IfdAliasProxy( IfdKey&  k );
  IfdAliasProxy( );


  // Methods

  virtual void* get(IfdProxyDict* d,  const IfdKey& , AbsArg& a );
  virtual void  store( IfdProxyDict*d, const IfdKey& k, AbsArg& a);

  virtual T*   faultHandler( IfdProxyDict*, const IfdKey&, AbsArg&    )
     {return 0;};
  virtual void storeHandler( IfdProxyDict*, const IfdKey&, AbsArg&,T* ) {};
  virtual void testCache( void ) {};

private:
  IfdKey* _targetKey;

  // Copy Constructor and assignment op not allowed.  CXX forces definition.
  //
  IfdAliasProxy( const IfdAliasProxy<T>& ) 
     { abort(); }
  IfdAliasProxy<T>& operator= ( const IfdAliasProxy<T>& )
     { return *this; abort(); }

};


#include  "IfdAliasProxy.cxx"


#endif  // IFDALIASPROXY_HH
