//--------------------------------------------------------------------------
// File and Version Information:
// 	$Id: AstSTLNameMap.hh,v 1.1.1.1 2005/03/29 16:49:05 steinke Exp $
//
// Description:
//	Class AstSTLNameMap
//
//      A set of std::string stored both as a 
//         map<string,int> and vector<string> for fast lookup in either 
//         direction
//
// Environment:
//	Software developed for BaBar expirment @ SLAC B-Factory
//
// Author List:
//      Eric Charles
//
// Copyright Information:
//	Copyright (C) 2003              LBNL
//
// Bertram Kopf (RUB)  migrated to PandaRoot
//------------------------------------------------------------------------
//-----------------------
// This Class's Header --
//-----------------------

#include "AstSTLNameMap.h"

//-------------
// C Headers --
//-------------
extern "C" {
}

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

AstSTLNameMap::AstSTLNameMap(){}
AstSTLNameMap::AstSTLNameMap( const AstSTLNameMap& other )
    :_map(other._map),_vect(other._vect){
  }

//--------------
// Destructor --
//--------------

AstSTLNameMap::~AstSTLNameMap(){}

//-------------
// Methods   --
//-------------
unsigned int AstSTLNameMap::add(const std::string& sname) {
    map_const_iterator itr = _map.find(sname);
    if ( itr != _map.end() ) return itr->second;
    unsigned int val = _vect.size();
    _map[sname] = val;
    _vect.push_back(sname);
    return val;
  }

void AstSTLNameMap::print(std::ostream& o) const {
    for ( const_iterator itr = begin(); itr != end(); ++itr ){
      o << *itr << std::endl;
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
