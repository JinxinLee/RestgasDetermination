//--------------------------------------------------------------------------
// File and Version Information:
// 	$Id: AstSTLTwoWayMatch.hh,v 1.1.1.1 2005/03/29 16:49:05 steinke Exp $
//
// Description:
//	Class AstSTLTwoWayMatch<type 1, type 2, type 3>
//      Templated class to hold associations between matched objects.
//      It is envsiaged that one event's worth of associations will be stored
//      in a given AstSTLTwoWayMatch object at any one time.  Associations are 
//      in the form  of a match between an object of type 1, and a set of 
//      objects of type 2, with some measure of the quality of the 
//      associtation between a given pair of objects (type 1, type 2) provided
//      by the type 3 object for that pair. 
//
//      In effect, this is a hash dictionary of hash dictionaries.
//
//      As usual with templates, this is the end of the road. No inheritance
//      allowed. 
//
//      There should be a lot more const's in this class.  The fact that there 
//      aren't is because I still don't understand the putting const's into
//      RW problem.  Help would be appreciated!
//
// Environment:
//	Software developed for the BaBar Detector at the SLAC B-Factory.
//
// Author List:
//      Phil Strother                     Original Author
//
// Copyright Information:
//	Copyright (C) 1997                Imperial College
//
// Bertram Kopf (RUB)  migrated to PandaRoot
//------------------------------------------------------------------------

#ifndef ASTSTLTWOWAYMATCH_HH
#define ASTSTLTWOWAYMATCH_HH

//-------------
// C Headers --
//-------------
extern "C" {
}

//---------------
// C++ Headers --
//---------------

#include <assert.h>
#include <iostream>

//----------------------
// Base Class Headers --
//----------------------

//-------------------------------
// Collaborating Class Headers --
//-------------------------------

// Have to include header file.  Forward decl. doesn't work on Sun
#include <map>
#include "CollectionUtils.h"

//------------------------------------
// Collaborating Class Declarations --
//------------------------------------



//		---------------------
// 		-- Class Interface --
//		---------------------

template <class MatchKey1, class MatchKey2, class MatchQuality> 
class AstSTLTwoWayMatch {

  //--------------------
  // Declarations     --
  //--------------------

  // Typedefs, consts, and enums

  //--------------------
  // Instance Members --
  //--------------------

public:

  // Constructors

  AstSTLTwoWayMatch( unsigned (*keyHashFunction)(const MatchKey1 &),
		  unsigned (*valueHashFunction)(const MatchKey2 &),
		  bool deleteQualityObjectsAtDestruct=true);

  // Construct a match object between objects of type MatchKey1 and 
  // MatchedValues. The consistency of the match is also held in a 
  // hash dictionary, and this is why we need the second hash function.  

  // Destructor
  virtual ~AstSTLTwoWayMatch();

  // Operators
    
  
  // Selectors (const)

  const std::map<MatchKey1*, 
    std::map<MatchKey2*, MatchQuality*, PndPtrLess>*, PndPtrLess>* getTheMatchDict1() const
                                                { return _theMatchDict1; }

  const std::map<MatchKey2*, 
    std::map<MatchKey1*, MatchQuality*, PndPtrLess>*, PndPtrLess >* getTHeMatchDict2() const
                                                { return _theMatchDict2; }

  // Fix typo.

  const std::map<MatchKey2*, 
    std::map<MatchKey1*, MatchQuality*, PndPtrLess>*, PndPtrLess >* getTheMatchDict2() const
                                                { return _theMatchDict2; }

  bool contains(const MatchKey1 *, const MatchKey2 *) const;
  // Returns true if the pair can be found.

  size_t entries1(const MatchKey1*) const;
  size_t entries2(const MatchKey2*) const;

  const MatchQuality *getMatchQuality(const MatchKey1 *, 
				      const MatchKey2 *) const;
  // This returns the MatchQuality between the matched pair offered, 
  // or 0 (null) if either the key or the value cannot be found

  const std::map<MatchKey2*, MatchQuality*, PndPtrLess> *
  findMatchSet1(const MatchKey1 *) const;
  const std::map<MatchKey1*, MatchQuality*, PndPtrLess> *
  findMatchSet2(const MatchKey2 *) const;
  // Returns the set of matched values for the given key, or 
  // 0 if none can be found
  
  // Modifiers

  void insertMatch(MatchKey1*, MatchKey2 *, MatchQuality *);
  // Put a match into the collection.  NOTE!  The Match object does 
  // *NOT* assume responsibility for the MatchKey1, nor for the MatchedValue, 
  // but it *DOES* assume responsiblity for the MatchQuality.  The MatchKey1 
  // and MatchKey2 should be taken care of by the Event 
  // (if not, then what exactly are you matching???) but the MatchQuality is 
  // created by the match process, so we should "look after it" (read "Kill 
  // it mercilessly.")

  size_t removeMatchesForKey1(const MatchKey1 *);
  size_t removeMatchesForKey2(const MatchKey2 *);
  // Remove all the matches for a given Key.  The Consistencies 
  // for the Key/Value pairs
  // will be deleted. To be consistent with the RW convetion, 
  // returns 0 if the MatchKey1 
  // was *NOT* found.  Need to think about this.

  size_t removeSpecificMatch(MatchKey1 *, MatchKey2 *);
  // Remove a specific Key/Value pair and the associated MatchQuality.  
  // Returns 0 if the pair was *NOT* found.  
  // Note that the Key is *NOT* removed unless
  // it is the last such match in the list.


protected:

  // Helper functions

private:
  AstSTLTwoWayMatch();

  AstSTLTwoWayMatch(const AstSTLTwoWayMatch<MatchKey1, MatchKey2, MatchQuality>& );
  
  AstSTLTwoWayMatch<MatchKey1, MatchKey2, MatchQuality> &
  operator=( const AstSTLTwoWayMatch<MatchKey1, MatchKey2, MatchQuality>& );


  // Friends
  // Data members

  unsigned (*_key1HashFunction)(const MatchKey1 &);
  unsigned (*_key2HashFunction)(const MatchKey2 &);

  std::map<MatchKey1*, std::map<MatchKey2*, MatchQuality*, PndPtrLess>*, PndPtrLess > 
  *_theMatchDict1;

  std::map<MatchKey2*, std::map<MatchKey1*, MatchQuality*, PndPtrLess>*, PndPtrLess > 
  *_theMatchDict2;

  bool _deleteQuality;

  //------------------
  // Static Members --
  //------------------

public:

  // Selectors (const)

  // Modifiers

private:

  // Data members

};


#include  "AstSTLTwoWayMatch.cxx"

#endif // ASTSTLTWOWAYMATCH_HH




