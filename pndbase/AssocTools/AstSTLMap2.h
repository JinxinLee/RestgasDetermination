//-------------------------------------------------------------------
// File and Version Information:
// 	$Id: AstSTLMap2.hh,v 1.1.1.1 2005/03/29 16:49:05 steinke Exp $
//
// Description:
//      two -directional association map between two data types.
//
//   
// Author List:
//      Luca Lista         27 Mar 97
//
// Bertram Kopf (RUB)  migrated to PandaRoot
//-------------------------------------------------------------------
#ifndef ASTSTLMAP2_HH
#define ASTSTLMAP2_HH

//-------------
// C Headers --
//-------------

//---------------
// C++ Headers --
//---------------
#include <iostream>
#include <vector>
using std::vector;
#include <map>
using std::map;
#include "CollectionUtils.h"

//----------------------
// Base Class Headers --
//----------------------

//-------------------------------
// Collaborating Class Headers --
//-------------------------------

//              ---------------------
//              -- Class Interface --
//              ---------------------


template<class T1, class T2>
class AstSTLMap2
{
  //--------------------
  // Declarations     --
  //--------------------

  // Typedefs, consts, and enums

  //--------------------
  // Instance Members --
  //--------------------

public:
  // Constructors

  AstSTLMap2();
  AstSTLMap2(unsigned (*)(T1 *const&), unsigned (*)(T2 *const&));

  // Detsructor
  
  virtual ~AstSTLMap2();

  // Selectors (const)

  inline const map<T1*, vector<T2*>, PndPtrLess> &map1() const
    { return _map1; }
  inline const map<T2*, vector<T1*>, PndPtrLess> &map2() const
    { return _map2; }

  bool findValue1(T1*, vector<T2*>& retval) const;
  bool findValue2(T2*, vector<T1*>& retval) const;
  // Return the associated list

  T2* findFirstValue1(const T1*) const;
  T1* findFirstValue2(const T2*) const;
  // Return the first item in the associated list, or 0 if there is no match.
  
  int members1() const {return _map1.size();}
  int members2() const {return _map2.size();}
  int entries1(T1* ) const;
  int entries2(T2*) const;
  size_t removeMatchesForKey1(const T1 *);
  size_t removeMatchesForKey2(const T2 *);
  // Return the number of matches removed, 0 if the key was not found.

  // Helpers (const)

  bool contains(T1*, T2*) const ;
  // Note that the search for T2 is linear.  

  bool containsT1(T1*) const ;

  bool containsT2(T2*) const ;

  // Modifiers

  void append(T1*, T2*); 

  size_t remove(T1*, T2*);
  // Returns 0 if either T1 or T2 cannot be found.  
  // Note that the search for T2 is linear.  

  void clear();
  // Clear map of entries.  Does not delete contents.

protected:

private:

  static unsigned hashFunction1(T1 *const & p);
  static unsigned hashFunction2(T2 *const & p);
  map<T1*, vector<T2*>, PndPtrLess> _map1;
  map<T2*, vector<T1*>, PndPtrLess> _map2;

  map<T1*, vector<T2*>, PndPtrLess> &notConstMap1() const;
  map<T2*, vector<T1*>, PndPtrLess> &notConstMap2() const;
};

#include  "AstSTLMap2.cxx"


#endif // TEMPLATE_HH






