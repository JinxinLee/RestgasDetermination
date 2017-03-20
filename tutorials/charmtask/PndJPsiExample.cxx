/******************************************************

JPsi->e+e-
Reads the TPC tracks and reconstruct the InvariantMass 
of J/Psi: Dipak
*******************************************************/ 

#include "TClonesArray.h"

#include "FairRootManager.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"
#include "PndLheCandidate.h"

#include "TVector3.h"
#include "FairRun.h"
#include "FairRuntimeDb.h"
#include "PndJPsiExample.h"
#include <string>
		
// -----   Default constructor   -------------------------------------------
PndJPsiExample::PndJPsiExample() :
  FairTask("FastSim Dump") { 
}
// -------------------------------------------------------------------------

// -----   Destructor   ----------------------------------------------------
PndJPsiExample::~PndJPsiExample() { }
// -------------------------------------------------------------------------



// -----   Public method Init   --------------------------------------------
InitStatus PndJPsiExample::Init() {
 
  cout << " Inside the Init function****" << endl;
  
  //FairDetector::Initialize();
  //FairRun* sim = FairRun::Instance();
  //FairRuntimeDb* rtdb=sim->GetRuntimeDb();
  
  // Get RootManager
  FairRootManager* ioman = FairRootManager::Instance();
  if ( ! ioman ) {
    cout << "-E- PndEmcHitProducer::Init: "
	 << "RootManager not instantiated!" << endl;
    return kFATAL;
  }
  // Get input array
  fTrArray = (TClonesArray*) ioman->GetObject("LheCandidate");
  if ( ! fTrArray) {
    cout << "-W- PndJPsiExample::Init: "
	 << "No TpcTrack array!" << endl;
    return kERROR;
  }

  // Create and register output array
  cout << "-I- PndJPsiExample: Intialization successfull" << endl;

  fInvMass = new TH1D("invmass","",100,0.0,4.0);
  
  return kSUCCESS;

}

void PndJPsiExample::SetParContainers() {

  // Get run and runtime database
  FairRunAna* run = FairRunAna::Instance();
  if ( ! run ) Fatal("SetParContainers", "No analysis run");

  FairRuntimeDb* db = run->GetRuntimeDb();
  if ( ! db ) Fatal("SetParContainers", "No runtime database");

 
}

// -------------------------------------------------------------------------

// -----   Public method Exec   --------------------------------------------
void PndJPsiExample::Exec(Option_t*) {
  
  // Reset output array
  Int_t nTracks = fTrArray->GetEntriesFast();

  //  cout <<"number of tracks **** "<< nTracks <<endl;
  
  for (Int_t i1=0; i1<nTracks; i1++){
    
    // Get the Tpc Track information
    PndLheCandidate *tr1 = (PndLheCandidate*)fTrArray->At(i1);    
    track1.SetXYZM(tr1->GetPx(), tr1->GetPy(), tr1->GetPz(), 0.000511);

    for (Int_t i2=0; i2<nTracks; i2++){
      PndLheCandidate *tr2 = (PndLheCandidate*)fTrArray->At(i2);    

      track2.SetXYZM(tr2->GetPx(), tr2->GetPy(), tr2->GetPz(), 0.000511);
      
      TLorentzVector sumTrack = track1 + track2;

     fInvMass->Fill(sumTrack.M());

    }
    
  }

}
// -------------------------------------------------------------------------

void PndJPsiExample::Finish()
{
  fInvMass->Write();
}

ClassImp(PndJPsiExample)
