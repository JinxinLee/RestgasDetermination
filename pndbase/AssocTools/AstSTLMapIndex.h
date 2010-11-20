//-------------------------------------------------------------------
// File and Version Information:
// 	$Id: AstSTLMapIndex.hh,v 1.1.1.1 2005/03/29 16:49:05 steinke Exp $
//
// Description:
//      Given a HepAList or std::vector, provides a fast
//      map between the objects in the vector and the 
//      index within the vector.
//   
// Author List:
//      Matthias Steinke
//
// Bertram Kopf (RUB)  migrated to PandaRoot
//-------------------------------------------------------------------
#ifndef ASTSTLMAPINDEX_HH
#define ASTSTLMAPINDEX_HH

//-------------
// C Headers --
//-------------

//---------------
// C++ Headers --
//---------------
#include <vector>
#include <map>
#include "PndCollectionUtils.h"

//----------------------
// Base Class Headers --
//----------------------

//-------------------------------
// Collaborating Class Headers --
//-------------------------------

template <class T> class AstIndexedObject;

//              ---------------------
//              -- Class Interface --
//              ---------------------


template<class T> 
class AstSTLMapIndex {
  //--------------------
  // Declarations     --
  //--------------------

  // Typedefs, consts, and enums

  //--------------------
  // Instance Members --
  //--------------------

public:
  // Constructors
  AstSTLMapIndex(const std::vector<T*> &,
	      unsigned (*)(const T&)=AstSTLMapIndex<T>::hashFunction);

  // Detsructor
  virtual ~AstSTLMapIndex();

  // Selectors (const)

  // Contains the given object
  bool contains(const T*) const;

  // Fills given index with position in original
  // vector.  Returns true if T* can be found
  bool index(const T*, int &index) const;

  int members() const;

  
  // Modifiers
  // Append a T* to the end of the list
  virtual bool append(T*); 

  // Remove an entry from the map
  // *Expensive* and will result in indices being recalculated!!!
  virtual T* remove(const T*);

  // Clear map of entries.  Does not delete contents.
  virtual void clear();

  // Clear map of entries and delete the objects
  virtual void clearAndDestroy();

protected:

private:

  AstSTLMapIndex();

  std::map<T*, AstIndexedObject<T>*, panda::Collection::PtrLess> *_map;

  unsigned int (*_hashFun)(const T&);

  // default hashFunctions (based on pointer value)
  static unsigned hashFunction(const T& p);

};

#include  "AstSTLMapIndex.cxx"


#endif // ASTMAPINDEX_HH





