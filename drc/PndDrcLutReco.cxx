// -----------------------------------------
// PndDrcLutReco.cpp
//
// Created on: 13.07.2013
// Author: R.Dzhygadlo at gsi.de
// -----------------------------------------

#include "PndDrcLutReco.h"

#include "FairRootManager.h"
#include "PndMCTrack.h"
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

 // Get bar points array
  fBarPointArray = (TClonesArray*) ioman->GetObject("DrcBarPoint");
  if ( ! fBarPointArray ) {
    cout << "-W- PndDrcLutReco::Init: " << "No DrcBarPoint array!" << endl;
    return kERROR;
  }

  // Get ev points array
  fEVPointArray = (TClonesArray*) ioman->GetObject("DrcEVPoint");
  if ( ! fEVPointArray ) {
    cout << "-W- PndDrcLutReco::Init: " << "No DrcEVPoint array!" << endl;
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

  fGeo = new PndGeoDrc();
  fBboxNum    = fGeo->BBoxNum();
  fPipehAngle = fGeo->PipehAngle();
  fDphi       = 2.*(180. - 2*fPipehAngle)/(Double_t)fGeo->BBoxNum();

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
  if ( ! fDrcLutInfoArray ) Fatal("Exec", "No fDrcLutInfoArray");
  fDrcLutInfoArray->Clear();

  int nHits = fPDHitArray->GetEntriesFast();
  if(fVerbose>1) std::cout<<"Event # "<< nevents<<" has "<<nHits<<" hits."<< std::endl;
  else if(fVerbose==1 && nevents%1000==0) std::cout<<"Event # "<< nevents<<" has "<<nHits<<" hits."<< std::endl;

  PndDrcLutInfo lutinfo;
  TVector3 dirm, dir, momAtZero, momInBar,posInBar;
  Double_t cangle,tangle, barPhi;
  Int_t pdgcode;

  TVector3 fnX1 = TVector3 (1,0,0);   
  TVector3 fnY1 = TVector3( 0,1,0); 
  //information retrieved correctly if there is only one primary track
  for(Int_t k=0; k<fMCArray->GetEntriesFast(); k++){
    fMCTrack = (PndMCTrack*)fMCArray->At(k);
    if(fMCTrack->GetMotherID()==-1) {
      momAtZero = fMCTrack->GetMomentum();
      pdgcode = fMCTrack->GetPdgCode();
      Double_t Mrmass;
      if(fabs(pdgcode) == 211){Mrmass = 0.139570;}
      if(fabs(pdgcode) == 321){Mrmass = 0.49368;}

      Double_t Mrmom = momAtZero.Mag();      
      cangle = acos(sqrt(pow(Mrmom,2) + pow(Mrmass,2))/Mrmom/1.46907);
      break;
    }
  }

  // Loop over PndDrcPDHits
  for(Int_t k=0; k<nHits; k++) {
   
    fPDHit = (PndDrcPDHit*)fPDHitArray->At(k);
  
    Int_t digiID= fPDHit->GetRefIndex();
    fDigi = (PndDrcDigi*) fDigiArray->At(digiID);

    Int_t pointID= fDigi->GetIndex(0);
    fPDPoint = (PndDrcPDPoint*)fPDPointArray->At(pointID);
    
    fBarPoint= (PndDrcBarPoint*)fBarPointArray->At(fPDPoint->GetBarPointID());
    fBarPoint->Momentum(momInBar);
    fBarPoint->Position(posInBar);

    pdgcode = fBarPoint->GetPdgCode();
    Double_t startPhi = posInBar.Phi()/TMath::Pi()*180;
    if(startPhi < 0) startPhi = 360 + startPhi;
    if(startPhi >= 0 && startPhi < 90) barPhi = TMath::Floor(startPhi/fDphi) *fDphi + fDphi/2.;
    if(startPhi >= 90 && startPhi < 270) barPhi = 90  + fPipehAngle + TMath::Floor((startPhi-90-fPipehAngle)/fDphi) *fDphi + fDphi/2.;
    if(startPhi >= 270 && startPhi < 360) barPhi = 270 + fPipehAngle + TMath::Floor((startPhi-270-fPipehAngle)/fDphi) *fDphi + fDphi/2.;
    momInBar.RotateZ(-barPhi/180.*TMath::Pi());
    cangle = fBarPoint->GetThetaC();
    
    Int_t trackID = fPDPoint->GetTrackID();
    Int_t evpointcount = 0;
    for(int i=0; i<fEVPointArray->GetEntriesFast(); i++){
      fEVPoint = (PndDrcEVPoint*)fEVPointArray->At(i);
      if(trackID == fEVPoint->GetTrackID()) evpointcount++;
    }

    // Int_t trackID= fPDPoint->GetTrackID();
    // fMCTrack = (PndMCTrack*)fMCArray->At(trackID);
    // TVector3 vert =  fMCTrack->GetStartVertex();
 
    if(fPDHit->GetDetectorID()>150000) {
      std::cout<<"WTQ  fPDHit->GetDetectorID()   "<<fPDHit->GetDetectorID() <<std::endl;
      continue;
    }
    PndDrcLutNode *node= (PndDrcLutNode*) fLut->At(fPDHit->GetDetectorID());
    Int_t size = node->Entries();
    for(int i=0; i<size; i++){
      dirm = node->GetEntry(i);
      dirm.RotateZ(-barPhi/180.*TMath::Pi());

      for(int u=0; u<8; u++){
	if(u == 0) dir = dirm;
	if(u == 1) dir.SetXYZ( dirm.X(), dirm.Y(),-dirm.Z());
	if(u == 2) dir.SetXYZ( dirm.X(),-dirm.Y(), dirm.Z());
	if(u == 3) dir.SetXYZ(-dirm.X(), dirm.Y(), dirm.Z());
	if(u == 4) dir.SetXYZ(-dirm.X(),-dirm.Y(), dirm.Z());
	if(u == 5) dir.SetXYZ(-dirm.X(), dirm.Y(),-dirm.Z());
	if(u == 6) dir.SetXYZ( dirm.X(),-dirm.Y(),-dirm.Z());
	if(u == 7) dir = -dirm;

	tangle = momInBar.Angle(dir);
	if(tangle>TMath::Pi()/2.) tangle = TMath::Pi()-tangle;
	lutinfo.AddAngle(cangle - tangle);


	if(!((dir.Cross(fnX1)).Mag() > 1.00028/fGeo->nQuartz() || (dir.Cross(fnY1)).Mag() > 1.00028/fGeo->nQuartz())){
	  std::cout<<"Wrong combination "<<std::endl;
	}

      }

      // dir = TVector3(dirm.X(),dirm.Y(),-dirm.Z());
      // tangle = momInBar.Angle(dir);
      // if(tangle>TMath::Pi()/2.) tangle = TMath::Pi()-tangle;
      // lutinfo.AddAngle(cangle - tangle);


    }
    lutinfo.AddPixelEnd(lutinfo.AngleEntries());
    lutinfo.AddNOfEVReflections(evpointcount);
  }

  lutinfo.SetChPartDir(momAtZero);
  lutinfo.SetChPartDirInBar(momInBar);
  lutinfo.SetChPartPdg(pdgcode);
  lutinfo.SetCherenkovMC(cangle);
  new ((*fDrcLutInfoArray)[fDrcLutInfoArray->GetEntriesFast()]) PndDrcLutInfo(lutinfo);
}

// -----   Finish Task   ---------------------------------------------------
void PndDrcLutReco::Finish()
{
    fLut->Clear();
    cout << "-I- PndDrcLutReco: Finish" << endl; 
}

ClassImp(PndDrcLutReco)
