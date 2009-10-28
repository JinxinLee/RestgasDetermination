//--------------------------------------------------------------------------
// File and Version Information:
// 	$Id: IfdAliasProxy.cxx,v 1.1.1.1 2005/03/29 17:13:30 steinke Exp $
//
// Description:
//    See .h file.
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
//	Copyright (C) 1998
//
//      Bertram Kopf (RUB) migrated to PandaRoot
//------------------------------------------------------------------------

//-----------------------
// This Class's Header --
//-----------------------
#include "IfdAliasProxy.h"

#include "Ifd.h"


//		----------------------------------------
// 		-- Public Function Member Definitions --
//		----------------------------------------

//----------------
// Constructors --
//----------------

template <class T>
IfdAliasProxy<T>::IfdAliasProxy( IfdKey&  k )
   : _targetKey( k.clone() )
{}

template <class T>
IfdAliasProxy<T>::IfdAliasProxy( )
   : _targetKey( 0 )
{}


//--------------
// Destructor --
//--------------

//-------------
// Methods   --
//-------------
    
template <class T>
void*
IfdAliasProxy<T>::get(IfdProxyDict* d,  const IfdKey& , AbsArg& a ) {
  // Note that we explicitly use Ifd<T> to make sure that
  // our type and the target type are compatible.
  //
  if ( 0 == _targetKey ) {
    return Ifd<T>::get(d, a );
  } else {
    return Ifd<T>::get(d, *_targetKey, a );
  }
}

template <class T>
void
IfdAliasProxy<T>::store( IfdProxyDict*d, const IfdKey& k, AbsArg& a) {
  // no op.
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
