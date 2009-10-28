//--------------------------------------------------------------------------
// File and Version Information:
// 	$Id: AbsArgCast.cxx,v 1.1.1.1 2005/03/29 17:13:30 steinke Exp $
//
// Description:
//      AbsArgCast.h is forced to implement all the meothds because of DEC 
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
#include "AbsArgCast.h"

#include "IfdTypeKey.h"
#include "AbsArg.h"
#include "AbsArgVal.h"

//		----------------------------------------
// 		-- Public Function Member Definitions --
//		----------------------------------------

//----------------
// Constructors --
//----------------

//--------------
// Destructor --
//--------------

//-------------
// Methods   --
//-------------
    

template <class T>
T*
AbsArgCast<T>::value( AbsArg* a ) {
  return value( *a );
}


template <class T>
T*
AbsArgCast<T>::value( AbsArg& a ) {

  IfdTypeKey<T> tk;

  if ( tk == a.getTypeKey() ) {
    return ((AbsArgVal<T>*) &a)->_myT;
  } else {
    return (T*) 0;
  }
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
