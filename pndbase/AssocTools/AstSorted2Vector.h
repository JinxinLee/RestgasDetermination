//-------------------------------------------------------------------
// File and Version Information:
// 	$Id: AstSorted2Vector.hh,v 1.1.1.1 2005/03/29 16:49:05 steinke Exp $
//
// Description:
//      Bi-directional 1-1 mapping object
//      (Based on AstMap2 by Luca Lista)
//   
// Author List:
//      Roland Martin         5th May 1998
//
// Bertram Kopf (RUB)  migrated to PandaRoot
//-------------------------------------------------------------------
#ifndef ASTSORTED2VECTOR_HH
#define ASTSORTED2VECTOR_HH

//-------------
// C Headers --
//-------------
#include <stddef.h>

//---------------
// C++ Headers --
//---------------
#include <vector>

//----------------------
// Base Class Headers --
//----------------------

//-------------------------------
// Collaborating Class Headers --
//-------------------------------
template < class T, class U > class AstSortableAssoc;

//              ---------------------
//              -- Class Interface --
//              ---------------------


template<class T, class Q> 
class AstSorted2Vector {
  //--------------------
  // Declarations     --
  //--------------------

  // Typedefs, consts, and enums

  //--------------------
  // Instance Members --
  //--------------------

public:
  // Constructors
  // The function given to the constructors returns true if the first 
  // argyument is less than the second.
  AstSorted2Vector(bool (*)(const Q &, const Q&));
  AstSorted2Vector(bool (*)(const Q &, const Q&),
		   size_t length );

  // Detsructor
  virtual ~AstSorted2Vector();

  // Selectors (const)
  
  // Linear search function - not quick!
  bool contains(const T*, const Q*) const;

  // Nonlinear search function
  const T* find( const Q& ) const;

  int entries() const;

  const T& operator[]( size_t index ) const;
  const T& operator()( size_t index ) const;
  
  const Q& quality( size_t index ) const;
  const Q& qualityFast( size_t index ) const;

  // Modifiers
  // Add a pair. Return false if either T or Q already exist in the map
  // and do not add the pair to the map
  void insert(T*, Q*); 

  bool remove(T*, Q*);

  // Clear the vector
  void clear();
  
  // Clear the vector and delete all the T's and Q's
  void clearAndDestroy();

protected:

private:

  std::vector<AstSortableAssoc<T, Q>* > *_vector;
  bool (*_lessThanFunction)(const Q & q1, const Q& q2);

  
};

#include  "AstSorted2Vector.cxx"

#endif // ASTSORTED2VECTOR_HH





