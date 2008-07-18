#ifndef TCANDLISTITERATOR_H
#define TCANDLISTITERATOR_H
//////////////////////////////////////////////////////////////////////////
//                                                                      //
// TCandListIterator							//
//                                                                      //
// Iterator class for TCandList						//
//									//
// Author List:								//
// Marcel Kunze,  RUB, Feb. 99						//
// Copyright (C) 1999-2001, Ruhr-University Bochum.			//
//									//
//////////////////////////////////////////////////////////////////////////

#include "TObject.h"

class TCandidate;
class TCandList;

class TCandListIterator : public TObject {
    
public:
    
    TCandListIterator(TCandList &);
    // Constructor taking a corresponding list as argument. Starting at the 
    // first element.
    
    TCandListIterator(const TCandListIterator &);
    // Copy constructor.
    
    ~TCandListIterator();
    
    TCandidate * Next();
    // Returns a pointer to the current object in the associated list, moving
    // forward to the next. Returns 0 if all objects are done.
    
    TCandidate * Previous();
    // Moves backward one step in the list and returns the object found there.
    // If current object is the first in the list, no stepping is done and 0 is
    // returned.
    
    TCandidate * Current();
    // Returns a pointer to the current object in the associated list,
    // without incrementing the index.
    
    // public:
    
    int Index() const;
    
    void Rewind();
    // Rewinds the iterator to the first element of the list.
    
    void Skip(int);
    // Move iterator forward or backward.
    
    void SkipAll();
    // Move iterator past last element.
    
private:
    
    TCandList * l; //! Pointer to the associated list.
    
    int i; //! Current position in the associated list.
    
public:
    ClassDef(TCandListIterator,1) // Iterator for TCandList
};


#endif
