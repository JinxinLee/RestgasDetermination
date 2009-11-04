//----------------------------------------------------------------
// File and Version Information:
// 	$Id: AstSTLMatch.cc,v 1.1.1.1 2005/03/29 16:49:05 steinke Exp $
//
// Description:
//      Implementation of AstSTLMatch class.  For more info see .hh
//   
// Author List:
//      Matthias Steinke March 2002
//
// Bertram Kopf (RUB)  migrated to PandaRoot
//---------------------------------------------------------------

//-----------------------
// This Class's Header --
//-----------------------

#include "AstSTLMatch.h"

//-------------
// C Headers --
//-------------
extern "C" {
}

//---------------
// C++ Headers --
//---------------
#include <map>
#include "CollectionUtils.h"

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


template <class MatchKey, class MatchedValue, class MatchQuality> 
AstSTLMatch<MatchKey,MatchedValue,MatchQuality>::AstSTLMatch()
{
  _theMatchDict = new 
    std::map<MatchKey*, 
    std::map<MatchedValue*, MatchQuality*, PndPtrLess>*, PndPtrLess>;

  _valueHashFunction  = hashFunction2;

  _matchQualityCleanerFunction = 0;
}

template <class MatchKey, class MatchedValue, class MatchQuality> 
AstSTLMatch<MatchKey,MatchedValue,MatchQuality>::
AstSTLMatch(unsigned (*keyHashFunction)(const MatchKey &),
	 unsigned (*valueHashFunction)(const MatchedValue &))
{
  _theMatchDict = new 
    std::map<MatchKey*, 
    std::map<MatchedValue*, MatchQuality*, PndPtrLess>*, PndPtrLess>;

  _valueHashFunction  = valueHashFunction;

  _matchQualityCleanerFunction = 0;
}

//--------------
// Destructor --
//--------------


template <class MatchKey, class MatchedValue, class MatchQuality> 
AstSTLMatch<MatchKey,MatchedValue,MatchQuality>::~AstSTLMatch(){
  
  // The class has responsibility for all the dictionaries associating 
  // matched objects and their consistencies, 
  // and for the consistencies themselves
  // but *not* the matched objects.

  typename std::map<MatchKey*, 
    std::map<MatchedValue*,MatchQuality*,PndPtrLess>*, PndPtrLess >::iterator
    dictionaryCleaner = _theMatchDict->begin();
  while (dictionaryCleaner != _theMatchDict->end()){
    
    // For each match we have to......
    
    // .....loop over the MatchQuality map and delete the contents.....

    typename std::map<MatchedValue*, MatchQuality*, PndPtrLess>::iterator
      matchQualityCleaner = dictionaryCleaner->second->begin();
    while (matchQualityCleaner != dictionaryCleaner->second->end()) {
      
      // .....call a cleaner function for cases where MatchQuality is an object
      // which just contains pointers to more objects.  e.g. MatchQuality may 
      // be a vector of pointers.
      if( _matchQualityCleanerFunction != 0 ) {
	(*_matchQualityCleanerFunction)( *(matchQualityCleaner->second) );
      }
      
      delete matchQualityCleaner->second;
      ++matchQualityCleaner;
    }    
    
    // ..... and then delete the dictionary itself

    delete dictionaryCleaner->second;
    ++dictionaryCleaner;
  }

  delete _theMatchDict;
  _theMatchDict=0;
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


template <class MatchKey, class MatchedValue, class MatchQuality> 
bool AstSTLMatch<MatchKey,MatchedValue,MatchQuality>::contains(const MatchKey *theKey,
							    const MatchedValue *theValue)const {
  bool contained = false;
  typename std::map<MatchKey*, std::map<MatchedValue*, MatchQuality*,
    PndPtrLess>*, PndPtrLess >::iterator position
    = _theMatchDict->find((MatchKey *)theKey);
  if (position != _theMatchDict->end()) {
    if (position->second->find((MatchedValue *)theValue) != position->second->end()) contained=true;
  }
  return contained;
}

template <class MatchKey, class MatchedValue, class MatchQuality> 
const MatchQuality *
AstSTLMatch<MatchKey,MatchedValue,MatchQuality>::getMatchQuality(const MatchKey *theKey, 
						const MatchedValue *theValue) const{

  // This returns the MatchQuality between the matched pair offered, or 0 (null) if
  // either the key or the value cannot be found
  
  typename std::map<MatchKey*, std::map<MatchedValue*, MatchQuality*, PndPtrLess>*, 
    PndPtrLess >::iterator position = _theMatchDict->find((MatchKey *)theKey);
  if (position != _theMatchDict->end()) {
    typename map<MatchedValue*, MatchQuality*, PndPtrLess>::iterator 
      innerPos = position->second->find((MatchedValue *)theValue);
    if (innerPos != position->second->end())
      return innerPos->second;
  }

  return 0;
}
  

template <class MatchKey, class MatchedValue, class MatchQuality> 
const std::map<MatchedValue*, MatchQuality*, PndPtrLess> *
AstSTLMatch<MatchKey,MatchedValue,MatchQuality>::findMatchSet(const MatchKey *theKey) const{

  // Returns the set of matched values for the given key, or 0 if none can be found
  typename std::map<MatchKey*, std::map<MatchedValue*, MatchQuality*, PndPtrLess>*, 
    PndPtrLess >::iterator position;
  if ((position = _theMatchDict->find((MatchKey *)theKey)) != _theMatchDict->end())
    return position->second;
  else
    return 0;
}

  
template <class MatchKey, class MatchedValue, class MatchQuality> 
const std::map<MatchKey*, std::map<MatchedValue*, MatchQuality*,PndPtrLess>*, PndPtrLess > *
AstSTLMatch<MatchKey,MatchedValue,MatchQuality>::getTheMatchDictionary() const{

  return _theMatchDict;
}
    
//-------------
// Modifiers --
//-------------

template <class MatchKey, class MatchedValue, class MatchQuality> void
AstSTLMatch<MatchKey,MatchedValue,MatchQuality>::clearAndDestroy()
{
 
  typename std::map<MatchKey*, std::map<MatchedValue*,MatchQuality*,PndPtrLess>*, PndPtrLess>::iterator
    dictionaryCleaner = _theMatchDict->begin();
  
  while (dictionaryCleaner !=_theMatchDict->end()) {
    
    typename std::map<MatchedValue*, MatchQuality*, PndPtrLess>::iterator 
      matchQualityCleaner = dictionaryCleaner->second->begin();
    
    while (matchQualityCleaner != dictionaryCleaner->second->end()) {
      
      // delete the MatchedValue
      delete matchQualityCleaner->first;
      ++matchQualityCleaner;

    } // End while (matchQualityCleaner)
    dictionaryCleaner->second->clear();

    // delete the MatchKey
    delete dictionaryCleaner->first;
    ++dictionaryCleaner;
  } // End while (dictionaryCleaner)
  _theMatchDict->clear();

}

template <class MatchKey, class MatchedValue, class MatchQuality> void 
AstSTLMatch<MatchKey,MatchedValue,MatchQuality>::insertMatch(MatchKey *theKey, 
							  MatchedValue *theValue, 
							  MatchQuality *theMatchQuality){
  
  typename std::map<MatchKey*, std::map<MatchedValue*, MatchQuality*, PndPtrLess>*, 
    PndPtrLess >::iterator position;
  if ((position = _theMatchDict->find(theKey)) != _theMatchDict->end()) {

    std::map<MatchedValue*, MatchQuality*, PndPtrLess> *theAssocDictionary = 
      position->second;

    // We already have the key, dictionary pair
    // Check to see if we also have this match - thanks due to Bob J for spotting
    // this leak.

    typename std::map<MatchedValue*, MatchQuality*, PndPtrLess>::iterator mqPosition;
    if ((mqPosition = theAssocDictionary->find(theValue)) != theAssocDictionary->end()) {
      MatchQuality *existingMQ = mqPosition->second;
      // We need to delete the old MatchQuality object for which we have
      // responsibility, because RW will simply overwrite it.
      if (theMatchQuality!=existingMQ)  delete existingMQ;
    }
    (*theAssocDictionary)[theValue] = theMatchQuality;

  } else {
    
    // The key is new to us.  Create a new dictionary, and put the all the new 
    // information into our collection

    std::map<MatchedValue*, MatchQuality*, PndPtrLess> *theNewDictionary = 
      new std::map<MatchedValue*, MatchQuality*, PndPtrLess>;
    
    // The following shouldn't really be needed, but for safety...
    assert(theNewDictionary!=0);

    (*theNewDictionary)[theValue] = theMatchQuality;

    (*_theMatchDict)[theKey] = theNewDictionary;

  }
}

template <class MatchKey, class MatchedValue, class MatchQuality> size_t 
AstSTLMatch<MatchKey,MatchedValue,MatchQuality>::removeMatchesForKey(MatchKey *theKey){

  // Remove the matches, and clean up after ourselves

  typename std::map<MatchKey*, std::map<MatchedValue*, MatchQuality*, PndPtrLess>*, 
    PndPtrLess >::iterator position;
  if ((position = _theMatchDict->find(theKey)) != _theMatchDict->end()) {

    std::map<MatchedValue*, MatchQuality*, PndPtrLess> *theMatchQualityDict = 
      position->second;
    
    // If the key is known....
    
    typename std::map<MatchedValue*, MatchQuality*, PndPtrLess>::iterator 
      cleanUp = theMatchQualityDict->begin();

    // ....delete the MatchQuality objects.....

    while( cleanUp != theMatchQualityDict->begin() ) {
      delete cleanUp->second;
      ++cleanUp;
    }
      
    // .....delete the Dictionary we just found......

    delete theMatchQualityDict;

    //......and finally remove the key from the matched collection
	
    _theMatchDict->erase(theKey);

    return 1;
  } else {

    // If we do not recognise the key....do nothing
    return 0;
  }

}


template <class MatchKey, class MatchedValue, class MatchQuality> size_t 
AstSTLMatch<MatchKey,MatchedValue,MatchQuality>::removeSpecificMatch(MatchKey *theKey, 
								  MatchedValue *theValue){
  typename std::map<MatchKey*, std::map<MatchedValue*, MatchQuality*, PndPtrLess>*, 
    PndPtrLess >::iterator position;
  if ((position = _theMatchDict->find(theKey)) != _theMatchDict->end()) {
    std::map<MatchedValue*, MatchQuality*, PndPtrLess> *theMatchQualityDict = 
      position->second;
  
    if (theMatchQualityDict->size()==1){

      // We are down to our last association.  Pull out the entire workings for this key

      return removeMatchesForKey(theKey);

    } else {
      
      // There is more than one match left for this key.  Only remove the match we want
      // (if we have it already - but that is taken care of by RW)
      
      if (theMatchQualityDict->erase(theValue) != 0){
	return 1;
      } else {
	return 0;
      }
    }     
  } else {
    
    // We do not recognise the key.  Do nothing
    return 0;
  }
}

//              -----------------------------------------------
//              -- Static Data & Function Member Definitions --
//              -----------------------------------------------


template <class MatchKey, class MatchedValue, class MatchQuality> unsigned
AstSTLMatch<MatchKey,MatchedValue,MatchQuality>::hashFunction1(const MatchKey & p)
{ 
  return ((unsigned long) &p >> 5) & 0x03ff; 
}

template <class MatchKey, class MatchedValue, class MatchQuality> unsigned
AstSTLMatch<MatchKey,MatchedValue,MatchQuality>::hashFunction2(const MatchedValue & p)
{ 
  return ((unsigned long) &p >> 5) & 0x03ff; 
}

//              -------------------------------------------
//              -- Protected Function Member Definitions --
//              -------------------------------------------

//              -----------------------------------------
//              -- Private Function Member Definitions --
//              -----------------------------------------

//              -----------------------------------
//              -- Internal Function Definitions --
//              -----------------------------------


