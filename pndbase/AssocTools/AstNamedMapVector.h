//--------------------------------------------------------------------------
// File and Version Information:
// 	$Id: AstNamedMapVector.hh,v 1.1.1.1 2005/03/29 16:49:05 steinke Exp $
//
// Description:
//	Class AstNamedMapVector
//      Sorts out bookkeeping necessary to create a vector of T1
//      and a (one to one) map of T1 <-> T2, all with a name to describe
//      them.  Useful for a process whose output is a list of T1 
//      which needs to go in the event, but whose input is a list of T2.
//
// Environment:
//	Software developed for the BaBar Detector at the SLAC B-Factory.
//
// Author List:
//      Phil Strother           Original author
//
// Copyright Information:
//	Copyright (C) 1999      LBNL
//
// Bertram Kopf (RUB)  migrated to PandaRoot
//------------------------------------------------------------------------

#ifndef ASTNAMEDMAPVECTOR_HH
#define ASTNAMEDMAPVECTOR_HH

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
#include "AstUniqMap2.h"

//-------------------------------
// Collaborating Class Headers --
//-------------------------------


#include <string>
#include <vector>

//------------------------------------
// Collaborating Class Declarations --
//------------------------------------

//		---------------------
// 		-- Class Interface --
//              ---------------------

template <class T1, class T2>
class AstNamedMapVector : public AstUniqMap2<T1, T2> {

//--------------------
// Declarations     --
//--------------------

    // Typedefs, consts, and enums

//--------------------
// Instance Members --
//--------------------

public:

  // Constructors
  AstNamedMapVector(const std::string &name);

  AstNamedMapVector(const std::string &name,
		    unsigned (*)(const T1&), unsigned (*)(const T2&));

  AstNamedMapVector(const std::string &name,
		    unsigned (*)(const T1&), unsigned (*)(const T2&), 
		    size_t buckets);

  
  // Copy Constructor
  AstNamedMapVector( const AstNamedMapVector<T1, T2> &);

  // Destructor
  virtual ~AstNamedMapVector( );

  // Operators

  AstNamedMapVector<T1, T2> &operator= ( const AstNamedMapVector<T1, T2> &);

  virtual bool operator==( const AstNamedMapVector<T1, T2> & ) const;

  bool operator!=( const AstNamedMapVector<T1, T2> & ) const;

  // Selectors

  const std::string &name() const                 {return *_myName;}
  const std::vector<T1*> & list1() const {return *_vectorOne;}
  const std::vector<T2*> & list2() const {return *_vectorTwo;}

  // Modifiers
  // Add a pair. Return false if either T1 or T2 already exist in the map
  // and do not add the pair to the map
  virtual bool append(T1*, T2*); 

  // Remove an entry from the map
  // Does not delete the entry, also removes the mapped entry and 
  // returns a pointer to the mapped entry.
  // Warning!!! Uses two linear searches.  Use with care.
  virtual T2* remove1(const T1*);
  virtual T1* remove2(const T2*);


  // Modifiers

  // Clear map of entries.  Does not delete contents.
  virtual void clear();

  // Clear map of entries and delete the objects
  virtual void clearAndDestroy();
  
protected:

    // Helper functions

private:

  // Friends
  
  // Data members

  std::string *_myName;
  
  std::vector<T1*> *_vectorOne;
  std::vector<T2*> *_vectorTwo;

  AstNamedMapVector();
  
//------------------
// Static Members --
//------------------

public:

    // Selectors (const)

    // Modifiers

private:

    // Data members

};

#include  "AstNamedMapVector.cxx"


#endif







