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

#include "RhoBase/RhoCandListIterator.h"
#include "RhoBase/RhoCandList.h"
#include "RhoBase/RhoCandidate.h"

ClassImp ( RhoCandListIterator )

using namespace std;

RhoCandListIterator::RhoCandListIterator ( RhoCandList& li ) :
  fList ( &li ), fCurrentIndex ( 0 ) {}

RhoCandListIterator::RhoCandListIterator ( const RhoCandListIterator& li ) :
  TObject(li), fList ( li.fList ), fCurrentIndex ( li.fCurrentIndex ) {}

RhoCandListIterator::~RhoCandListIterator() {}

RhoCandidate* RhoCandListIterator::Next()
{
  if ( fList==0 ) { return 0; }
  return fCurrentIndex < fList->GetNumberOfTracks() ? fList->Get ( fCurrentIndex++ ) : 0;
}

RhoCandidate* RhoCandListIterator::Current()
{
  return fCurrentIndex < fList->GetNumberOfTracks() ? fList->Get ( fCurrentIndex ) : 0;
}

RhoCandidate* RhoCandListIterator::Previous()
{
  if ( fCurrentIndex >= fList->GetNumberOfTracks() ) { fCurrentIndex = fList->GetNumberOfTracks(); }
  return fCurrentIndex ? fList->Get ( --fCurrentIndex ) : 0;
}

void RhoCandListIterator::Rewind()
{
  fCurrentIndex = 0;
}

void RhoCandListIterator::SkipAll()
{
  fCurrentIndex = fList->GetNumberOfTracks();
}

int RhoCandListIterator::Index() const
{
  return fCurrentIndex;
}

void RhoCandListIterator::Skip ( int si )
{
  if ( si + int ( fCurrentIndex ) < 0 ) {
    Rewind();
  } else {
    fCurrentIndex += si;
  }
}
