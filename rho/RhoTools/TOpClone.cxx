//--------------------------------------------------------------------------
// File and Version Information:
// 	$Id: TOpClone.cxx,v 1.2 2002-02-01 23:00:16 marcel Exp $
//
// Description:
//	Class TOpClone
//
// Environment:
//	Software developed for the BaBar Detector at the SLAC B-Factory.
//
// Author List:
//	Gautier Hamel de Monchenault - CEN Saclay 
//
// History (add to end):
//      Gautier   March, 1999  - creation
//
// Copyright Information:
//	Copyright (C) 1999	       CEA - Centre d'Etude de Saclay
//
// ROOT Version by Marcel Kunze, RUB
//------------------------------------------------------------------------

#include "RhoTools/TOpClone.h"
#include "RhoBase/TFactory.h"
#include "RhoBase/TCandidate.h"

ClassImp(TOpClone)

TBuffer &operator>>(TBuffer &buf, TOpClone *&obj)
{
   obj = (TOpClone *) buf.ReadObject(TOpClone::Class());
   return buf;
}

//
// Constructor
//

TOpClone::TOpClone()
{
}

//--------------
// Destructor --
//--------------

TOpClone::~TOpClone() 
{
}

TCandidate*
TOpClone::Create( const TCandidate& cand ) 
{
    // make a copy of the candidate
    TCandidate* newCand = TFactory::Instance()->NewCandidate(cand);
    return newCand;
}

