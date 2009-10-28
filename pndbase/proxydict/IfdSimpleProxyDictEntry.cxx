//--------------------------------------------------------------------------
// File and Version Information:
// 	$Id: IfdSimpleProxyDictEntry.cxx,v 1.1.1.1 2005/03/29 17:13:30 steinke Exp $
//
// Description:
//    See .h file
//
// Environment:
//	Software developed for the BaBar Detector at the SLAC B-Factory.
//
//
// Author List:
//	Ed Frank		University of Pennsylvania
//
// Copyright Information:
//	Copyright (C) 1997
//
//
//      Bertram Kopf (RUB) migrated to PandaRoot
//------------------------------------------------------------------------

//-----------------------
// This Class's Header --
//-----------------------
#include "IfdSimpleProxyDictEntry.h"

//-------------------------------
// Collaborating Class Headers --
//-------------------------------

#include "IfdDictKey.h"
#include "IfdDataProxyIFace.h"
#include "AbsArg.h"

//-----------------------------------------------------------------------
// Local Macros, Typedefs, Structures, Unions and Forward Declarations --
//-----------------------------------------------------------------------



//		----------------------------------------
// 		-- Public Function Member Definitions --
//		----------------------------------------

//----------------
// Constructors --
//----------------

IfdSimpleProxyDictEntry::IfdSimpleProxyDictEntry(const IfdDictKey& k,
						 IfdDataProxyIFace* p )
    : key( k.cloneDictKey() ), proxy( p ), storeFlag( false), arg(0)
     ,next(0)
     ,nextDelete(0)
{}
  
  
//--------------
// Destructor --
//--------------
IfdSimpleProxyDictEntry::~IfdSimpleProxyDictEntry() {
  key->deleteKeys();
  delete key;
  delete proxy;
  delete arg; 
}

//-------------
// Methods   --
//-------------
    
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
