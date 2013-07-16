// -----------------------------------------
// PndDrcLutReco.cpp
//
// Created on: 13.07.2013
// Author: R.Dzhygadlo at gsi.de
// -----------------------------------------

#include "PndDrcLutReco.h"

#include "FairRootManager.h"
#include "PndMCTrack.h"
#include "PndDrcBarPoint.h"
#include "PndDrcPDPoint.h"
#include "PndDrcHit.h"
#include "PndDrcPDHit.h"
#include "PndDrcLutNode.h"
#include "PndDrcLutInfo.h"

using std::cout;
using std::endl;

// -----   Default constructor   -------------------------------------------
PndDrcLutReco::PndDrcLutReco() 
:FairTask("PndDrcLutReco")
{
  fInputFile = "luttab.root";
}

// -----   Standard constructors   -----------------------------------------
PndDrcLutReco::PndDrcLutReco(Int_t verbose) 
  :FairTask("PndDrcLutReco",verbose)
{
  fVerbose = verbose;  
  fInputFile = "luttab.root";
}

PndDrcLutReco::PndDrcLutReco(Int_t verbose, TString infilename) 
  :FairTask("PndDrcLutReco",verbose)
{
  fVerbose = verbose;  
  fInputFile = infilename;
}

// -----   Destructor   ----------------------------------------------------
PndDrcLutReco::~PndDrcLutReco()
{

}

// -----   Initialization   ------------------------------------------------
InitStatus PndDrcLutReco::Init()
{
  cout << " ---------- INITIALIZATION ------------" << endl;
  nevents = 0;
  // Get RootManager
  FairRootManager* ioman = FairRootManager::Instance();
  if ( ! ioman ) {
    cout << "-E- PndDrcLutReco::Init: " << "RootManager not instantiated!" << endl;
    return kFATAL;
  }

  // Get input array
  fMCArray = (TClonesArray*) ioman->GetObject("MCTrack");
  if ( ! fMCArray ) {
    cout << "-W- PndDrcLutReco::Init: " << "No MCTrack array!" << endl;
    return kERROR;
  } 
  // Get Photon point array
  fPDPointArray = (TClonesArray*) ioman->GetObject("DrcPDPoint");
  if ( ! fPDPointArray ) {
    cout << "-W- PndDrcLutReco::Init: " << "No DrcPDPoint array!" << endl;
    return kERROR;
  }  
  // Get digi array
  fDigiArray = (TClonesArray*) ioman->GetObject("DrcDigi");
  if ( ! fDigiArray ) {
    cout << "-W- PndDrcLutReco::Init: " << "No DrcDigi array!" << endl;
    return kERROR;
  }
  // Get input array
  fPDHitArray = (TClonesArray*) ioman->GetObject("DrcPDHit");
  if ( ! fPDHitArray ) {
    cout << "-W- PndDrcLutReco::Init: " << "No DrcPDHit array!" << endl;
    return kERROR;
  }
 
  fFile = new TFile(fInputFile);
  fLut = new TClonesArray("PndDrcLutNode");
  fTree=(TTree *) fFile->Get("dircsim") ;
  fTree->SetBranchAddress("LUT",&fLut);
  fTree->GetEntry(0);


  // Create and register output array
  fDrcLutInfoArray = new TClonesArray("PndDrcLutInfo");
  ioman->Register("DrcLutInfo","Drc",fDrcLutInfoArray, kTRUE);


  cout << "-I- PndDrcLutReco: Intialization successfull" << endl;
  return kSUCCESS;
   
}

// -----   Execution of Task   ---------------------------------------------
void PndDrcLutReco::Exec(Option_t* option)
{
  nevents++;
  fDetectorID = 0;
  ProcessPhotonHit();
}

//--------------Process Photon Hits-----------------------------------------
void PndDrcLutReco::ProcessPhotonHit()
{
  PndDrcLutInfo lutinfo;
  TVector3 dir, trackdir;
  Double_t cangle,tangle;
  for(Int_t k=0; k<fMCArray->GetEntriesFast(); k++){
    fMCTrack = (PndMCTrack*)fMCArray->At(k);
    if(fMCTrack->GetMotherID()==-1) {
      //std::cout<<"Z "<< fMCTrack->GetStartVertex().Z()<<std::endl;
      trackdir = fMCTrack->GetMomentum();
      Int_t pdgcode = fMCTrack->GetPdgCode();
      Double_t Mrmass;
      if(fabs(pdgcode) == 211){Mrmass = 0.139570;}
      if(fabs(pdgcode) == 321){Mrmass = 0.49368;}

      Double_t Mrmom = trackdir.Mag();      
      cangle = acos(sqrt(pow(Mrmom,2) + pow(Mrmass,2))/Mrmom/1.47);
      std::cout<<"cangle  "<< cangle<<std::endl;
      break;
    }
  }

 // for(Int_t k=0; k<fMCArray->GetEntriesFast(); k++) {
 //    fMCTrack = (PndMCTrack*)fMCArray->At(k);
 //    if(fMCTrack->GetMotherID()==0) {
 //      //std::cout<<"Z "<< fMCTrack->GetStartVertex().Z()<<std::endl;
 //      dir = fMCTrack->GetMomentum(); 
 //      lutinfo.AddEntry(cangle - trackdir.Angle(dir));
    
 //    }
 //  }

  // Loop over PndDrcPDHits
  for(Int_t k=0; k<fPDHitArray->GetEntriesFast(); k++) {
   
    fPDHit = (PndDrcPDHit*)fPDHitArray->At(k);

    PndDrcLutNode *node= (PndDrcLutNode*) fLut->At(fPDHit->GetDetectorID());
    Int_t size = node->Entries();
    for(int i=0; i<size; i++){
      dir = node->GetEntry(i);
      tangle=trackdir.Angle(dir);
      if(tangle>TMath::Pi()/2.) tangle = TMath::Pi()-tangle;
      lutinfo.AddAngle(cangle - tangle);
    }

    // Int_t digiID= fPDHit->GetRefIndex();
    // fDigi = (PndDrcDigi*) fDigiArray->At(digiID);

    // Int_t pointID= fDigi->GetIndex(0);
    // fPDPoint = (PndDrcPDPoint*)fPDPointArray->At(pointID);
    
    // Int_t trackID= fPDPoint->GetTrackID();

    // fMCTrack = (PndMCTrack*)fMCArray->At(trackID);
    // TVector3 dir =  fMCTrack->GetMomentum().Unit();
    // ((PndDrcLutNode*)(fLut->At(fDigi->GetSensorID())))->AddEntry(dir);
  }

  new ((*fDrcLutInfoArray)[fDrcLutInfoArray->GetEntriesFast()]) PndDrcLutInfo(lutinfo);
}

// -----   Finish Task   ---------------------------------------------------
void PndDrcLutReco::Finish()
{
    fLut->Clear();
    cout << "-I- PndDrcLutReco: Finish" << endl; 
}

ClassImp(PndDrcLutReco)
