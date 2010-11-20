//--------------------------------------------------------------------------
// File and Version Information:
// 	$Id: AbsArgVal.cxx,v 1.1.1.1 2005/03/29 17:13:30 steinke Exp $
//
// Description:
//      AbsArgVal.h is forced to implement all the meothds because of DEC cxx.
//      However, HP-UX complains about no .cxx file.  So here we have
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
#include "AbsArgVal.h"

#include "IfdTypeKey.h"

//		----------------------------------------
// 		-- Public Function Member Definitions --
//		----------------------------------------

//----------------
// Constructors --
//----------------

template <class T>
AbsArgVal<T>::AbsArgVal( T* t ) : _myT( t )
{}

template <class T>
AbsArgVal<T>::AbsArgVal( T& t ) : _myT( &t )
{}


//--------------
// Destructor --
//--------------


template <class T>
AbsArgVal<T>::~AbsArgVal() 
{}

//-------------
// Methods   --
//-------------
    

//-------------
// Operators --
//-------------
    
//-------------
// Selectors --
//-------------
    
template <class T>
const IfdTypeKeyIFace&
AbsArgVal<T>::getTypeKey() const {

  // The object _myKey is equivalent to being class data, i.e.,
  // eqivalent to being a static data member declared in the class
  // declaration.  We place it here, in this .cxxessor function, because
  // g++ does not support statics in templates, but will compile this.
  // Thanks to Simon Patton for pointing this out.

   static const IfdTypeKey<T> _myKey;

  return _myKey;
}

template <class T>
AbsArg*
AbsArgVal<T>::clone() const {
  return new AbsArgVal<T>( _myT );
}

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
