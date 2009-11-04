//----------------------------------------------------------------
// File and Version Information:
// 	$Id: AstSTLTwoWayMatch.cc,v 1.2 2006/06/30 11:24:22 steinke Exp $
//
// Description:
//      .cc file for the AstSTLTwoWayMatch class to keep the HP linker 
//     happy.  No code should be added here - add it to the .hh 
//     file.
//   
// Author List:
//      Phil Strother         April 97
//
// Bertram Kopf (RUB)  migrated to PandaRoot
//---------------------------------------------------------------

//-----------------------
// This Class's Header --
//-----------------------

#include "AstSTLTwoWayMatch.h"
#include "TError.h"
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


template <class MatchKey1, class MatchKey2, class MatchQuality> 
AstSTLTwoWayMatch<MatchKey1,MatchKey2,MatchQuality>::
AstSTLTwoWayMatch(unsigned (*key1HashFunction)(const MatchKey1 &),
	       unsigned (*key2HashFunction)(const MatchKey2 &),
	       bool deleteQualityObjectsAtDestruct){

  _theMatchDict1 = new std::map<MatchKey1*, std::map<MatchKey2*, MatchQuality*, PndPtrLess>*, PndPtrLess>;
  _theMatchDict2 = new std::map<MatchKey2*, std::map<MatchKey1*, MatchQuality*, PndPtrLess>*, PndPtrLess>;
  _deleteQuality = deleteQualityObjectsAtDestruct;
}

//--------------
// Destructor --
//--------------

template <class MatchKey1, class MatchKey2, class MatchQuality> 
AstSTLTwoWayMatch<MatchKey1,MatchKey2,MatchQuality>::~AstSTLTwoWayMatch(){
  
  // The class has responsibility for all the dictionaries associating 
  // matched objects and their consistencies, and for the consistencies 
  // themselves but *not* the matched objects.

  typename std::map<MatchKey1*, std::map<MatchKey2*,MatchQuality*,PndPtrLess>*, PndPtrLess>::iterator
    dictionaryCleaner = _theMatchDict1->begin();
  while(dictionaryCleaner != _theMatchDict1->end()){
    
    // For each match we have to......
    
    // .....loop over the MatchQuality map and delete the contents.....

    if (_deleteQuality){
      typename std::map<MatchKey2*, MatchQuality*, PndPtrLess>::iterator
	matchQualityCleaner = dictionaryCleaner->second->begin();
      while (matchQualityCleaner != dictionaryCleaner->second->end()) {
	delete matchQualityCleaner->second;
	++matchQualityCleaner;
      }
    }
    // ..... and then delete the dictionary itself

    delete dictionaryCleaner->second;
    ++dictionaryCleaner;
  }

  typename std::map<MatchKey2*,std::map<MatchKey1*,MatchQuality*,PndPtrLess>*, PndPtrLess>::iterator
    dictionaryCleaner2 = _theMatchDict2->begin();

  while (dictionaryCleaner2 != _theMatchDict2->end()) {
    delete dictionaryCleaner2->second;
    ++dictionaryCleaner2;
  }

  _theMatchDict1->clear();
  delete _theMatchDict1;
  _theMatchDict1=0;
  _theMatchDict2->clear();
  delete _theMatchDict2;
  _theMatchDict2=0;
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


template <class MatchKey1, class MatchKey2, class MatchQuality> 
bool 
AstSTLTwoWayMatch<MatchKey1,MatchKey2,MatchQuality>::contains(const MatchKey1 *theKey,
							   const MatchKey2 *theValue)
  const 
{
  bool contained = false;
  typename std::map<MatchKey1*, std::map<MatchKey2*, MatchQuality*, PndPtrLess>*, 
      PndPtrLess>::iterator position;
  if ((position = _theMatchDict1->find((MatchKey1 *)theKey)) != _theMatchDict1->end()) 
      if (position->second->find((MatchKey2 *)theValue) != position->second->end()) contained=true;
  return contained;
}

template <class MatchKey1, class MatchKey2, class MatchQuality> 
size_t
AstSTLTwoWayMatch<MatchKey1,MatchKey2,MatchQuality>::entries1(const MatchKey1 *theKey) const{

  typename std::map<MatchKey1*, std::map<MatchKey2*, MatchQuality*, PndPtrLess>*, 
      PndPtrLess>::iterator position;
  if ((position = _theMatchDict1->find((MatchKey1 *)theKey)) == _theMatchDict1->end())
    return 0;
  else {
    return position->second->size();
  }
}
  
template <class MatchKey1, class MatchKey2, class MatchQuality> 
size_t
AstSTLTwoWayMatch<MatchKey1,MatchKey2,MatchQuality>::entries2(const MatchKey2 *theKey) const{

  typename std::map<MatchKey2*, std::map<MatchKey1*, MatchQuality*, PndPtrLess>*, 
      PndPtrLess>::iterator position;
  if ((position = _theMatchDict2->find(theKey)) == _theMatchDict2->end())
    return 0;
  else {
    return position->second->size();
  }
}


template <class MatchKey1, class MatchKey2, class MatchQuality> 
const MatchQuality *
AstSTLTwoWayMatch<MatchKey1,MatchKey2,MatchQuality>::getMatchQuality(const MatchKey1 *theKey, 
								  const MatchKey2 *theValue)
 const {

  // This returns the MatchQuality between the matched pair offered, or 0 (null) if
  // either the key or the value cannot be found

  typename std::map<MatchKey1*, std::map<MatchKey2*, MatchQuality*, PndPtrLess>*, 
      PndPtrLess>::iterator position;
  if ((position = _theMatchDict1->find((MatchKey1 *)theKey)) == _theMatchDict1->end())
      return 0;
  else {
    typename std::map<MatchKey2*, MatchQuality*, PndPtrLess>::iterator 
      innerPos = position->second->find((MatchKey2 *)theValue);
    if (innerPos == position->second->end())
      return 0;
    else
      return innerPos->second;
  }
}
  

template <class MatchKey1, class MatchKey2, class MatchQuality> 
const std::map<MatchKey2*, MatchQuality*, PndPtrLess> *
AstSTLTwoWayMatch<MatchKey1,MatchKey2,MatchQuality>::findMatchSet1(const MatchKey1 *theKey) 
  const{

  // Returns the set of matched values for the given key, or 0 if none can be found
  
  typename std::map<MatchKey1*, std::map<MatchKey2*, MatchQuality*, PndPtrLess>*, 
      PndPtrLess>::iterator position;
  if ((position = _theMatchDict1->find((MatchKey1 *)theKey)) == _theMatchDict1->end())
      return 0;
  return position->second;
}

  
template <class MatchKey1, class MatchKey2, class MatchQuality> 
const std::map<MatchKey1*, MatchQuality*, PndPtrLess> *
AstSTLTwoWayMatch<MatchKey1,MatchKey2,MatchQuality>::findMatchSet2(const MatchKey2 *theKey) 
  const{

  // Returns the set of matched values for the given key, or 0 if none can be found
  
  typename std::map<MatchKey2*, std::map<MatchKey1*, MatchQuality*, PndPtrLess>*, 
      PndPtrLess>::iterator position;
  if ((position = _theMatchDict2->find((MatchKey2 *)theKey)) == _theMatchDict2->end())
      return 0;
  return position->second;
}

    
//-------------
// Modifiers --
//-------------

template <class MatchKey1, class MatchKey2, class MatchQuality> void 
AstSTLTwoWayMatch<MatchKey1,MatchKey2,MatchQuality>::insertMatch(MatchKey1 *theKey, 
							      MatchKey2 *theValue, 
							      MatchQuality *theMatchQuality)
{
  
  typename std::map<MatchKey1*, std::map<MatchKey2*, MatchQuality*, PndPtrLess>*, 
      PndPtrLess>::iterator position;
  if ((position = _theMatchDict1->find(theKey)) != _theMatchDict1->end()) {

    std::map<MatchKey2*, MatchQuality*, PndPtrLess> *theAssocDictionary = 
      position->second;

    // We already have the key, dictionary pair
    // Check to see if we also have this match - thanks due to Bob J for spotting
    // this leak.
    typename std::map<MatchKey2*, MatchQuality*, PndPtrLess>::iterator assocPosition;
    if ((assocPosition = theAssocDictionary->find(theValue)) != theAssocDictionary->end()) {
	MatchQuality *existingMQ = assocPosition->second;
      // We need to delete the old MatchQuality object for which we have
      // responsibility, because RW will simply overwrite it.
      if (theMatchQuality!=existingMQ && _deleteQuality)  delete existingMQ;
    }
    (*theAssocDictionary)[theValue] = theMatchQuality;

  } else {
    
    // The key is new to us.  Create a new dictionary, and put all the new 
    // information into our collection

    std::map<MatchKey2*, MatchQuality*, PndPtrLess> *theNewDictionary1 = 
      new std::map<MatchKey2*, MatchQuality*, PndPtrLess>;

    
    // The following shouldn't really be needed, but for safety...
    assert(theNewDictionary1!=0);

    theNewDictionary1->insert(typename std::map<MatchKey2*, MatchQuality*, 
			      PndPtrLess>::value_type(theValue, theMatchQuality));

    (*_theMatchDict1)[theKey] = theNewDictionary1;

  }

  typename std::map<MatchKey2*, std::map<MatchKey1*, MatchQuality*, PndPtrLess>*, 
      PndPtrLess>::iterator position2;
  if ((position2 = _theMatchDict2->find(theValue)) != _theMatchDict2->end()) {
    std::map<MatchKey1*, MatchQuality*, PndPtrLess> *theAssocDictionary1 = 
	position2->second;

    // We already have the key, dictionary pair
    // Don't need to check for memory leak since the above will 
    // have already caught it.

    (*theAssocDictionary1)[theKey] = theMatchQuality;

  } else {
    
    // The key is new to us.  Create a new dictionary, and put all the new 
    // information into our collection


    std::map<MatchKey1*, MatchQuality*, PndPtrLess> *theNewDictionary = 
      new std::map<MatchKey1*, MatchQuality*, PndPtrLess>;

    
    // The following shouldn't really be needed, but for safety...
    assert(theNewDictionary!=0);

    theNewDictionary->insert(typename std::map<MatchKey1*, MatchQuality*, 
			     PndPtrLess>::value_type(theKey, theMatchQuality));

    (*_theMatchDict2)[theValue] = theNewDictionary;

  }

}

template <class MatchKey1, class MatchKey2, class MatchQuality> size_t 
AstSTLTwoWayMatch<MatchKey1,MatchKey2,MatchQuality>::removeMatchesForKey1(const MatchKey1 *theKey){

  // Remove the matches, and clean up after ourselves
  typename std::map<MatchKey1*, std::map<MatchKey2*, MatchQuality*, PndPtrLess>*, 
      PndPtrLess>::iterator position;
  if ((position = _theMatchDict1->find((MatchKey1 *)theKey)) != _theMatchDict1->end()) {
      std::map<MatchKey2*, MatchQuality*, PndPtrLess> *theMatchQualityDict1 = 
	  position->second;
    
    // If the key is known....
    
      typename std::map<MatchKey2*, MatchQuality*, PndPtrLess>::iterator cleanUp = theMatchQualityDict1->begin();
    
    // ....delete the MatchQuality objects.....

    MatchKey2 *nextKey;
    while(cleanUp != theMatchQualityDict1->end()){
      nextKey = cleanUp->first;
      typename std::map<MatchKey2*, std::map<MatchKey1*, MatchQuality*, PndPtrLess>*, 
	  PndPtrLess>::iterator position2;
      if ((position2 = _theMatchDict2->find(nextKey)) != _theMatchDict2->end()) {
	std::map<MatchKey1*, MatchQuality*, PndPtrLess> *theMatchQualityDict2 = 
	  position2->second;
	theMatchQualityDict2->erase((MatchKey1 *)theKey);
      } else {

	Fatal("AstSTLTwoWayMatch<MatchKey1,MatchKey2,MatchQuality>::removeMatchesForKey1(const MatchKey1 *theKey)","Error:  Failed to find key in _theMatchDict2");
      }
      if (_deleteQuality) delete cleanUp->second;
      ++cleanUp;
    }

    // .....delete the Dictionary we just found......

    delete theMatchQualityDict1;

    //......and finally remove the key from the matched collection
	
    _theMatchDict1->erase((MatchKey1 *)theKey);


    return 1;
  } else {

    // If we do not recognise the key....do nothing
    return 0;
  }

}


template <class MatchKey1, class MatchKey2, class MatchQuality> size_t 
AstSTLTwoWayMatch<MatchKey1,MatchKey2,MatchQuality>::removeMatchesForKey2(const MatchKey2 *theKey){

  // Remove the matches, and clean up after ourselves
  typename std::map<MatchKey2*, std::map<MatchKey1*, MatchQuality*, PndPtrLess>*, 
      PndPtrLess>::iterator position;
  if ((position = _theMatchDict2->find(theKey)) != _theMatchDict2->end()) {
      std::map<MatchKey1*, MatchQuality*, PndPtrLess> *theMatchQualityDict1 = 
	  position->second;
    
    // If the key is known....
    
      typename std::map<MatchKey1*, MatchQuality*, PndPtrLess>::iterator cleanUp = 
	theMatchQualityDict1->begin();

    // ....delete the MatchQuality objects.....

    while(cleanUp != theMatchQualityDict1->end()){

      std::map<MatchKey2*, MatchQuality*, PndPtrLess> *theMatchQualityDict2 = 
	_theMatchDict1->find(cleanUp->first)->second;
      theMatchQualityDict2->erase(theKey);

      if (_deleteQuality) delete cleanUp->second;
      ++cleanUp;
    }

    // .....delete the Dictionary we just found......

    delete theMatchQualityDict1;

    //......and finally remove the key from the matched collection
	
    _theMatchDict2->erase(theKey);


    return 1;
  } else {

    // If we do not recognise the key....do nothing
    return 0;
  }

}



template <class MatchKey1, class MatchKey2, class MatchQuality> size_t 
AstSTLTwoWayMatch<MatchKey1,MatchKey2,MatchQuality>::removeSpecificMatch(MatchKey1 *theKey, 
								      MatchKey2 *theValue){

  std::map<MatchKey1*, MatchQuality*, PndPtrLess> *theMatchQualityDict2 = 
    _theMatchDict2->find(theValue)->second;
  
  typename std::map<MatchKey1*, std::map<MatchKey2*, MatchQuality*, PndPtrLess>*, 
      PndPtrLess>::iterator position;
  if ((position = _theMatchDict1->find(theKey)) != _theMatchDict1->end()) {
      std::map<MatchKey2*, MatchQuality*, PndPtrLess> *theMatchQualityDict1 = 
	  position->second;
    if (theMatchQualityDict1->size()==1){

      // We are down to our last association.  Pull out the entire workings for this key

      return removeMatchesForKey1(theKey);

    } else {
      
      // There is more than one match left for this key.  Only remove the match we want
      // (if we have it already - but that is taken care of by RW)
      
      MatchQuality *smallFish;

      typename std::map<MatchKey2*, MatchQuality*, PndPtrLess>::iterator position2;
      if ((position2 = theMatchQualityDict1->find(theValue)) != 
	  theMatchQualityDict1->end() && _deleteQuality) {
	  smallFish  = position2->second;
	  delete smallFish;
      }

      // Would be OK if we could guarantee rwnil will always be 0, but we can't

      int retVal = 0;
      if (theMatchQualityDict1->find(theValue) != theMatchQualityDict1->end()) {
	  theMatchQualityDict1->erase(theValue);
	  retVal = 1;
      }
      if (theMatchQualityDict2->find(theKey) != theMatchQualityDict2->end()) {
	  theMatchQualityDict2->erase(theKey);
	  retVal = 1;
      }

      return retVal;
      
    }     
  } else {
    
    // We do not recognise the key.  Do nothing
    return 0;
  }
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
