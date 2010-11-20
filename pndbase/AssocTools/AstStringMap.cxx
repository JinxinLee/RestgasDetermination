//--------------------------------------------------------------------------
// File and Version Information:
// 	$Id: AstStringMap.cc,v 1.1.1.1 2005/03/29 16:49:06 steinke Exp $
//
// Description:
//	This class is meant to be used to store an unowned pointer to
//      and object that you reference with a std::string key.
//
// Environment:
//	Software developed for the BaBar Detector at the SLAC B-Factory.
//
// Author List:
//      Stephen J. Gowdy        Originator
//
// Copyright Information:
//      Copyright (C) 2004      Stanford Linear Accelerator Center
//
// Bertram Kopf (RUB)  migrated to PandaRoot
//------------------------------------------------------------------------

//-----------------------
// This Class's Header --
//-----------------------
#include "AstStringMap.h"

//-------------
// C Headers --
//-------------
extern "C" {
}

//---------------
// C++ Headers --
//---------------
#include <iostream>

//-------------------------------
// Collaborating Class Headers --
//-------------------------------

//-----------------------------------------------------------------------
// Local Macros, Typedefs, Structures, Unions and Forward Declarations --
//-----------------------------------------------------------------------

// One for each possible first character
#define ASTSTRINGMAP_NUM_VECTORS 256

//		----------------------------------------
// 		-- Public Function Member Definitions --
//		----------------------------------------

//----------------
// Constructors --
//----------------
template < class T >
AstStringMap< T >::AstStringMap()
  : _stringMap( ASTSTRINGMAP_NUM_VECTORS )
{
}

//--------------
// Destructor --
//--------------
template < class T >
AstStringMap< T >::~AstStringMap()
{
}


//-------------
// Methods   --
//-------------
    
//-------------
// Operators --
//-------------
template < class T >
T*
AstStringMap< T >::operator[]( const std::string& key ) const
{
  uint64_t theInt = stringToInt( key );
  T* theObject = 0;
  if ( theInt == 0 )
    {
      // Get the right map out of the vector
      const std::map< std::string, T* >& theMap
	= _stringMap[ stringToBucket( key ) ];
      // Find the entry, if there is one
      typename std::map< std::string, T* >::const_iterator iter
	= theMap.find( key );
      if ( iter != theMap.end() ) theObject = iter->second;
    }
  else
    {
      // Find the entry, if there is one
      typename std::map< uint64_t, T* >::const_iterator iter
	= _intMap.find( theInt );
      if ( iter != _intMap.end() ) theObject = iter->second;
    }

  return theObject;
}

template < class T >
std::ostream&
operator<<( std::ostream& os, const AstStringMap< T >& map )
{
  for ( int i = 0; i<ASTSTRINGMAP_NUM_VECTORS; i++ )
    {
      os << "Vector: " << i << '\n';
      const std::map< std::string, T* >& aMap = map._stringMap[ i ];
      typename std::map< std::string, T* >::const_iterator iter = aMap.begin();
      while ( iter != aMap.end() )
	{
	  os << '\t' << (*iter).first << '\t' << (*iter).second
	     << '\t' << *((*iter).second) << '\n';
	  ++iter;
	}
    }

  os << "Vector: Int" << '\n';
  typename std::map< uint64_t, T* >::const_iterator iter = map._intMap.begin();
  while ( iter != map._intMap.end() )
    {
      os << '\t' << (*map._intStringMap.find( (*iter).first )).second
	 << '\t' << (*iter).second
	 << '\t' << *((*iter).second) << '\n';
      ++iter;
    }
  return os;
}

//-------------
// Selectors --
//-------------
template < class T >
int
AstStringMap< T >::stringToBucket( const std::string& key ) const
{
  if ( key.empty() ) return 0;
  char firstLetter = key[0];

  return firstLetter % ASTSTRINGMAP_NUM_VECTORS;
}

template < class T >
uint64_t
AstStringMap< T >::stringToInt( const std::string& key ) const
{
  // if it is empty or bigger than we can handle return 0
  int size = key.size();
  if ( key.empty() || size > 8 ) return 0;

  uint64_t theNumber = 0;

  for ( int i = 0; i < size; i++ )
    {
      uint64_t keyChar = static_cast< unsigned char >( key[i] );
      theNumber += keyChar << (i*8);
    }

  return theNumber;
}

template < class T >
int
AstStringMap< T >::size() const
{
  int theSize=0;
  for ( int i = 0; i<ASTSTRINGMAP_NUM_VECTORS; i++ )
    {
      theSize += _stringMap[ i ].size();
    }
  theSize += _intMap.size();

  return theSize;
}

//-------------
// Modifiers --
//-------------
template < class T >
T*&
AstStringMap< T >::insert( const std::string& key )
{
  uint64_t theInt = stringToInt( key );
  if ( theInt == 0 )
    return _stringMap[ stringToBucket( key ) ][key];
  else
    {
      _intStringMap[ theInt ] = key;
      return _intMap[ theInt ];
    }
}


template < class T >
void
AstStringMap< T >::clear()
{
  for ( int i = 0; i<ASTSTRINGMAP_NUM_VECTORS; i++ )
    {
      _stringMap[ i ].clear();
    }
  _intMap.clear();
  _intStringMap.clear();
}

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
