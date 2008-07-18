//--------------------------------------------------------------------------
// File and Version Information:
// 	$Id: TCandPtr.cxx,v 1.1.1.1 2001-05-04 16:19:49 marcel Exp $
//
// Description:
//	TCandPtr
//      a class to summarize in a simple and compact format mapping between:
//      1) many TCandidates and a composite (which is not saved)
//      2) one TCandidates and several of them in the same list
//    
//      the default is option 1 and one can switch to option one with
//      setOneToMany()

//
// Environment:
//	Software developed for the BaBar Detector at the SLAC B-Factory.
//
// Author List:
//	Riccardo Faccini                
//
// History (add to end):
//      R.Faccini   May, 1999  - creation
//
// Copyright Information:
//	Copyright (C) 1999	       U.C.S.D
//
// ROOT Version by Marcel Kunze, RUB
//------------------------------------------------------------------------
#include "RhoTools/TCandPtr.h"
#include "RhoBase/TCandidate.h"
#include "TParticlePDG.h"

ClassImp(TCandPtr)

//
// Constructor
//

TCandPtr::TCandPtr( const TParticlePDG* pdt )
  : _oneToMany(kFALSE) 
  , _lundId(0)
{
  if ( pdt != 0 ) _lundId = pdt->PdgCode();
}
 

UInt_t
TCandPtr::NDaughters(  ) const
{
  return _oneToMany? _indices.GetSize()-1:_indices.GetSize();
}

Bool_t
TCandPtr::SetMother(Int_t im)
{
  if(!_oneToMany || ( _indices.GetSize() != 0)) return kFALSE;
  AddDaughter(im);
  return kTRUE;
}

Bool_t
TCandPtr::GetMother(Int_t &im)
{
  if(!_oneToMany || ( _indices.GetSize() == 0)) return kFALSE;
  im = _indices.At(0);
  return kTRUE;
}

Int_t
TCandPtr::operator()( Int_t i )
{
  if (i<NDaughters())
    return _oneToMany?  _indices.At(i+1):_indices.At(i);
  else
    return 0;
}

void 
TCandPtr::AddDaughter(Int_t ind){_indices.AddAt(ind,_indices.GetSize());}
