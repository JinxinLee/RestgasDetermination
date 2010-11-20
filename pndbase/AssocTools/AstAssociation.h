//--------------------------------------------------------------------------
// File and Version Information:
// 	$Id: AstAssociation.hh,v 1.1.1.1 2005/03/29 16:49:05 steinke Exp $
//
// Description:
//	Class AstAssociation
//      Very simple class to provide an association between two
//      pointer types which don't obviously belong together in a class,
//      but need to go together for some reason.
//
// Environment:
//	Software developed for the BaBar Detector at the SLAC B-Factory.
//
// Author List:
//      Phil Strother           Original author
//
// Copyright Information:
//	Copyright (C) 1997      Imperial College
//
//  Bertram Kopf (RUB)  migrated to PandaRoot
//------------------------------------------------------------------------

#ifndef ASTASSOCIATION_HH
#define ASTASSOCIATION_HH

//-------------
// C Headers --
//-------------
extern "C" {
}

//---------------
// C++ Headers --
//---------------

//----------------------
// Base Class Headers --
//----------------------
#include "AstAbsAssociation.h"

//-------------------------------
// Collaborating Class Headers --
//-------------------------------


//------------------------------------
// Collaborating Class Declarations --
//------------------------------------

template <class T1, class T2> class AstAssociation;

//		---------------------
// 		-- Class Interface --
//              ---------------------

template <class T1, class T2>
class AstAssociation : public AstAbsAssociation<T1, T2> {

//--------------------
// Declarations     --
//--------------------

    // Typedefs, consts, and enums

//--------------------
// Instance Members --
//--------------------

public:

  // Constructors
  AstAssociation(T1 *, T2 *);

  // Copy Constructor
  AstAssociation( const AstAssociation<T1, T2> &);

  AstAssociation<T1, T2> &operator=(const AstAssociation<T1, T2> &);
  // Destructor
  virtual ~AstAssociation( );

  // Selectors (const)

  virtual const T1 *firstArgument() const;
  virtual const T2 *secondArgument() const;

  virtual T1 *firstArgument();
  virtual T2 *secondArgument();
  // Modifiers

  virtual void deleteMembers();

protected:
  
    // Helper functions

private:

  // Friends
  
  // Data members
  
  T1 *_itsT1;
  T2 *_itsT2;

  AstAssociation();
  
//------------------
// Static Members --
//------------------

public:

    // Selectors (const)

    // Modifiers

private:

    // Data members

};

#include  "AstAssociation.cxx"


#endif







