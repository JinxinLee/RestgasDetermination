//--------------------------------------------------------------------------
// File and Version Information:
// 	$Id: AstAbsAssociation.cc,v 1.1.1.1 2005/03/29 16:49:05 steinke Exp $
//
// Description:
//	Class AstAbsAssociation
//      Abstract interface for simple one to one associations
//
// Environment:
//	Software developed for the BaBar Detector at the SLAC B-Factory.
//
// Author List:
//      Phil Strother           LBNL
//
// Bertram Kopf (RUB)  migrated to PandaRoot
//------------------------------------------------------------------------

 
//-----------------------
// This Class's Header --
//-----------------------

#include "AstAbsAssociation.h"

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


//              ----------------------------------------
//              -- Public Function Member Definitions --
//              ----------------------------------------

//----------------
// Constructors --
//----------------


template<class T1, class T2>
AstAbsAssociation< T1,  T2>::AstAbsAssociation()
{}

// Copy constructor

template<class T1, class T2>
AstAbsAssociation< T1,  T2>::AstAbsAssociation(const AstAbsAssociation<T1, T2> 
					       &other)
{}

//--------------
// Destructor --
//--------------


template<class T1, class T2>
AstAbsAssociation< T1,  T2>::~AstAbsAssociation()
{}

//-------------
// Methods   --
//-------------
    
//-------------
// Operators --
//-------------



template<class T1, class T2>
bool 
AstAbsAssociation<T1, T2>::operator==(const AstAbsAssociation<T1, T2> &other) const
{
  return ((this->firstArgument() == other.firstArgument()) &&
	  (this->secondArgument() == other.secondArgument()));
}


template<class T1, class T2>
bool 
AstAbsAssociation<T1, T2>::operator!=(const AstAbsAssociation<T1, T2> &other) const
{
  return (! (*this==other));
}
