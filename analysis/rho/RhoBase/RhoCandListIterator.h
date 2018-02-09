#ifndef RHOCANDLISTITERATOR_H
#define RHOCANDLISTITERATOR_H
//////////////////////////////////////////////////////////////////////////
//                                                                      //
// RhoCandListIterator                                                  //
//                                                                      //
// Iterator class for RhoCandList                                       //
//                                                                      //
// Author List:                                                         //
// Marcel Kunze,  RUB, Feb. 99                                          //
// Copyright (C) 1999-2001, Ruhr-University Bochum.                     //
// Ralf Kliemt, HIM/GSI Feb.2013 (Cleanup & Restructuring)              //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#include "TObject.h"

class RhoCandidate;
class RhoCandList;

class RhoCandListIterator : public TObject
{

  public:

    RhoCandListIterator ( RhoCandList& );
    // Constructor taking a corresponding list as argument. Starting at the
    // first element.

    RhoCandListIterator ( const RhoCandListIterator& );
    // Copy constructor.

    ~RhoCandListIterator();

    RhoCandidate* Next();
    // Returns a pointer to the current object in the associated list, moving
    // forward to the next. Returns 0 if all objects are done.

    RhoCandidate* Previous();
    // Moves backward one step in the list and returns the object found there.
    // If current object is the first in the list, no stepping is done and 0 is
    // returned.

    RhoCandidate* Current();
    // Returns a pointer to the current object in the associated list,
    // without incrementing the index.

    // public:

    int Index() const;

    void Rewind();
    // Rewinds the iterator to the first element of the list.

    void Skip ( int );
    // Move iterator forward or backward.

    void SkipAll();
    // Move iterator past last element.

  private:

    RhoCandList* fList;  //! Pointer to the associated list.

    int fCurrentIndex; //! Current position in the associated list.

  public:
    ClassDef ( RhoCandListIterator,1 ) // Iterator for RhoCandList
};


#endif
