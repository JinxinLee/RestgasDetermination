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

  // Get bar points array
  fBarPointArray = (TClonesArray*) ioman->GetObject("DrcBarPoint");
  if ( ! fBarPointArray ) {
    cout << "-W- PndDrcLutReco::Init: " << "No DrcBarPoint array!" << endl;
    return kERROR;
  }

  // Get Photon point array
  fPDPointArray = (TClonesArray*) ioman->GetObject("DrcPDPoint");
  if ( ! fPDPointArray ) {
    cout << "-W- PndDrcLutFill::Init: " << "No DrcPDPoint array!" << endl;
    return kERROR;
  }  

  // Get ev points array
  fEVPointArray = (TClonesArray*) ioman->GetObject("DrcEVPoint");
  if ( ! fEVPointArray ) {
    cout << "-W- PndDrcLutFill::Init: " << "No DrcEVPoint array!" << endl;
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
  fTree = new TTree("dircsim","Look-up table for DIRC"); 
  for(Int_t l=0; l<5; l++){
    fLut[l] = new TClonesArray("PndDrcLutNode");
    fTree->Branch(Form("LUT%d",l),&fLut[l],256000,0); 
  }
 
  InitLut();

  fGeo = new PndGeoDrc();
  fBboxNum    = fGeo->BBoxNum();
  fPipehAngle = fGeo->PipehAngle();
  fDphi       = 2.*(180. - 2*fPipehAngle)/(Double_t)fGeo->BBoxNum();

  cout << "-I- PndDrcLutFill: Intialization successfull" << endl;
  return kSUCCESS;
   
}

void PndDrcLutFill::InitLut()
{
  Int_t Nnodes = 30000;
  for(Int_t l=0; l<5; l++){
    TClonesArray &fLuta = *fLut[l]; 
    for (Long64_t n=0; n<Nnodes; n++) {
      new((fLuta)[n]) PndDrcLutNode(-1);
    }
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
  // LUT was generated for:
  Int_t  lutboxId=3;
  Double_t  lutboxPhi=10.825;

  Int_t nofChPho = 0;
  Double_t id, barPhi;
  TVector3 dir, dirm, vec, posInBar;

  // Loop over PndDrcPDHits
  for(Int_t k=0; k<fPDHitArray->GetEntriesFast(); k++) {
   
    fPDHit = (PndDrcPDHit*)fPDHitArray->At(k);

    Int_t digiID= fPDHit->GetRefIndex();
    fDigi = (PndDrcDigi*) fDigiArray->At(digiID);

    Int_t pointID= fDigi->GetIndex(0);
    std::cout<<"pointID "<<pointID <<std::endl;
    
    fPDPoint = (PndDrcPDPoint*)fPDPointArray->At(pointID);
    fBarPoint= (PndDrcBarPoint*)fBarPointArray->At(fPDPoint->GetBarPointID());
    Int_t barId = fBarPoint->GetBarId();
 
    Int_t trackID = fPDPoint->GetTrackID();
    Double_t time = fPDPoint->GetTime();
    Int_t nev=0; 
    id=0;
    for(int i=0; i<fEVPointArray->GetEntriesFast(); i++){
      fEVPoint = (PndDrcEVPoint*)fEVPointArray->At(i);
      if(trackID == fEVPoint->GetTrackID()){
	// if(fEVPoint->GetDetectorID()==9375) {
	//   // get the direction of the phothon at the ent of the bar
	//   fEVPoint->Momentum(dirm);
	// }
	nev++;
	vec = fEVPoint->GetNormal();
	id += (vec.X()+vec.Y()*10 + vec.Z()*100)*1000*nev;
      }
    }
    
    fMCTrack = (PndMCTrack*)fMCArray->At(trackID);
    dir =  fMCTrack->GetMomentum().Unit();
    dir.RotateZ(-lutboxPhi/180.*TMath::Pi());

    Int_t sensorId = fDigi->GetSensorId();
    // sensorId =  (sensorId/100 + lutboxId*17)*100 + sensorId%100; 
 
    if(sensorId>30000 || sensorId<0) {
      std::cout<<"WTQ  fPDHit->GetDetectorID()   "<<sensorId <<std::endl;
      continue;
    }

    // //======================
    // posInBar = fMCTrack->GetStartVertex();
    // Double_t phi = posInBar.Phi()/TMath::Pi()*180;
    // if(phi < 0) phi = 360 + phi;
    // if(phi >= 0 && phi < 90) barPhi = TMath::Floor(phi/fDphi) *fDphi + fDphi/2.;
    // if(phi >= 90 && phi < 270) barPhi = 90  + fPipehAngle + TMath::Floor((phi-90-fPipehAngle)/fDphi) *fDphi + fDphi/2.;
    // if(phi >= 270 && phi < 360) barPhi = 270 + fPipehAngle + TMath::Floor((phi-270-fPipehAngle)/fDphi) *fDphi + fDphi/2.;

    // dirm.RotateZ(-barPhi/180.*TMath::Pi());
    // TVector3 fnX1 = TVector3 (1,0,0);   
    // TVector3 fnY1 = TVector3( 0,1,0); 
    // double criticalAngle = asin(1.00028/fGeo->nQuartz()); 
    // for(int u=0; u<8; u++){
    //   if(u == 0) dir = dirm;
    //   if(u == 1) dir.SetXYZ( dirm.X(), dirm.Y(),-dirm.Z());
    //   if(u == 2) dir.SetXYZ( dirm.X(),-dirm.Y(), dirm.Z());
    //   if(u == 3) dir.SetXYZ(-dirm.X(), dirm.Y(), dirm.Z());
    //   if(u == 4) dir.SetXYZ(-dirm.X(),-dirm.Y(), dirm.Z());
    //   if(u == 5) dir.SetXYZ(-dirm.X(), dirm.Y(),-dirm.Z());
    //   if(u == 6) dir.SetXYZ( dirm.X(),-dirm.Y(),-dirm.Z());
    //   if(u == 7) dir = -dirm;

    //   if(dir.Angle(fnX1) < criticalAngle || dir.Angle(fnY1) < criticalAngle){
    // 	//std::cout<<"Wrong combination "<<std::endl;
    // 	continue;
    //   }
    //   ((PndDrcLutNode*)(fLut->At(sensorId)))->AddEntry(dir);
    //   ((PndDrcLutNode*)(fLut->At(sensorId)))->AddPathId(id);
    // }
    // //======================


    ((PndDrcLutNode*)(fLut[barId]->At(sensorId)))->AddEntry(fDigi->GetDetectorId(), dir,id,time,fPDHit->GetPosition());
  }
}


// -----   Finish Task   ---------------------------------------------------
void PndDrcLutFill::Finish()
{
    fTree->Fill();
    fTree->Write();
    fFile->Write();
 
    for(Int_t l=0; l<10; l++){
      fLut[l]->Clear(); 
    }
    cout << "-I- PndDrcLutFill: Finish" << endl; 
}

ClassImp(PndDrcLutFill)
