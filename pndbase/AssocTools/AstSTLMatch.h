//--------------------------------------------------------------------------
// File and Version Information:
// 	$Id: AstSTLMatch.hh,v 1.1.1.1 2005/03/29 16:49:05 steinke Exp $
//
// Description:
//	Class AstSTLMatch<type 1, type 2, type 3>
//      Templated class to hold associations between matched objects.
//      It is envsiaged that one event's worth of associations will be stored
//      in a given AstSTLMatch object at any one time.  Associations are in the 
//      form  of a match between an object of type 1, and a set of objects of 
//      type 2, with some measure of the quality of the associtation between 
//      a given pair of objects (type 1, type 2) provided by the type 3 
//      object for that pair. 
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
//      Matthias Steinke
//
// Bertram Kopf (RUB)  migrated to PandaRoot
//------------------------------------------------------------------------

#ifndef ASTSTLMATCH_HH
#define ASTSTLMATCH_HH

//-------------
// C Headers --
//-------------
extern "C" {
}

//---------------
// C++ Headers --
//---------------

#include <assert.h>
#include <map>
using std::map;

#include "CollectionUtils.h"

//----------------------
// Base Class Headers --
//----------------------

//-------------------------------
// Collaborating Class Headers --
//-------------------------------


//------------------------------------
// Collaborating Class Declarations --
//------------------------------------



//		---------------------
// 		-- Class Interface --
//		---------------------

template <class MatchKey, class MatchedValue, class MatchQuality>
class AstSTLMatch
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

  AstSTLMatch();
  // Default constructor.  Uses pointer based hash funcition.
  // Not recommended.  This is only public because  our compilers
  // can't hack a templated friend by forward declaration (needed for
  // DB access).

  AstSTLMatch( unsigned (*keyHashFunction)(const MatchKey &),
	    unsigned (*valueHashFunction)(const MatchedValue &));
  // Construct a match object between objects of 
  // type MatchKey and MatchedValues.
  // The consistency of the match is also held 
  // in a hash dictionary, and this is 
  // why we need the second hash function.  


  virtual ~AstSTLMatch();
  // Destructor.  Does not delete any of the MatchKey or MatchedValue
  // objects, but does delete the MatchQuality objects.
  // (See clearAndDestroy() if you do want to delete MatchKey and 
  // MatchedValue, i.e. if you are matching objects _you_ own).
  // NOTE!  If the MatchQuality object if is complex, e.g. a vector
  // of owned pointers, a function can be supplied via the
  // setMatchQualityCleanerFunction to do something more 
  // complex that simply delete the MatchQuality object.  

  void clearAndDestroy();
  // If you happen to be matching objects that do not belong 
  // to the event, you will need to delete them before the 
  // AstSTLMatch is destructed.  Calling clearAndDestroy
  // for an AstSTLMatch will ensure that memory allocated for the
  // MatchKey or MatchedValue is freed.

  void setMatchQualityCleanerFunction( void (*cleaner)( MatchQuality& ))
    { _matchQualityCleanerFunction = cleaner; }
  // Provide a function to be applied to each MatchQuality 
  // object in the destructor before that object is deleted
  // (e.g. delete the objects contained within a MQ object
  // if its destructor doesn't do that for you).

  // Operators
    
  
  // Selectors (const)
  
  bool contains(const MatchKey *, const MatchedValue *) const;
  // Returns true if the pair can be found.

  const MatchQuality *getMatchQuality(const MatchKey *, 
				      const MatchedValue *) const;
  // This returns the MatchQuality between the 
  // matched pair offered, or 0 (null) if
  // either the key or the value cannot be found

  const map<MatchedValue*, MatchQuality*, PndPtrLess> *
  findMatchSet(const MatchKey *) const;
  // Returns the set of matched values for the 
  // given key, or 0 if none can be found
  
  const map<MatchKey*, 
    map<MatchedValue*, MatchQuality*, PndPtrLess>*, PndPtrLess> 
  *getTheMatchDictionary() const;
    
  // Modifiers

  void insertMatch(MatchKey*, MatchedValue *, MatchQuality *);
  // Put a match into the collection.  
  // NOTE!  The Match object does *NOT* assume responsibility
  // for the MatchKey, nor for the MatchedValue, 
  // but it *DOES* assume responsiblity for the
  // MatchQuality.  The MatchKey and MatchedValue should be taken 
  // care of by the Event (if not, then what exactly are you 
  // matching???) but the MatchQuality is created by
  // the match process, so we should "look after it" 
  // (read "Kill it mercilessly.")

  size_t removeMatchesForKey(MatchKey *);
  // Remove all the matches for a given Key.  
  // The Consistencies for the Key/Value pairs
  // will be deleted. To be consistent with the 
  // RW convetion, returns 0 if the MatchKey 
  // was *NOT* found.  Need to think about this.

  size_t removeSpecificMatch(MatchKey *, MatchedValue *);
  // Remove a specific Key/Value pair and the associated MatchQuality.  
  // Returns 0 if the pair was *NOT* found.  Note that the 
  // Key is *NOT* removed unless it is the last such match in the list.

  

protected:

  // Helper functions

private:

  AstSTLMatch( const AstSTLMatch<MatchKey, MatchedValue, MatchQuality>& );

  AstSTLMatch<MatchKey, MatchedValue, MatchQuality> &
  operator=( const AstSTLMatch<MatchKey, MatchedValue, MatchQuality>& );

  // Friends

  // Data members

  unsigned (*_valueHashFunction)(const MatchedValue &);

  map<MatchKey*, map<MatchedValue*, MatchQuality*, PndPtrLess>*, 
    PndPtrLess > *_theMatchDict;

  void (*_matchQualityCleanerFunction)(MatchQuality& );

  //------------------
  // Static Members --
  //------------------

public:

  static unsigned hashFunction1( const MatchKey &);
  static unsigned hashFunction2( const MatchedValue &);

  // Selectors (const)

  // Modifiers

private:

  // Data members

};

#include  "AstSTLMatch.cxx"

#endif // ASTSTLMATCH_HH




