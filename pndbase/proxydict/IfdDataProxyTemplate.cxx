//--------------------------------------------------------------------------
// File and Version Information:
// 	$Id: IfdDataProxyTemplate.cxx,v 1.1.1.1 2005/03/29 17:13:30 steinke Exp $
//
// Description:
//	Implementation of IfdDataProxyTemplate.cxx methods.
//      
//      EXCLUDE THIS FILE FROM THE LIST OF .CC FILE TO COMPILE!
//
// Environment:
//	Software developed for the BaBar Detector at the SLAC B-Factory.
//
// Author List:
//	Ed Frank		University of Pennsylvania
//
// Copyright Information:
//	Copyright (C) 1996
//
// BUGS:
//    See the .h file.
//
//      Bertram Kopf (RUB) migrated to PandaRoot
//------------------------------------------------------------------------


//-----------------------
// This Class's Header --
//-----------------------
#include "IfdDataProxyTemplate.h"

//---------------
// C++ Headers --
//---------------

//-------------------------------
// Collaborating Class Headers --
//-------------------------------

//-----------------------------------------------------------------------
// Local Macros, Typedefs, Structures, Unions and Forward Declarations --
//-----------------------------------------------------------------------


//		----------------------------------------
// 		-- Public Function Member Definitions --
//		----------------------------------------

//----------------
// Constructors --
//----------------

template <class T>
IfdDataProxyTemplate<T>::IfdDataProxyTemplate()
   : _myT( 0 )
   , _cacheIsValid( false)
{}



//--------------
// Destructor --
//--------------

//-------------
// Methods   --
//-------------

template <class T>
void*
IfdDataProxyTemplate<T>::get(IfdProxyDict* d,  const IfdKey& k, AbsArg& a ) {
  if ( cacheIsValid() ) {
    return this->_myT;
  } else {
    return this->_myT = faultHandler(d, k, a );
  }
}


template <class T>
void
IfdDataProxyTemplate<T>::store( IfdProxyDict*d, const IfdKey& k, AbsArg& a) {
  storeHandler( d, k, a, _myT );
}

//-------------
// Operators --
//-------------
    
//-------------
// Selectors --
//-------------
    
//-------------
// Modifiers --
//-------------

