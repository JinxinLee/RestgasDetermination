//-------------------------------------------------------------
// File and Version Information:
// 	$Id: AstSorted2Vector.cc,v 1.1.1.1 2005/03/29 16:49:05 steinke Exp $
//
// Description:
//      Implementation of AstSorted2Vector.  May or may not be included 
//      by the header file depending on BABAR_COMP_INST
//   
// Author List:
//      Roland Martin        5th May 1998
//
// Bertram Kopf (RUB)  migrated to PandaRoot
//-------------------------------------------------------------

//-----------------------
// This Class's Header --
//-----------------------
#include "AstSorted2Vector.h"
#include "TError.h"
//---------------
// C++ Headers --
//---------------
#include <vector>
#include <algorithm>
#include "PndCollectionUtils.h"
using panda::Collection::DeleteObject;

//-------------------------------
// Collaborating Class Headers --
//-------------------------------

#include "AstSortableAssoc.h"


//-----------------------------------------------------------------------
// Local Macros, Typedefs, Structures, Unions and Forward Declarations --
//-----------------------------------------------------------------------

//              ----------------------------------------
//              -- Public Function Member Definitions --
//              ----------------------------------------

//----------------
// Constructors --
//----------------
template<class T, class Q>
AstSorted2Vector<T, Q>::AstSorted2Vector(bool (*lessThan)(const Q &,
							  const Q &)) :
  _vector(new std::vector<AstSortableAssoc<T,Q>* >()),
  _lessThanFunction(lessThan)
{
}

template<class T, class Q>
AstSorted2Vector<T, Q>::AstSorted2Vector(bool (*lessThan)(const Q &,
							  const Q &),
					 size_t initCapacity) :
  _vector(new std::vector<AstSortableAssoc<T,Q>* >(initCapacity)),
  _lessThanFunction(lessThan)
{
}


//--------------
// Destructor --
//--------------
template<class T, class Q>
AstSorted2Vector<T, Q>::~AstSorted2Vector()
{
  std::for_each(_vector->begin(), _vector->end(), DeleteObject());
  _vector->clear();
  delete _vector;
}

//-------------
// Methods   --
//-------------


//-------------
// Operators --
//-------------
    
//-------------
// Selectors --
//-------------

template <class T, class Q>
bool 
AstSorted2Vector<T, Q>::contains(const T* theT, const Q* theQ) const{

  AstSortableAssoc<T, Q> *theAssoc = 
    new AstSortableAssoc<T, Q>((T*)theT, (Q*)theQ, 
			       _lessThanFunction);

  typename std::vector<AstSortableAssoc<T, Q>* >::iterator position = _vector->begin();
  while (position != _vector->end()) {
    if (*theAssoc == **position) {
      delete theAssoc;
      return true;
    }
    ++position;
  }

  delete theAssoc;
  return false;
}

template <class T, class Q>
const T*
AstSorted2Vector<T, Q>::find( const Q& toFindWith ) const
{
  size_t length = this->entries(), i=length/2;
  if ( length == 0 ) return 0;

  int result = -1;
  bool stillGoing = true;

  size_t lowerEnd = 0, upperEnd = length-1, counter = 0;
  while ( stillGoing && counter < 1000 )
    {
      size_t oldI = i;

      const Q& toTry = this->quality(i);
      if ( toFindWith == toTry )
	{
	  result = i;
	  stillGoing = false;
	}
      else
	{
	  if ( (*_lessThanFunction)( toFindWith, toTry ) )
	    {
	      upperEnd = i;
	    }
	  else
	    {
	      lowerEnd = i;
	    }
	  i = (size_t)((upperEnd - lowerEnd) / 2. + lowerEnd);
	  if ( i == oldI )
	    {
	      if ( i == upperEnd ) i--;
	      else i++;
	    }
	  if ( lowerEnd == upperEnd ) stillGoing = false;
	}
      counter++;
    }

  if ( counter == 1000 )
    Fatal("AstSorted2Vector<T, Q>::find( const Q& toFindWith ) const","Didn't find entry after 1000 iterations.");

  if ( result != -1 ) return &(*this)(result);
  return 0;
}

template <class T, class Q>
int
AstSorted2Vector<T, Q>::entries() const{
  return _vector->size();
}

template <class T, class Q>
const T&
AstSorted2Vector<T, Q>::operator[](size_t index)const{
  return (*_vector)[index]->key();
}

template <class T, class Q>
const T&
AstSorted2Vector<T, Q>::operator()(size_t index)const{
  return (*_vector)[index]->key();
}

template <class T, class Q>
const Q&
AstSorted2Vector<T, Q>::quality(size_t index)const{
  return (*_vector)[index]->quality();
}

template <class T, class Q>
const Q&
AstSorted2Vector<T, Q>::qualityFast(size_t index)const{
  return (*_vector)[index]->quality();
}

//-------------
// Modifiers --
//-------------

template <class T, class Q>
void
AstSorted2Vector<T, Q>::insert(T* theT, Q* theQ){
  AstSortableAssoc<T, Q> *theAssoc = 
    new AstSortableAssoc<T, Q>(theT, theQ, _lessThanFunction);
  // Actually this always returns true.   Oh well.
  _vector->push_back(theAssoc);
  std::sort(_vector->begin(), _vector->end(), 
	    panda::Collection::PtrLess());
}
    
template <class T, class Q>
bool 
AstSorted2Vector<T, Q>::remove(T* theT, Q* theQ){

  AstSortableAssoc<T, Q> *theAssoc = 
    new AstSortableAssoc<T, Q>(theT, theQ, _lessThanFunction);

  typename std::vector<AstSortableAssoc<T, Q>* >::iterator position = _vector->begin();
  while (position != _vector->end()) {
    if (*theAssoc == **position) {
      delete *position;
      _vector->erase(position);
      delete *theAssoc;
      return true;
    }
    ++position;
  }

  delete theAssoc;
  return false;
}

template<class T, class Q>
void AstSorted2Vector<T, Q>::clear(){
  std::for_each(_vector->begin(), _vector->end(), DeleteObject());
  _vector->clear();
}


template<class T, class Q>
void AstSorted2Vector<T, Q>::clearAndDestroy(){
  for (int i=0; i< (int) _vector->size();i++){
    (*_vector)[i]->deleteMembers();
  }
  std::for_each(_vector->begin(), _vector->end(), DeleteObject());
  _vector->clear();
}

//              -----------------------------------------------
//              -- Static Data & Function Member Definitions --
//              -----------------------------------------------


//              -------------------------------------------
//              -- Protected Function Member Definitions --
//              -------------------------------------------

//              -----------------------------------------
//              -- Private Function Member Definitions --
//              -----------------------------------------

//              -----------------------------------
//              -- Internal Function Definitions --
//              -----------------------------------


