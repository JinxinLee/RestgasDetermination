// -------------------------------------------------------------------------
// -----                PndRpcSimpleReco source file             -----
//                      created 10/06/08 by A. Galoyan
// -------------------------------------------------------------------------


#include "TClonesArray.h"

#include "FairRootManager.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"
#include "FairRun.h"
#include "PndMCTrack.h"

#include "PndRpcSimpleReco.h"
#include "PndRpcHit.h"
#include "PndRpcPoint.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TFile.h"
#include "TVector3.h"
#include "TRandom.h"




// -----   Default constructor   -------------------------------------------
PndRpcSimpleReco::PndRpcSimpleReco() :
  FairTask("Reco of Rpc TOF Hits") { }
// -------------------------------------------------------------------------



// -----   Destructor   ----------------------------------------------------
PndRpcSimpleReco::~PndRpcSimpleReco() { }
// -------------------------------------------------------------------------



// -----   Public method Init   --------------------------------------------
InitStatus PndRpcSimpleReco::Init() {

  // Get RootManager
 std::cout << " INITIALIZATION OF Simple Reco Rpc-tof Hit **"<< std::endl;
  FairRootManager* ioman = FairRootManager::Instance();
  if ( ! ioman ) {
    cout << "-E- PndRpcSimpleReco::Init: "
	 << "RootManager not instantised!" << endl;
    return kFATAL;
  }

  // Get input array
  fPointArray = (TClonesArray*) ioman->GetObject("PndRpcPoint");
  if ( ! fPointArray ) {
    cout << "-W- PndRpcSimpleReco::Init: "
	 << "No PndRpcPoint array!" << endl;
    return kERROR;
  }

  // Get input array
  fHitArray = (TClonesArray*) ioman->GetObject("PndRpcHit");
  if ( ! fHitArray ) {
    cout << "-W- PndRpcSimpleReco::Init: "
	 << "No PndRpcHit array!" << endl;
    return kERROR;
  }

  // Get input array
  MCTrackArray = (TClonesArray*) ioman->GetObject("MCTrack");
  if ( ! MCTrackArray ) {
    cout << "-W- PndRpcSimpleReco::Init: "
	 << "No MCTrack array!" << endl;
    return kERROR;
  }

// Create and register output array
//  fRecoHitArray = new TClonesArray("PndRpcRecoHit");
//  ioman->Register("PndRpcRecoHit", "PNDRPC", fRecoHitArray, kTRUE);

 //output file for histos
 fOutFile = "rpcreco.root";

  RecoMass2 = new TH1F("Mass2","Mass2",200, -0.5, 1.2);
  Velocity_P = new TH2F("p_beta","p_beta",200,0.1,2.1, 250,0.0, 250.0 );
  Mass2_P = new TH2F("Mass_mom","Mass_mom", 200,-0.5,1.2, 200, 0.1, 2.1);  
   

  cout << "-I- PndRpcSimpleReco: Intialisation successfull" << endl;
  return kSUCCESS;

}
// -------------------------------------------------------------------------


// -----   Public method Exec   --------------------------------------------
void PndRpcSimpleReco::Exec(Option_t* opt) {

// Reset output array for future RecoHitArray
//  if ( ! fRecoHitArray ) Fatal("Exec", "No RpcRecoHitArray");

//  fRecoHitArray->Clear();

  // Declare some variables
  
  PndRpcPoint *point = NULL;
  PndRpcHit *hit = NULL;
  PndMCTrack *McTrack = NULL;
  
  TVector3 pos, dpos;       // Position and error vectors
  Int_t Gap=0;
  Int_t Cell=0;
  Int_t Module=0; 

// Get number of MC tracks in the event
  Int_t nMCTracks = MCTrackArray->GetEntriesFast(); 
    
  // Loop over PndRpcHits
  Int_t nPoints = fPointArray->GetEntriesFast();
  Int_t nHits = fHitArray->GetEntriesFast();
 
  for (Int_t iHit=0; iHit<nHits; iHit++) {
   hit  = (PndRpcHit*) fHitArray->At(iHit);
    if ( ! hit) continue;
   point = (PndRpcPoint*) fPointArray->At(hit->GetIndex());
   if (! point) continue;
  McTrack = (PndMCTrack *) MCTrackArray->At(point->GetTrackID());

// choose for example -  proton 2212, pi+ 211, K+ 321 .
// if(McTrack-> GetPdgCode()== 2212)
  if (point->GetLength() != 0) 
{
 Double_t 
pmod=sqrt(point->GetPx()*point->GetPx()+point->GetPy()*point->GetPy()+point->GetPz()*point->GetPz());
// smear  momentum with dp
 Double_t dp=0.001*pmod;
 Double_t pmod_sm=gRandom->Gaus(pmod,dp);

 Double_t tof = hit->GetTime();
// velocity
// 30 is the light velocity in cm/ns
 Double_t beta  = (point->GetLength())/(tof - McTrack->GetStartTime())/30.;  
// squared mass 
 Double_t mass2  = pmod_sm*pmod_sm*(1/beta/beta-1);
 
// Fill  histos
     RecoMass2->Fill(mass2);

// 1000/30 for transfer to ps/cm 
   Velocity_P->Fill(pmod_sm,1000./30./beta);

     Mass2_P->Fill(mass2,pmod_sm);   

    // Detector ID
    Gap = hit->GetGap();
    Cell = hit->GetCell();
    Module = hit->GetModule();

   //  hit position
    pos  = hit->Getpos();
   // hit position error
    dpos  = hit->Getdpos();
  
} //end  if

  }   // Loop over MCHits

  // Event summary
  cout << "-I- PndRpcSimpleReco: " << nHits << " PndRpcHits" 
 <<" mass2  and velocity   created." << endl;

}

void PndRpcSimpleReco::Finish(){
  TFile* f = new TFile(fOutFile,"RECREATE");
  RecoMass2->Write();
  Velocity_P->Write();
  Mass2_P->Write();
  f->Close();
//  delete f;
}

ClassImp(PndRpcSimpleReco)
