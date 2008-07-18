//--------------------------------------------------------------------------
// File and Version Information:
// 	$Id: TOpAdd4.cxx,v 1.3 2002-02-01 23:00:16 marcel Exp $
//
// Description:
//	Class TOperator
//
//	Implementation, adds 4vectors at origin
//
// Environment:
//	Software developed for the BaBar Detector at the SLAC B-Factory.
//
// Author List:
//	Bob Jacobsen     		Original Author
//
// Copyright Information:
//	Copyright (C) 1996		Lawrence Berkeley Laboratory
//
// ROOT Version by Marcel Kunze, RUB
//------------------------------------------------------------------------

#include "RhoTools/TOpAdd4.h"
#include <assert.h>
#include "RhoBase/TFactory.h"
#include "RhoBase/TFitParams.h"
#include "RhoBase/TCandidate.h"
#include "RhoBase/TCandListIterator.h"

ClassImp(TOpAdd4)

TBuffer &operator>>(TBuffer &buf, TOpAdd4 *&obj)
{
   obj = (TOpAdd4 *) buf.ReadObject(TOpAdd4::Class());
   return buf;
}

#include <iostream>
using namespace std;

static const char rcsid[] = "$Id: TOpAdd4.cxx,v 1.3 2002-02-01 23:00:16 marcel Exp $";

//----------------
// Constructors --
//----------------


//--------------
// Destructor --
//--------------

TOpAdd4::~TOpAdd4( )
{
}

//--------------
// Operations --
//--------------

void TOpAdd4::PrintOn(std::ostream& o) const {
    o << "TOpAdd4 class ";
}

std::ostream&  operator << (std::ostream& o, const TOpAdd4& a) {a.PrintOn(o); return o;};


//
// The following may be overridden, but are intended to basically work
//
TCandidate*
TOpAdd4::CreateFromList( TCandListIterator &li ) 
{
  // create a temporary sum 4vector and change
  TLorentzVector v(0,0,0,0);
  double c(0);
  TError err(4);
  UInt_t marker[4]={0,0,0,0};
 
  // now do the addition
  const TCandidate* bp;
  while ( bp = li.Next()) {
    v += bp->P4();
    c += bp->Charge();
    err+=bp->P4Err();
    marker[0] |= bp->GetMarker(0);
    marker[1] |= bp->GetMarker(1);
    marker[2] |= bp->GetMarker(2);
    marker[3] |= bp->GetMarker(3);
  }
  
  // copy info across 
  TCandidate* out = TFactory::Instance()->NewCandidate(TCandidate(v,c));
  SetP4Err(out, err);
  out->SetMarker(marker[0],0);
  out->SetMarker(marker[1],1);
  out->SetMarker(marker[2],2);
  out->SetMarker(marker[3],3);
  
  // add the daughter info
  li.Rewind();
  while ( bp = li.Next()) AddDaughterLink( *out, bp); 

  return out;
}




