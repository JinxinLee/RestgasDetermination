// -------------------------------------------------------------------------
// -----                PndGemTrackFinderIdeal source file             -----
// -----                Created 19.03.2009 by R. Karabowicz            -----
// -----              according to the PndDchTrackFinderIdeal          -----
// -------------------------------------------------------------------------

#include "PndGemTrackFinderIdeal.h"

// Pnd includes
#include "FairRootManager.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"
#include "FairBaseParSet.h"
#include "FairTrackParam.h"
#include "PndGemMCPoint.h"
#include "FairRootManager.h"
#include "PndDetectorList.h"
// ROOT includes
#include "TClonesArray.h"
#include "TGeoManager.h"
#include "TMatrixFSym.h"

// C++ includes
#include <iostream>
#include <map>
#include <cmath>
using std::cout;
using std::endl;
using std::map;

// -----   Default constructor   -------------------------------------------
PndGemTrackFinderIdeal::PndGemTrackFinderIdeal() {
  fMCTrackArray = NULL;
  fMCPointArray = NULL;
  fNofEvents    = 0;
}

// -----   Destructor   ----------------------------------------------------
PndGemTrackFinderIdeal::~PndGemTrackFinderIdeal() { }

// -----   Init  -----------------------------------------------------------
void PndGemTrackFinderIdeal::Init() {
  
  // Get and check FairRootManager
  FairRootManager* ioman = FairRootManager::Instance();
  if( !ioman ) {
    cout << "-E- "<< GetName() <<"::Init: "
	 << "RootManager not instantised!" << endl;
    return;
  }
  
  // Get the pointer to the singleton FairRunAna object
  FairRunAna* ana = FairRunAna::Instance();
  if(NULL == ana) {
    cout << "-E- "<< GetName() <<"::Init :"
	 <<" no FairRunAna object!" << endl;
    return;
  }
  // Get the pointer to run-time data base
  FairRuntimeDb* rtdb = ana->GetRuntimeDb();
  if(NULL == rtdb) {
    cout << "-E- "<< GetName() <<"::Init :"
	 <<" no runtime database!" << endl;
    return;
  }
  
  // Get MCTrack array
  fMCTrackArray  = (TClonesArray*) ioman->ActivateBranch("MCTrack");
  if( !fMCTrackArray ) {
    cout << "-E- "<< GetName() <<"::Init: No MCTrack array!"
	 << endl;
    return;
  }
  
  // Get PndGemPoint (MCPoint) array
  fMCPointArray  = (TClonesArray*) ioman->GetObject("GEMPoint");
  if( !fMCPointArray ) {
    cout << "-E- "<< GetName() <<"::Init: No MCPoint array!"
	 << endl;
    return;
  }
  if ( fVerbose )
    cout <<"fMCPointArray #: "<< fMCPointArray->GetEntriesFast() << endl;
  
  // Geometry loading
  //TFile *infile = ioman->GetInFile();
  //TGeoManager *geoMan = (TGeoManager*) infile->Get("FAIRGeom");
  //fGemStructure = PndGemStructure::Instance(geoMan);
  
  std::cout << "-I- "<< GetName() <<": Intialization successfull" << std::endl;
}

// -----   Public method DoFind   ------------------------------------------
Int_t PndGemTrackFinderIdeal::DoFind(TClonesArray* hitArray,
				     TClonesArray* trackArray) {
  
  // Count events
  fNofEvents++;
  if ( fVerbose ) {
    cout << endl << endl<< endl << endl;
    cout << "=======================================================" << endl;
    cout << "-I-        Event No: " <<  fNofEvents << endl;
    cout << "=======================================================" << endl;
    
    
    cout <<"-I- "<< GetName() <<"::DoFind "<< endl;
    cout << "-------------------------------------------------------" << endl;
    cout << "     ### Start DoFind" << endl;
    cout << "-------------------------------------------------------" << endl;
  }

  // Check pointers
  if( !fMCTrackArray ) {
    cout << "-E- PndGemTrackFinderIdeal::DoFind: "
	 << "MCTrack array missing! " << endl;
    return -1;
  }
  
  if( !fMCPointArray ) {
    cout << "-E- "<< GetName() <<"::DoFind: "
	 << "MCPoint array missing! " << endl;
    return -1;
  }
  
  if( !hitArray ) {
    cout << "-E- "<< GetName() <<"::DoFind: "
	 << "Hit arrays missing! "<< endl;
    return -1;
  }
  
  // Initialise control counters
  Int_t nNoMCTrack   = 0;
  Int_t nNoTrack     = 0;
  Int_t nNoGemPoint  = 0;
  Int_t nNoGemHit    = 0;
  
  // Create pointers to GemHit and GemPoint
  PndGemHit*   gemHit   = NULL;
  FairMCPoint*  mcPoint  = NULL;
  PndMCTrack*  mcTrack  = NULL;
  PndTrackCand* gemTrackCand = NULL;

  // Declare variables outside the loop
  Int_t ptIndex = 0;       // MC point index
  Int_t mcTrackIndex = 0;  // MC track index
  Int_t trackIndex = 0;    // Gem track index
  Int_t relDetID = -1;//3000;   // 

  // Create STL map from MCTrack index to number of hits
  std::map<Int_t, Int_t> hitMap;
  std::map<Int_t, Double_t> trackPMap;
  std::map<Int_t, Int_t>::iterator itHitMap;
  
  if ( fVerbose ) {
    // Size of fMCTrackArray
    cout <<"# MC Tracks: "<< fMCTrackArray->GetEntriesFast() << endl;
    // Size of fMCTrackArray
    cout <<"# MC Points: "<< fMCPointArray->GetEntriesFast() << endl;
  }

  // Number of Gem hits
  Int_t nGemHits = hitArray->GetEntriesFast();
  if(fVerbose > 2) cout <<"# GemHits: "<< nGemHits << endl;
  
  for(Int_t iHit = 0; iHit < nGemHits; iHit++){
    // Get the pointer to Gem hit
    gemHit = (PndGemHit*) hitArray->At(iHit);
    if(gemHit->GetDetectorID() > relDetID) {
      if(NULL == gemHit ) continue;
      
      // Get point index
      ptIndex = gemHit->GetRefIndex();

      if ( ptIndex == -1 ) continue;

      // Get pointer to MC point
      mcPoint = (FairMCPoint*) fMCPointArray->At(ptIndex);
      if(NULL == mcPoint) continue;
      
      // Get MC track index
      mcTrackIndex = mcPoint->GetTrackID();

      trackPMap[mcTrackIndex] = TMath::Sqrt(mcPoint->GetPx()*mcPoint->GetPx()+
					    mcPoint->GetPy()*mcPoint->GetPy()+
					    mcPoint->GetPz()*mcPoint->GetPz());
      
      // Increment the number of hits
      hitMap[mcTrackIndex] += 1;
    }
  }
  

  // Create STL map from MCTrack index to GemTrack index
  map<Int_t, Int_t> trackMap;
  
  // Loop over reconstructable MCTracks and create corresponding GemTrack
  Int_t nMCacc = 0;  // Accepted MC tracks
  Int_t nTracks = 0; // Reconstructable MC tracks
  Int_t nMCTracks = fMCTrackArray->GetEntriesFast();
  
  if(fVerbose > 2) {
    cout <<"# of MC Tracks (nMCTracks): " << nMCTracks << endl;
  }
  
  for(Int_t iMCTrack = 0; iMCTrack < nMCTracks; iMCTrack++) {
    mcTrack = (PndMCTrack*) fMCTrackArray->At(iMCTrack);
    if( !mcTrack ) continue;
    
    
    // Either Primary or All MC tracks are considered
    if(fPrimary) {
      if(mcTrack->GetMotherID() != -1 && fabs((mcTrack->GetStartVertex()).Z())>1.) continue;
      
      if(fVerbose > 2) {
	cout <<"MC track #: "<< iMCTrack
	     << "\t MotherID: "<< mcTrack->GetMotherID()
	     << "\t StartVertex Z: "<< mcTrack->GetStartVertex().Z() << endl;
      }
    }
    //----------------------------------------------------
    nMCacc++;
    
    if(fVerbose > 2) {
      cout << iMCTrack << ": #GemPoints in MCTrack: "<< mcTrack->GetNPoints(kGEM) << " -> " << hitMap[iMCTrack] << endl;
    }
    if ( !hitMap[iMCTrack] ) continue;

    new((*trackArray)[nTracks]) PndTrackCand();
    
    gemTrackCand = (PndTrackCand*) trackArray->At(nTracks);
    TLorentzVector tlVec = mcTrack->Get4Momentum();
    //    Double_t mom = tlVec.Mag();
    Double_t mom = trackPMap[iMCTrack];
    if ( fVerbose > 2 ) {
      cout << "momentum is " << mom << " (" 
	   << tlVec.X() << " , " 
	   << tlVec.Y() << " , " 
	   << tlVec.Z() << " , " 
	   << tlVec.T() << ") " << endl;
    }

//     if(mom != 0) {
//       gemTrack->GetParamLast()->SetQp(1./mom);
//     } else {
//       gemTrack->GetParamLast()->SetQp(0);
//     }

//     gemTrackCand->GetParamLast()->SetQp(1e6+mcTrack->GetPdgCode());
    gemTrackCand->setMcTrackId(iMCTrack);

    trackMap[iMCTrack] = nTracks;
    
    nTracks++;
  }
  
  // Loop over hits. Get corresponding MCPoint and MCTrack index
  for(Int_t iHit = 0; iHit < nGemHits; iHit++) {
    gemHit = (PndGemHit*) hitArray->At(iHit);
    if( !gemHit ) {
      nNoGemHit++;
      continue;
    }
    
    ptIndex = gemHit->GetRefIndex();
    if(ptIndex < 0) continue;           // fake or background hit
    mcPoint = (FairMCPoint*) fMCPointArray->At(ptIndex);
    
    if( !mcPoint ) {
      nNoGemPoint++;
      continue;
    }
    
    mcTrackIndex = mcPoint->GetTrackID();
    
    if(mcTrackIndex < 0 || mcTrackIndex > nMCTracks) {
      cout << "-E- "<< GetName() <<"::DoFind: "
	   << "MCTrack index out of range. " << mcTrackIndex << " "
	   << nMCTracks << endl;
      nNoMCTrack++;
      continue;
    }
    
    if(trackMap.find(mcTrackIndex) == trackMap.end()) continue;
    trackIndex = trackMap[mcTrackIndex];
    gemTrackCand = (PndTrackCand*) trackArray->At(trackIndex);
    
    if( !gemTrackCand ) {
      cout << "-E- "<< GetName() <<"::DoFind: "
	   << "No GemTrack pointer. " << iHit << " " << ptIndex
	   << " " << mcTrackIndex << " " << trackIndex << endl;
      nNoTrack++;
      continue;
    }
    
    gemTrackCand->AddHit(kGemHit,iHit,gemHit->GetPosition().Mag());
    //    cout << "gemTrack " << trackIndex << " has " << gemTrack->GetNofGemHits() << endl;
    
    if(fVerbose > 3) {
      cout << "GEM hit " << iHit << " from GEM point "
	   << ptIndex << " (" << mcTrackIndex << ") "
	   << "added to GEM track " << trackIndex << endl;
    }
  }
  
  // Sorting hits
  for(Int_t iTrack = 0; iTrack < nTracks; iTrack++) {
    gemTrackCand = (PndTrackCand*) trackArray->At(iTrack);
    gemTrackCand->Sort();
    //    cout << "gemTrack " << iTrack << " has " << gemTrack->GetNofGemHits() << endl;
    PndTrackCandHit tch = gemTrackCand->GetSortedHit(0);

    gemHit = (PndGemHit*) hitArray->At(tch.GetHitId());
    
    TVector3 pos;
    TVector3 mom;
    
    ptIndex = gemHit->GetRefIndex();
    mcPoint = (FairMCPoint*) fMCPointArray->At(ptIndex);

    gemHit->Position(pos);
    mcPoint->Momentum(mom);

    gemTrackCand->setTrackSeed(pos,mom.Unit(),1./mom.Mag());
  }
  
  if(fVerbose) {
    cout << endl;
    cout << "-------------------------------------------------------" << endl;
    cout << "-I-      "<< GetName() <<": Event summary      -I-" << endl;
    cout << "-------------------------------------------------------" << endl;
    cout << "Total Gem hits:  " << nGemHits << endl;
    cout << "MC tracks total: " << nMCTracks << ", accepted: "
	 << nMCacc << ", reconstructable: " << nTracks << endl;
    if(nNoGemHit)   cout << "GemHits not found   : " << nNoGemHit   << endl;
    if(nNoGemPoint) cout << "GemPoints not found : " << nNoGemPoint << endl;
    if(nNoMCTrack)  cout << "MCTracks not found  : " << nNoMCTrack  << endl;
    if(nNoTrack)    cout << "GemTracks not found : " << nNoTrack    << endl;
    cout << "------------------------------------------------------" << endl;
    cout << endl;
  } else {
//     cout << "All: "        << nMCTracks
// 	 << ", Accepted: "      << nMCacc
// 	 << ", Reconstructed: " << nTracks << endl;
  }
  
  return nTracks;
}


ClassImp(PndGemTrackFinderIdeal)
