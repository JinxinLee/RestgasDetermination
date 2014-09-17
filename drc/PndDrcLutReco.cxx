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
#include "TRandom.h"
#include "TSystem.h"

#include "TCanvas.h"

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

  fHist = new TH1F("chrenkov_angle_hist","chrenkov_angle_hist", 100,0.35,0.85);
  fFit = new TF1("fgaus","[0]*exp(-0.5*((x-[1])/[2])*(x-[1])/[2])",0.35,0.85);
  fSpect = new TSpectrum(10);
 
  cout << "-I- PndDrcLutReco: Intialization successfull" << endl;
  return kSUCCESS;
   
}

// -----   Execution of Task   ---------------------------------------------
void PndDrcLutReco::Exec(Option_t* option)
{
  nevents++;
  fDetectorID = 0;

  if ( ! fDrcTrackInfoArray ) Fatal("Exec", "No fDrcTrackInfoArray");
  fDrcTrackInfoArray->Clear();
  Int_t nHits = fPDHitArray->GetEntriesFast();
  if(fVerbose>1) std::cout<<"Event # "<< nevents<<" has "<<nHits<<" hits."<< std::endl;
  else if(fVerbose==1 && nevents%100==0) std::cout<<"Event # "<< nevents<<" has "<<nHits<<" hits."<< std::endl;

  if(fVerbose<2) gROOT->SetBatch(kTRUE);
  ProcessPhotonHit();
  if(fVerbose<2) gROOT->SetBatch(kFALSE);
}

//--------------Process Photon Hits-----------------------------------------
void PndDrcLutReco::ProcessPhotonHit()
{

  PndDrcTrackInfo trackinfo;
  TVector3 dird, dir, momInBar, posInBar,
    fnX1 = TVector3 (1,0,0),   
    fnY1 = TVector3( 0,1,0);
  
  Double_t cangle, tangle, boxPhi, evtime, bartime, directz, luttheta, barHitTime, pdHitTime, 
    lutboxPhi=10.825, window1, window2, angdiv, dtheta, dtphi;
  Int_t pdgcode, lutboxId=3;
  Bool_t reflected, testTrRes = false;

  if(testTrRes){
    Int_t rndm=0;
    if (gSystem->Getenv("RANDOM")) {
      rndm = atoi(gSystem->Getenv("RANDOM"));
    }
    gRandom->SetSeed(rndm);
    angdiv = 2*TMath::Pi()/180.;
    dtheta = gRandom->Uniform(-angdiv,angdiv);
    dtphi = gRandom->Uniform(-angdiv,angdiv);
  }

  // loop over MC tracks
  for(Int_t itrack=0; itrack<fMCArray->GetEntriesFast(); itrack++){
    fMCTrack = (PndMCTrack*)fMCArray->At(itrack);
    if( fMCTrack->GetMotherID()==-1) {
      Int_t mcboxId = -1;
      for(int i=0; i<fBarPointArray->GetEntriesFast(); i++){
	PndDrcBarPoint *barPoint = (PndDrcBarPoint*)fBarPointArray->At(i);
	if(itrack == barPoint->GetTrackID()) {
	  mcboxId = barPoint->GetBoxId();
	  break;
	}
      }
      window1 = (mcboxId)*17-17;
      window2 = (mcboxId+1)*17+17;
 
      // Loop over PndDrcPDHits
      for(Int_t ihit=0; ihit<fPDHitArray->GetEntriesFast(); ihit++) {
	PndDrcPhotonInfo photoninfo;

	fPDHit = (PndDrcPDHit*)fPDHitArray->At(ihit);
	Int_t wsensorId = fPDHit->GetSensorId()/100;
       	if(wsensorId < window1 || wsensorId > window2) {
	  // std::cout<<"wsensorId  "<<wsensorId << "  "<< window1<<" - "<< window2 <<std::endl;
	  continue;
	}

	Int_t digiID= fPDHit->GetRefIndex();
	fDigi = (PndDrcDigi*) fDigiArray->At(digiID);

	Int_t pointID= fDigi->GetIndex(0);
	fPDPoint = (PndDrcPDPoint*)fPDPointArray->At(pointID);
    
	fBarPoint= (PndDrcBarPoint*)fBarPointArray->At(fPDPoint->GetBarPointID());
	fBarPoint->Momentum(momInBar);
	photoninfo.SetMcPrimeMomentumInBar(momInBar);

	if(testTrRes){
	  double phiinit = momInBar.Phi();
	  momInBar.RotateZ(-phiinit);
	  momInBar.RotateY(dtheta);
	  momInBar.RotateZ(phiinit);
	  momInBar.RotateZ(dtphi);
	}
 
	fBarPoint->Position(posInBar);
	Int_t boxId = fBarPoint->GetBoxId();
	Int_t barId = fBarPoint->GetBarId();
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

	//Int_t barId = (int) (2.5 + (boxPhi-trackPhi)/fBarPhi);
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
	if(((PndMCTrack*)fMCArray->At(trackID))->GetMomentum().Z()>0) reflected = kTRUE;
	else  reflected = kFALSE;
    
	Int_t sensorId = fPDHit->GetSensorId();

	Int_t recalculatedSensorId = (sensorId/100 - (boxId - lutboxId)*17)*100 + sensorId%100; 
	if(sensorId>27200 || recalculatedSensorId>27200 || recalculatedSensorId <0) {
	  std::cout<<"LUT reco: ignore vertical MCPblock for now.  sensorId =  "<<sensorId << " recalculatedSensorId = "<< recalculatedSensorId  <<std::endl;
	  continue;
	}    

	PndDrcLutNode *node = (PndDrcLutNode*) fLut[barId]->At(recalculatedSensorId);
	Int_t size = node->Entries();
	for(int i=0; i<size; i++){
	  dird = node->GetEntry(i);
	  // dird.RotateZ(-lutboxPhi/180.*TMath::Pi());

	  evtime = node->GetTime(i);
	  for(int u=0; u<4; u++){
	    if(u == 0) dir = dird;
	    if(u == 1) dir.SetXYZ( dird.X(),-dird.Y(), dird.Z());
	    if(u == 2) dir.SetXYZ(-dird.X(), dird.Y(), dird.Z());
	    if(u == 3) dir.SetXYZ(-dird.X(),-dird.Y(), dird.Z());
	    if(reflected) dir.SetXYZ( dir.X(), dir.Y(),-dir.Z());
	    // if(reflected) dir.RotateX(-2./180.*TMath::Pi());
	
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
	    if(tangle > 0.35 && tangle < 0.85) fHist->Fill(tangle);
	  }
	}

	photoninfo.SetHitTime(pdHitTime);
	photoninfo.SetReflected(reflected);
	photoninfo.SetEvReflections(evpointcount);
	photoninfo.SetMcCherenkovInBar(cangle);

	trackinfo.AddPhoton(photoninfo);
      }

      Double_t cherenkovreco = FindPeak();
      Int_t pdgreco = FindPdg(fMCTrack->GetMomentum().Mag(), cherenkovreco);

      if(fVerbose>0) std::cout<<"pdg = " <<  fMCTrack->GetPdgCode() <<"-"<< pdgreco<< " reconstructed cherenkov vs. mc " <<cherenkovreco << " " << cangle <<std::endl;
      // if(fabs(cherenkovreco-cangle)>0.05 && fHist->GetEntries()>20){
      // 	if(fVerbose>1){
      // 	  TCanvas* c = new TCanvas("c","c",0,0,800,1200);
      // 	  fHist->Draw();
      // 	  c->Modified();
      // 	  c->Update();
      // 	  c->WaitPrimitive();
      // 	}
      // }
      // fHist->Reset();

      if(testTrRes) trackinfo.SetMomentum(TVector3(dtheta,dtphi,0)); //track deviation
      trackinfo.SetMcMomentum(fMCTrack->GetMomentum());
      trackinfo.SetMcMomentumInBar(momInBar);
      trackinfo.SetMcPdg(fMCTrack->GetPdgCode());
      trackinfo.SetPdg(pdgreco);
      trackinfo.SetMcCherenkov(cangle);
      trackinfo.SetCherenkov(cherenkovreco);
      trackinfo.SetMcTimeInBar(barHitTime);
      new ((*fDrcTrackInfoArray)[fDrcTrackInfoArray->GetEntriesFast()]) PndDrcTrackInfo(trackinfo);
    }
  }
}

Int_t g_num =0;
Double_t PndDrcLutReco::FindPeak(){
  Double_t cherenkovreco = -1;
  if(fHist->GetEntries()>20 ){
    Int_t nfound = fSpect->Search(fHist,1,"",0.6);
    Float_t *xpeaks = fSpect->GetPositionX();
    if(nfound>0) cherenkovreco = xpeaks[0];
    fFit->SetParameter(1,cherenkovreco);   // peak
    fFit->SetParameter(2,0.01); // width
    fHist->Fit("fgaus","Q","",cherenkovreco-0.02,cherenkovreco+0.02);
    cherenkovreco = fFit->GetParameter(1);
    if(cherenkovreco<0 || cherenkovreco>1 ) cherenkovreco = 0;
  
    if(fVerbose>1){
      TCanvas* c = new TCanvas("c","c",0,0,800,600);
      fHist->GetXaxis()->SetTitle("#theta_{C}, [rad]");
      fHist->GetYaxis()->SetTitle("Entries, [#]");
      fHist->Draw();
      c->Modified();
      c->Update();
      c->WaitPrimitive();
      c->Print(Form("pic/animpid/animpid_%d.png",g_num++));
    }
  }
  fHist->Reset();

  return cherenkovreco;
}

Int_t PndDrcLutReco::FindPdg(Double_t mom, Double_t cangle){
  Int_t pdg[]={11,13,211,321,2212};
  Double_t mass[] = {0.000511,0.1056584,0.139570,0.49368,0.9382723};
  Double_t tdiff, diff=100;
  Int_t minid=0;
  for(Int_t i=0; i<5; i++){
    tdiff = fabs(cangle - acos(sqrt(mom*mom + mass[i]*mass[i])/mom/1.46907)); //1.46907 - fused silica
    if(tdiff<diff){
      diff = tdiff;
      minid = i;
    }
  }
  return pdg[minid]; 
}

// -----   Finish Task   ---------------------------------------------------
void PndDrcLutReco::Finish(){
  for(Int_t l=0; l<10; l++){
    fLut[l]->Clear(); 
  }
  cout << "-I- PndDrcLutReco: Finish" << endl; 
}

ClassImp(PndDrcLutReco)
