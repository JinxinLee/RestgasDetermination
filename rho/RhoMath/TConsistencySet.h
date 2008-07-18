#ifndef TCONSISTENCYSET_H
#define TCONSISTENCYSET_H
//////////////////////////////////////////////////////////////////////////
//                                                                      //
// TConsistencySet			    				//
//                                                                      //
// Combined consistency class						//
//                                                                      //
// Author: Marcel Kunze, RUB, Nov. 99					//
// Copyright (C) 1999-2001, Ruhr-University Bochum.			//
//                                                                      //
//////////////////////////////////////////////////////////////////////////

// Description:
//      A set of consistency objects. Typically used to define the genealogy 
//      of a CombinedConsistency object, i.e. contains a list of 
//      Consistencies the CombinedConsistency object is built from. 
//      The class provides methods to get an overlap, union etc. of two sets
//
//      The current implementation requires a string label to be attached 
//      to each Consistency object; it typically represents a PID subsystem 
//      "svt", "dch", etc.; case-insensitive.

#include "THashList.h"
#include "RhoMath/TConsistency.h"

class TConsistencySet : public TObject {
public:
    
    // default constructor; creates an empty set
    TConsistencySet();
    
    // copy constructor
    TConsistencySet(const TConsistencySet& rhs) {}
    
    virtual ~TConsistencySet();
    
    // assignment
    //ConsistencySet& operator= (const ConsistencySet& rhs);
    
    // equality -- only checks the labels of parent objects, not their values
    virtual Bool_t operator==(TConsistencySet& rhs) ;
    virtual Bool_t operator!=(TConsistencySet& rhs) ;
    
    // modifiers
    
    // returns false if the operation cannot be performed (e.g., the label 
    // is already in the list
    virtual Bool_t Add( const char*, TConsistency& );
    
    // merge two sets. Tests for overlaps
    virtual Bool_t Combine(TConsistencySet& other);
    
    // clear the lists
    virtual void Reset();
    
    // accessors
    size_t NParents() const; 
    TConsistency* GetConsistency( size_t index );
    TConsistency* GetLabelConsistency( const char* );
    const char* GetLabel( size_t index );
    Double_t      WorstSignificance(); 
    const char* WorstSignificanceLabel();
    
    // overlaps -- returns a subset of self that overlaps with other
    // NOTE: the returned object is new'ed, client is responsible for deletion
    virtual TConsistencySet* Overlap(TConsistencySet& other);
    
    // print method
    virtual void PrintOn( std::ostream& o=std::cout ) const;

protected:
    
    //  helper functions
    Bool_t   GetLabelIndex_( const char*, size_t& index );
    Bool_t   WorstSignificanceIndex_( size_t& index );
    
private:
    
    THashList fConsistencyList; //! Do not stream
    
    ClassDef(TConsistencySet,1) // Set of consistency objects	
};

#endif




