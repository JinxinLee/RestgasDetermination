//-------------------------------------------------------------------
// File and Version Information:
// 	$Id: AstUniqMap2.hh,v 1.1.1.1 2005/03/29 16:49:06 steinke Exp $
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
#ifndef ASTUNIQMAP2_HH
#define ASTUNIQMAP2_HH

//-------------
// C Headers --
//-------------

//---------------
// C++ Headers --
//---------------
#include <iostream>
#include <map>
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
class AstUniqMap2 {
  //--------------------
  // Declarations     --
  //--------------------

  // Typedefs, consts, and enums

  //--------------------
  // Instance Members --
  //--------------------

public:
  // Constructors
  AstUniqMap2();
  AstUniqMap2(unsigned (*)(const T1&), unsigned (*)(const T2&));
  AstUniqMap2(unsigned (*)(const T1&), unsigned (*)(const T2&), size_t buckets);
  AstUniqMap2(const AstUniqMap2<T1, T2> &);

  // Detsructor
  virtual ~AstUniqMap2();

  // Selectors (const)
  // Find matches, return 0 if entry does not exist
  const T2* find1(const T1*) const;
  const T1* find2(const T2*) const;

  int members() const {return _map1.size();}

  // Modifiers
  // Add a pair. Return false if either T1 or T2 already exist in the map
  // and do not add the pair to the map
  virtual bool append(T1*, T2*); 

  // Remove an entry from the map
  // Does not delete the entry, also removes the mapped entry and 
  // returns a pointer to the mapped entry.
  virtual T2* remove1(const T1*);
  virtual T1* remove2(const T2*);

  // Clear map of entries.  Does not delete contents.
  virtual void clear();

  // Clear map of entries and delete the objects
  virtual void clearAndDestroy();

protected:

private:

  std::map<T1*, T2*, PndPtrLess> _map1;
  std::map<T2*, T1*, PndPtrLess> _map2;

  // default hashFunctions (based on pointer value)
  static unsigned hashFunction1(const T1& p);
  static unsigned hashFunction2(const T2& p);

};

#include  "AstUniqMap2.cxx"

#endif // ASTUNIQMAP2_HH





