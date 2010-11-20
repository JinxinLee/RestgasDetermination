//--------------------------------------------------------------------------
// File and Version Information:
// 	$Id: IfdDataProxy.cxx,v 1.1.1.1 2005/03/29 17:13:30 steinke Exp $
//
// Description:
//      IfdDataProxy.h is forced to implement all the meothds because of DEC
//      cxx.  However, HP-UX complains about no .cxx file.  So here we have
//      an empty one.
//      
//      EXCLUDE THIS FILE FROM THE LIST OF .CC FILE TO COMPILE!
//
// Environment:
//	Software developed for the BaBar Detector at the SLAC B-Factory.
//
// Author List:
//      Ed Frank		University of Pennsylvania
//
// Copyright Information:
//	Copyright (C) 1997
//
//      Bertram Kopf (RUB) migrated to PandaRoot
//------------------------------------------------------------------------

//-----------------------
// This Class's Header --
//-----------------------
#include "IfdDataProxy.h"

#include <stdlib.h>


//		----------------------------------------
// 		-- Public Function Member Definitions --
//		----------------------------------------

//----------------
// Constructors --
//----------------

template <class T>
IfdDataProxy<T>::IfdDataProxy(T* t)
   : handler( 0 )
{ 
  this->_myT = t;
  this->setCacheIsValid();
}

template <class T>
IfdDataProxy<T>::IfdDataProxy(T* (*f)(const IfdKey& ))
   : handler( f )
{}

template <class T>
IfdDataProxy<T>::IfdDataProxy() {
  ::abort();
}


//--------------
// Destructor --
//--------------

//-------------
// Methods   --
//-------------
    
template <class T>
T*
IfdDataProxy<T>::faultHandler(IfdProxyDict*, const IfdKey& k, AbsArg& ) {
  this->setCacheIsValid();
  return handler( k );
}

template <class T>
void
IfdDataProxy<T>::storeHandler(IfdProxyDict*, const IfdKey& k, AbsArg&, T* )
{}

template <class T>
void
IfdDataProxy<T>::testCache( void )
     // We provide an implementation of testCache() that thinks
     // that its cache is never invalid.  This is useful for the simple
     // case where the proxy just holds a list and does not use any
     // kind of faulting, e.g., when ctor IfdDataProxy<T>( T* t)  is used.
{}

//-------------
// Operators --
//-------------
    
//-------------
// Selectors --
//-------------
    
//-------------
// Modifiers --
//-------------

//		-----------------------------------------------
// 		-- Static Data & Function Member Definitions --
//		-----------------------------------------------

//		-------------------------------------------
// 		-- Protected Function Member Definitions --
//		-------------------------------------------

//		-----------------------------------------
// 		-- Private Function Member Definitions --
//		-----------------------------------------

//		-----------------------------------
// 		-- Internal Function Definitions --
//		-----------------------------------
