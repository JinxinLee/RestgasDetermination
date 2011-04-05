// -------------------------------------------------------------------------
// -----                PndMdtHitProducer source file                  -----
// -----                  Created 04/04/11  by  S.Spataro              -----
// -------------------------------------------------------------------------

#include "PndMdtHitProducer.h"

#include "PndMdtHit.h"
#include "PndMdtDigi.h"

#include "FairRootManager.h"
#include "FairDetector.h"
#include "FairRun.h"
#include "FairRuntimeDb.h"

#include "TClonesArray.h"
#include "TVector3.h"

#include <iostream>

using std::cout;
using std::endl;

// -----   Default constructor   -------------------------------------------
PndMdtHitProducer::PndMdtHitProducer() :
  FairTask("MDT Hit Producer") 
{
  // Reset();
}
// -------------------------------------------------------------------------

// -----   Destructor   ----------------------------------------------------
PndMdtHitProducer::~PndMdtHitProducer() { }
// -------------------------------------------------------------------------



// -----   Public method Init   --------------------------------------------
InitStatus PndMdtHitProducer::Init() {
  
  cout << "-I- PndMdtHitProducer::Init: "
       << "INITIALIZATION *********************" << endl;
  
  FairRun* sim = FairRun::Instance();
  FairRuntimeDb* rtdb=sim->GetRuntimeDb();
    
  // Get RootManager
  FairRootManager* ioman = FairRootManager::Instance();
  if ( ! ioman ) {
    cout << "-E- PndMdtHitProducer::Init: "
	 << "RootManager not instantiated!" << endl;
    return kFATAL;
  }
  
  // Get input array
  fBoxArray = (TClonesArray*) ioman->GetObject("MdtDigiBox");
  if ( ! fBoxArray ) {
    cout << "-W- PndMdtHitProducer::Init: "
	 << "No MdtDigiBox array!" << endl;
    return kERROR;
  }
 
  fStripArray = (TClonesArray*) ioman->GetObject("MdtDigiStrip");
  if ( ! fStripArray ) {
    cout << "-W- PndMdtHitProducer::Init: "
	 << "No MdtDigiStrip array!" << endl;
    return kERROR;
  }
  

  // Create and register output array
  fHitArray = new TClonesArray("PndMdtHit");
  
  ioman->Register("MdtHit","Mdt",fHitArray,kTRUE);
 
   cout << "-I- PndMdtHitProducer: Intialization successfull" << endl;
  
  return kSUCCESS;

}
// -------------------------------------------------------------------------



// -----   Public method Exec   --------------------------------------------
void PndMdtHitProducer::Exec(Option_t* opt) {
  
  // Reset output array
  fHitArray->Delete();

  Int_t nBox = fBoxArray->GetEntriesFast();
  Int_t nStrip = fStripArray->GetEntriesFast();
  if (nBox==0 || nStrip==0) return;

  for (Int_t iBox=0; iBox<nBox; iBox++) 
    {
      PndMdtDigi* boxDigi  = (PndMdtDigi*) fBoxArray->At(iBox);
      for (Int_t iStrip=0; iStrip<nStrip; iStrip++) 
	{
	  PndMdtDigi* stripDigi  = (PndMdtDigi*) fStripArray->At(iStrip);
	  if ( (boxDigi->GetModule()!=stripDigi->GetModule()) ||
	       (boxDigi->GetSector()!=stripDigi->GetSector()) ||
	       (boxDigi->GetLayerID()!=stripDigi->GetLayerID()) )
	    continue;
	  vector<Int_t>vecBox = boxDigi->GetPointList();
	  vector<Int_t>vecStrip = stripDigi->GetPointList();
	  for (Int_t vBox=0; vBox<(vecBox.size()); vBox++) 
	    {
	      for (Int_t vStrip=0; vStrip<(vecStrip.size()); vStrip++) 
		{
		  if (vecBox[vBox]==vecStrip[vStrip])
		    {
		      TVector3 pos = stripDigi->GetLabPosition();
		      TVector3 dpos(0.5,0.5,0.5);
		      
		      AddHit(boxDigi->GetDetectorID(), (Int_t)stripDigi->GetStrip(), pos, dpos, vecBox[vBox],  vecStrip[vStrip] );
		    }
		}
	    }
	}
    }
  
  
}
// -------------------------------------------------------------------------

// -----   Private method AddHit   --------------------------------------------
PndMdtHit* PndMdtHitProducer::AddHit(Int_t detID, Int_t stripID, TVector3& pos, TVector3& dpos, Int_t bIndex, Int_t sIndex){
  // It fills the PndMdtHit category
 
  TClonesArray& clref = *fHitArray;
  Int_t size = clref.GetEntriesFast();
  return new(clref[size]) PndMdtHit(detID, stripID, pos, dpos, bIndex, sIndex);
}
// ----


ClassImp(PndMdtHitProducer)
