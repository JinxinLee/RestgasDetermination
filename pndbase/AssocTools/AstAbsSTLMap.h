//----------------------------------------------------------------
// File and Version Information:
// 	$Id: AstAbsSTLMap.hh,v 1.1.1.1 2005/03/29 16:49:05 steinke Exp $
//
// Description:
//      Abstract map interface providing set operations
//        
//   
// Author List:
//      Matthias Steinke
//
//  Bertram Kopf (RUB)  migrated to PandaRoot
//---------------------------------------------------------------

#ifndef ASTABSSTLMAP_HH
#define ASTABSSTLMAP_HH

//-------------
// C Headers --
//-------------

//---------------
// C++ Headers --
//---------------
#include <set>

//----------------------
// Base Class Headers --
//----------------------

//-------------------------------
// Collaborating Class Headers --
//-------------------------------

//------------------------------------
// Collaborating Class Declarations --
//------------------------------------

//              ---------------------
//              -- Class Interface --
//              ---------------------


template<class T1, class T2>
class AstAbsSTLMap{
public:
 
  AstAbsSTLMap(size_t buckets=50);
  AstAbsSTLMap(unsigned (*)(T1 *const &), size_t buckets=50);

  virtual ~AstAbsSTLMap();
  

  // Append sorts out what to do with the T1 (for hash set operations)
  // then leaves the decision on what to do with the pair (T1,T2) to
  // the derived classes
  void append(T1* const&, T2* const&);

protected:

  // Given a set of T1*, find intersection set and return it.
  bool findIntersectionSet(const std::set<T1*> &, std::set<T1*> &returnedSet) const;

  // Given a set of T1*, find difference set and return it
  bool findDifferenceSet(const std::set<T1*> &, std::set<T1*> &) const;

  virtual void appendToMap(T1* const&, T2* const&)=0;

  static unsigned hashFunction(T1 *const &);
  unsigned (*_myHashFunction)(T1 *const &);

  std::set<T1*> *hashSetT1() {return _hashSetT1;}
  const std::set<T1*> *hashSetT1() const {return _hashSetT1;}

private:

  std::set<T1*> *_hashSetT1;
  size_t _buckets;

};


//--------------------
// Member functions --
//--------------------

//---------------
// Constructor --
//---------------

#include  "AstAbsSTLMap.cxx"

#endif
