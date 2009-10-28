//--------------------------------------------------------------------------
// File and Version Information:
// 	$Id: IfdProxyDictFrontEnd.cxx,v 1.1.1.1 2005/03/29 17:13:30 steinke Exp $
//
// Description:
//	Implementation of IfdProxyDictFrontend.  See .h file for comments.
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

//-----------------------
// This Class's Header --
//-----------------------
#include "IfdProxyDictFrontEnd.h"


//---------------
// C++ Headers --
//---------------

#include "IfdTypeKeyIFace.h"
#include "IfdDictKey.h"

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
IfdProxyDictFrontEnd::IfdProxyDictFrontEnd()
{}

//--------------
// Destructor --
//--------------

IfdProxyDictFrontEnd::~IfdProxyDictFrontEnd() {
}



//		-------------------------------------------
// 		-- Protected Function Member Definitions --
//		-------------------------------------------


IfdDictKey*
IfdProxyDictFrontEnd::newDictKey( const IfdTypeKeyIFace& k ) {

  // NOTE: The caller owns the returned IfdDictKey and must delete it.

  IfdDictKey* dk = new IfdDictKey( k );
  return dk;
}


IfdDictKey*
IfdProxyDictFrontEnd::newDictKey( const IfdTypeKeyIFace& k, const IfdKey& k2 )
{

  // NOTE: The caller owns the returned IfdDictKey and must delete it.

  IfdDictKey* dk = new IfdDictKey( k );
  dk->add( k2 );
  return dk;
}


//		-----------------------------------------
// 		-- Private Function Member Definitions --
//		-----------------------------------------
