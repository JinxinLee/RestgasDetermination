//--------------------------------------------------------------------------
// File and Version Information:
// 	$Id: TCandPtr.h,v 1.2 2001-12-17 18:46:17 Marcel Exp $
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
//------------------------------------------------------------------------

#ifndef TCANDPTR_H
#define TCANDPTR_H

#include "TObject.h"
#include "TArrayS.h"

class TCandidate;
class TParticlePDG;

class TCandPtr : public TObject
{
  //--------------------
  // Instance Members --
  //--------------------
  // map between candidates
  Bool_t _oneToMany;      // is it a composite or a cluster - bump match?
  Short_t _lundId;        // Pdt IDs
  TArrayS _indices;       // Array of indices

public:
  
  TCandPtr(const TParticlePDG* pdt);
  virtual ~TCandPtr() {}
  void SetOneToMany(Bool_t oTM=kTRUE){_oneToMany=oTM;}
  Bool_t SetMother(Int_t im);
  Bool_t GetMother(Int_t &im);  
  UInt_t NDaughters() const;
  Int_t operator()(Int_t i);

  Int_t LundId() const {return _lundId;}
  
  void AddDaughter(Int_t ind);
  Bool_t operator== (const TCandPtr& p){
    return &p==this;
  }

  ClassDef(TCandPtr,1) //Pointer to TCandidate
};

#endif
