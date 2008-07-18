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

#include "RhoBase/TCandListIterator.h"
#include "RhoBase/TCandList.h"
#include "RhoBase/TCandidate.h"

ClassImp(TCandListIterator)

using namespace std;

TCandListIterator::TCandListIterator(TCandList & li) : 
  l(&li), i(0) {}

TCandListIterator::TCandListIterator(const TCandListIterator & li) : 
  l(li.l), i(li.i) {}

TCandListIterator::~TCandListIterator() {}

TCandidate * TCandListIterator::Next() {
  if (l==0) return 0;
  return i < l->GetNumberOfTracks() ? l->Get(i++) : 0;
}

TCandidate * TCandListIterator::Current() {
  return i < l->GetNumberOfTracks() ? l->Get(i) : 0;
}

TCandidate * TCandListIterator::Previous() {
  if ( i >= l->GetNumberOfTracks() ) i = l->GetNumberOfTracks();
  return i ? l->Get(--i) : 0;
}

void TCandListIterator::Rewind() {
  i = 0;
}

void TCandListIterator::SkipAll() {
  i = l->GetNumberOfTracks();
}

int TCandListIterator::Index() const {
  return i;
}

void TCandListIterator::Skip(int si) {
  if ( si + int(i) < 0 )
    Rewind();
  else
    i += si;
}
