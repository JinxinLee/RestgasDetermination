////////////////////////////////////////////////////////////
//
// PndSttHitProducerIdeal
// 
// Class to sum up 2 TCA of STTHits
// Usually used to put together the stt parallel and skewed
// hits in one full TCA
//
// authors: Lia Lavezzi - University of Torino (2015)
//                                   
////////////////////////////////////////////////////////////

#include "PndSttFullTCAProducer.h"

#include "PndSttHit.h"

#include "FairRootManager.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"

#include "TClonesArray.h"

#include <iostream>
#include <cmath>

using std::cout;
using std::endl;
using std::sqrt;

// -----   Default constructor   -------------------------------------------
PndSttFullTCAProducer::PndSttFullTCAProducer() :
  FairTask("Full STT Hit TCA Producer",0) 
{ 
 
  fSttHitArray1 = NULL;
  fSttHitArray2 = NULL;
  fSttHitArray0 = NULL;
  fBranchName1 = "STTParalHit";
  fBranchName2 = "STTSkewHit";
}
// -------------------------------------------------------------------------



// -----   Destructor   ----------------------------------------------------
PndSttFullTCAProducer::~PndSttFullTCAProducer() 
{ 
}
// -------------------------------------------------------------------------



// -----   Public method Init   --------------------------------------------
InitStatus PndSttFullTCAProducer::Init() 
{
  // Get RootManager
  FairRootManager* ioman = FairRootManager::Instance();

  if ( ! ioman ) 
    {
      cout << "-E- PndSttFullTCAProducer::Init: "
	   << "RootManager not instantiated!" << endl;
      return kFATAL;
    }

  // Get input array
  fSttHitArray1 = (TClonesArray*) ioman->GetObject(fBranchName1);
  if ( ! fSttHitArray1)
    {
      cout << "-W- PndSttFullTCAProducer::Init: "
	   << "No " << fBranchName1 << " array!" << endl;
      return kERROR;
  }

  fSttHitArray2 = (TClonesArray*) ioman->GetObject(fBranchName2);
  if ( ! fSttHitArray2)
    {
      cout << "-W- PndSttFullTCAProducer::Init: "
	   << "No " << fBranchName2 << " array!" << endl;
      return kERROR;
    }

  // Create and register output array
  fSttHitArray0 = new TClonesArray("PndSttHit");
  ioman->Register("STTHit", "STT", fSttHitArray0, kTRUE);

  cout << "-I- PndSttFullTCAProducer: Intialisation successfull" << endl;
  return kSUCCESS;
}
// -------------------------------------------------------------------------

// -----   Public method Exec   --------------------------------------------
void PndSttFullTCAProducer::Exec(Option_t*) 
{
  fSttHitArray0->Delete();

  PndSttHit* hit = NULL;
  for(int ihit = 0; ihit < fSttHitArray1->GetEntriesFast(); ihit++) {
    hit = (PndSttHit*) fSttHitArray1->At(ihit);
    
    int size = fSttHitArray0->GetEntriesFast();
    new ((*fSttHitArray0)[size]) PndSttHit(*hit);
  }

  for(int ihit = 0; ihit < fSttHitArray2->GetEntriesFast(); ihit++) {
    hit = (PndSttHit*) fSttHitArray2->At(ihit);

    int size = fSttHitArray0->GetEntriesFast();
    new ((*fSttHitArray0)[size]) PndSttHit(*hit);
  }
}
 


ClassImp(PndSttFullTCAProducer)
