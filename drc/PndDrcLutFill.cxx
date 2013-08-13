// -----------------------------------------
// PndDrcLutFill.cpp
//
// Created on: 08.07.2013
// Author: R.Dzhygadlo at gsi.de
// -----------------------------------------

#include "PndDrcLutFill.h"

#include "FairRootManager.h"
#include "PndMCTrack.h"
#include "PndDrcBarPoint.h"
#include "PndDrcPDPoint.h"
#include "PndDrcHit.h"
#include "PndDrcPDHit.h"
#include "PndDrcLutNode.h"

using std::cout;
using std::endl;

// -----   Default constructor   -------------------------------------------
PndDrcLutFill::PndDrcLutFill() 
:FairTask("PndDrcLutFill")
{
  fOutputFile = "luttab.root";
}

// -----   Standard constructors   -----------------------------------------
PndDrcLutFill::PndDrcLutFill(Int_t verbose) 
  :FairTask("PndDrcLutFill",verbose)
{
  fVerbose = verbose;  
  fOutputFile = "luttab.root";
}

PndDrcLutFill::PndDrcLutFill(Int_t verbose, TString outfilename) 
  :FairTask("PndDrcLutFill",verbose)
{
  fVerbose = verbose;  
  fOutputFile = outfilename;
}

// -----   Destructor   ----------------------------------------------------
PndDrcLutFill::~PndDrcLutFill()
{

}

// -----   Initialization   ------------------------------------------------
InitStatus PndDrcLutFill::Init()
{
  cout << " ---------- INITIALIZATION ------------" << endl;
  nevents = 0;
  // Get RootManager
  FairRootManager* ioman = FairRootManager::Instance();
  if ( ! ioman ) {
    cout << "-E- PndDrcLutFill::Init: " << "RootManager not instantiated!" << endl;
    return kFATAL;
  }

  // Get input array
  fMCArray = (TClonesArray*) ioman->GetObject("MCTrack");
  if ( ! fMCArray ) {
    cout << "-W- PndDrcLutFill::Init: " << "No MCTrack array!" << endl;
    return kERROR;
  } 
  // Get Photon point array
  fPDPointArray = (TClonesArray*) ioman->GetObject("DrcPDPoint");
  if ( ! fPDPointArray ) {
    cout << "-W- PndDrcLutFill::Init: " << "No DrcPDPoint array!" << endl;
    return kERROR;
  }  
  // Get digi array
  fDigiArray = (TClonesArray*) ioman->GetObject("DrcDigi");
  if ( ! fDigiArray ) {
    cout << "-W- PndDrcLutFill::Init: " << "No DrcDigi array!" << endl;
    return kERROR;
  }
  // Get input array
  fPDHitArray = (TClonesArray*) ioman->GetObject("DrcPDHit");
  if ( ! fPDHitArray ) {
    cout << "-W- PndDrcLutFill::Init: " << "No DrcPDHit array!" << endl;
    return kERROR;
  }
 
  fFile = TFile::Open(fOutputFile,"RECREATE");
  fLut = new TClonesArray("PndDrcLutNode");
  fTree = new TTree("dircsim","Look-up table for DIRC");
  fTree->Branch("LUT",&fLut,256000,0); 

  InitLut();

  cout << "-I- PndDrcLutFill: Intialization successfull" << endl;
  return kSUCCESS;
   
}

void PndDrcLutFill::InitLut()
{
  TClonesArray &fLuta = *fLut;
  Int_t Nnodes = 150000;
  for (Long64_t n=0; n<Nnodes; n++) {
    new((fLuta)[n]) PndDrcLutNode(n);
  }
}

// -----   Execution of Task   ---------------------------------------------
void PndDrcLutFill::Exec(Option_t* option)
{
  nevents++;
  if(fVerbose>0 && nevents%1000==0) std::cout<<"Event # "<< nevents<<std::endl;  
  fDetectorID = 0;
  ProcessPhotonHit();
}

//--------------Process Photon Hits-----------------------------------------
void PndDrcLutFill::ProcessPhotonHit()
{
  Int_t nofChPho = 0;
  // Loop over PndDrcPDHits
  for(Int_t k=0; k<fPDHitArray->GetEntriesFast(); k++) {
   
    fPDHit = (PndDrcPDHit*)fPDHitArray->At(k);

    Int_t digiID= fPDHit->GetRefIndex();
    fDigi = (PndDrcDigi*) fDigiArray->At(digiID);

    Int_t pointID= fDigi->GetIndex(0);
    fPDPoint = (PndDrcPDPoint*)fPDPointArray->At(pointID);
    
    Int_t trackID= fPDPoint->GetTrackID();

    fMCTrack = (PndMCTrack*)fMCArray->At(trackID);
    TVector3 dir =  fMCTrack->GetMomentum().Unit();
    ((PndDrcLutNode*)(fLut->At(fDigi->GetSensorID())))->AddEntry(dir);
  }
}

// -----   Finish Task   ---------------------------------------------------
void PndDrcLutFill::Finish()
{
    fTree->Fill();
    fTree->Write();
    fFile->Write();
 
    fLut->Clear();
    cout << "-I- PndDrcLutFill: Finish" << endl; 
}

ClassImp(PndDrcLutFill)
