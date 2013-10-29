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
#include "PndDrcTrackInfo.h"
#include "PndDrcPhotonInfo.h"
#include "PndDrcAmbiguityInfo.h"

#include "PndGeoHandling.h"

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
  fTree=(TTree *) fFile->Get("dircsim") ;
  for(Int_t l=0; l<5; l++){
    fLut[l] = new TClonesArray("PndDrcLutNode");
    fTree->SetBranchAddress(Form("LUT%d",l),&fLut[l]); 
  }
  fTree->GetEntry(0);


  // Create and register output array
  fDrcTrackInfoArray = new TClonesArray("PndDrcTrackInfo");
  ioman->Register("DrcTrackInfo","Drc",fDrcTrackInfoArray, kTRUE);

  fGeo = new PndGeoDrc();
  fBboxNum    = fGeo->BBoxNum();
  fPipehAngle = fGeo->PipehAngle();
  fBarPhi = 2*atan(((fGeo->BarWidth() + fGeo->barhGap())/2.)/fGeo->radius())*180/TMath::Pi();
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
  if ( ! fDrcTrackInfoArray ) Fatal("Exec", "No fDrcTrackInfoArray");
  fDrcTrackInfoArray->Clear();

  int nHits = fPDHitArray->GetEntriesFast();
  if(fVerbose>1) std::cout<<"Event # "<< nevents<<" has "<<nHits<<" hits."<< std::endl;
  else if(fVerbose==1 && nevents%1000==0) std::cout<<"Event # "<< nevents<<" has "<<nHits<<" hits."<< std::endl;

  PndDrcTrackInfo trackinfo;
  TVector3 dird, dir, momAtZero, momInBar,posInBar;
  Double_t cangle,tangle, boxPhi, evtime, bartime, directz, luttheta, barHitTime, pdHitTime, lutboxPhi=10.825;
  Int_t pdgcode, lutboxId=3;
  Bool_t reflected;

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
      cangle = acos(sqrt(pow(Mrmom,2) + pow(Mrmass,2))/Mrmom/fGeo->nQuartz());
      break;
    }
  }

  // Loop over PndDrcPDHits
  for(Int_t k=0; k<nHits; k++) {
    PndDrcPhotonInfo photoninfo;

    fPDHit = (PndDrcPDHit*)fPDHitArray->At(k);
  
    Int_t digiID= fPDHit->GetRefIndex();
    fDigi = (PndDrcDigi*) fDigiArray->At(digiID);

    Int_t pointID= fDigi->GetIndex(0);
    fPDPoint = (PndDrcPDPoint*)fPDPointArray->At(pointID);
    
    fBarPoint= (PndDrcBarPoint*)fBarPointArray->At(fPDPoint->GetBarPointID());
    fBarPoint->Momentum(momInBar);
    fBarPoint->Position(posInBar);
    Int_t boxId = fBarPoint->GetBoxId();
    //Int_t barId = fBarPoint->GetBarId();
    barHitTime = fBarPoint->GetTime();
    pdgcode = fBarPoint->GetPdgCode();
    cangle = fBarPoint->GetThetaC();
 
    pdHitTime = fPDHit->GetTime();

    Double_t startPhi = posInBar.Phi()/TMath::Pi()*180;
    if(startPhi < 0) startPhi = 360 + startPhi;
    if(startPhi >= 0 && startPhi < 90) boxPhi = TMath::Floor(startPhi/fDphi) *fDphi + fDphi/2.;
    if(startPhi >= 90 && startPhi < 270) boxPhi = 90  + fPipehAngle + TMath::Floor((startPhi-90-fPipehAngle)/fDphi) *fDphi + fDphi/2.;
    if(startPhi >= 270 && startPhi < 360) boxPhi = 270 + fPipehAngle + TMath::Floor((startPhi-270-fPipehAngle)/fDphi) *fDphi + fDphi/2.;

    Double_t trackPhi = momInBar.Phi()*180/TMath::Pi();
    if(trackPhi<0) trackPhi += 360; 

    
    Int_t barId = (int) (2.5 + (boxPhi-trackPhi)/fBarPhi);
    if(barId>4 || barId<0) {
      std::cout<<"Error in PndDrcLutReco:  Bar Id is wrong. barId = "<< barId <<std::endl;
      continue;
    }
    momInBar.RotateZ(-boxPhi/180.*TMath::Pi());
  
    Int_t trackID = fPDPoint->GetTrackID();
    Int_t evpointcount = 0;
    for(int i=0; i<fEVPointArray->GetEntriesFast(); i++){
      fEVPoint = (PndDrcEVPoint*)fEVPointArray->At(i);
      if(trackID == fEVPoint->GetTrackID()) evpointcount++;
    }

    fMCTrack = (PndMCTrack*)fMCArray->At(trackID);
    if(fMCTrack->GetMomentum().Z()>0) reflected = kTRUE;
    else  reflected = kFALSE;
    
    Int_t sensorId = fPDHit->GetSensorId();
    if(sensorId>30000) {
      std::cout<<"WTQ  fPDHit->GetSensorID()   "<<sensorId <<std::endl;
      continue;
    }

    Int_t recalculatedSensorId = (sensorId/100 - (boxId - lutboxId)*17)*100 + sensorId%100; 
    if(sensorId>27200 || recalculatedSensorId>27200) {
      std::cout<<"LUT reco: ignore vertical MCPblock for now.  sensorId =  "<<sensorId << " recalculatedSensorId = "<< recalculatedSensorId  <<std::endl;
      continue;
    }    

    if(recalculatedSensorId <0) continue;
    PndDrcLutNode *node= (PndDrcLutNode*) fLut[barId]->At(recalculatedSensorId);
    Int_t size = node->Entries();
    for(int i=0; i<size; i++){
      dird = node->GetEntry(i);
      dird.RotateZ(-lutboxPhi/180.*TMath::Pi());

      evtime = node->GetTime(i);
      for(int u=0; u<4; u++){
	if(u == 0) dir = dird;
	if(u == 1) dir.SetXYZ( dird.X(),-dird.Y(), dird.Z());
	if(u == 2) dir.SetXYZ(-dird.X(), dird.Y(), dird.Z());
	if(u == 3) dir.SetXYZ(-dird.X(),-dird.Y(), dird.Z());
       	if(reflected) dir.SetXYZ( dir.X(), dir.Y(),-dir.Z());
	
	double criticalAngle = asin(1.00028/fGeo->nQuartz());
	if(dir.Angle(fnX1) < criticalAngle || dir.Angle(fnY1) < criticalAngle) continue;

	luttheta = dir.Theta();	
	if(luttheta > TMath::Pi()/2.) luttheta = TMath::Pi()-luttheta;
	directz = posInBar.Z()+119;
	if(!reflected) bartime = directz/cos(luttheta)/19.8; 
	else bartime = ((240 - directz)*2 + directz)/cos(luttheta)/19.8; 
	
	// if(fabs((bartime + evtime)-(pdHitTime-barHitTime))>2) continue;

	tangle = momInBar.Angle(dir);
	if(tangle>TMath::Pi()/2.) tangle = TMath::Pi()-tangle;
	 
	PndDrcAmbiguityInfo ambinfo;
	ambinfo.SetBarTime(bartime);
	ambinfo.SetEvTime(evtime);
	ambinfo.SetCherencov(tangle);
	photoninfo.AddAmbiguity(ambinfo);
      }
    }

    photoninfo.SetHitTime(pdHitTime);
    photoninfo.SetReflected(reflected);
    photoninfo.SetEvReflections(evpointcount);
    
    trackinfo.AddPhoton(photoninfo);
  }

  trackinfo.SetMcMomentum(momAtZero);
  trackinfo.SetMcMomentumInBar(momInBar);
  trackinfo.SetMcPdg(pdgcode);
  trackinfo.SetMcCherenkov(cangle);
  trackinfo.SetMcTimeInBar(barHitTime);
  new ((*fDrcTrackInfoArray)[fDrcTrackInfoArray->GetEntriesFast()]) PndDrcTrackInfo(trackinfo);
}

// -----   Finish Task   ---------------------------------------------------
void PndDrcLutReco::Finish()
{
  for(Int_t l=0; l<10; l++){
    fLut[l]->Clear(); 
  }
  
  cout << "-I- PndDrcLutReco: Finish" << endl; 
}

ClassImp(PndDrcLutReco)
