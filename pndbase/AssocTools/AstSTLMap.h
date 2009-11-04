//----------------------------------------------------------------
// File and Version Information:
// 	$Id: AstSTLMap.hh,v 1.1.1.1 2005/03/29 16:49:05 steinke Exp $
//
// Description:
//      one-directional association map ofbetween two data types.
//      the operator [] returns a std::vector<T*>
//      of the second data type.
//
//      usage:
//        T1* t1; T2* t2;
//        IfrMap<T1, T2> map;
//
//          // appends the asociation (t1, t2)
//        map[t1].append(t2); 
//
//          // returns a vector of objects associated to t1
//        std::vector<T2*> v1 = map[t1];
//
//          // checks for the asociation (t1, t2)
//        bool associated map[t1].contains(t2); 
//        
//   
// Author List:
//      Luca Lista         26 Mar 97
//
// Bertram Kopf (RUB)  migrated to PandaRoot
//---------------------------------------------------------------

#ifndef ASTSTLMAP_HH
#define ASTSTLMAP_HH

//-------------
// C Headers --
//-------------

//---------------
// C++ Headers --
//---------------
#include <map>
#include <vector>
#include <set>

//----------------------
// Base Class Headers --
//----------------------
#include "AstAbsSTLMap.h"

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
class AstSTLMap : 
  public AstAbsSTLMap<T1, T2>,
  public std::map<T1*, std::vector<T2*> >
{
public:
 
  AstSTLMap(size_t buckets=50);
  AstSTLMap(unsigned (*)(T1 *const &), size_t buckets=50);

  virtual ~AstSTLMap();
  
  // Return a list of all T2
  const std::vector<T2*> & allType2() const;


  // Given a set of T1*, return all associated T2.  No check is made on the 
  // returned vector - if of non-zero length, items are simply appended. 
  // Returns false if no matches are found
  bool findIntersectionSet(const std::set<T1*> &, std::vector<T2*> &returned) const;

  // As above but copies the intersection of the given HashSet and the known
  // table into the final argument.
  bool findIntersectionSet(const std::set<T1*> &, 
			   std::vector<T2*> &returned, 
			   std::set<T1*> &returnedSet) const;

  // As above but saves user having to think about hash functions.  Slower, as
  // it has to copy the vector into a hash set first.
  bool findIntersectionSet(const std::vector<T1*> &, std::vector<T2*> &returned) const;

  // Given a set of T1*, return all T2 for which no T1 in the given set exists.  No check is made on the 
  // returned vector - if of non-zero length, items are simply appended. 
  // Returns false if no matches are found
  bool findDifferenceSet(const std::set<T1*> &, std::vector<T2*> &returned) const;

  // As above but copies the intersection of the given HashSet and the known
  // table into the final argument.
  bool findDifferenceSet(const std::set<T1*> &, 
			 std::vector<T2*> &returned, 
			 std::set<T1*> &) const;

  // As above but saves user having to think about hash functions.  Slower, as
  // it has to copy the vector into a hash set first.
  bool findDifferenceSet(const std::vector<T1*> &, std::vector<T2*> &returned) const;

  // Compute the union with another map, modifying & returning self.
  // Note that this is a union with the T1 type ONLY.  No comparison
  // is made with the associated (vector of) T2.  Maybe one day I'll get 
  // around to writing one of those.
  AstSTLMap<T1, T2> &unionMap(const AstSTLMap<T1, T2> &);

  // Compute the union with another map, modifying & returningself.
  AstSTLMap<T1, T2> &intersectionMap(const AstSTLMap<T1, T2> &);

protected:

  void appendToMap(T1* const&, T2* const&);

private:

  std::vector<T2*> *_allT2;
  std::set<T1*> *_hashSetT1;


};


//--------------------
// Member functions --
//--------------------

//---------------
// Constructor --
//---------------

#include  "AstSTLMap.cxx"

#endif
