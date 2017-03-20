// -------------------------------------------------------------------------
// -----                PndGlobalTrackFitQA source file             -----
// -----                Created 02.06.2009 by R. Karabowicz            -----
// -------------------------------------------------------------------------

#include "PndGlobalTrackFitQA.h"

// FairRoot includes
#include "FairRootManager.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"
#include "FairBaseParSet.h"
#include "FairTrackParam.h"
#include "FairRootManager.h"
// Pnd includes
#include "PndTrack.h"
#include "PndTrackCand.h"
#include "PndTrackCandHit.h"
#include "PndDetectorList.h"
// ROOT includes
#include "TClonesArray.h"
#include "TGeoManager.h"
#include "TMatrixFSym.h"

// C++ includes
#include <iostream>
#include <iomanip>
#include <map>
#include <cmath>
using std::cout;
using std::endl;
using std::setw;
using std::map;

// -----   Default constructor   ------------------------------------------
PndGlobalTrackFitQA::PndGlobalTrackFitQA() : FairTask("Global Track Fit QA", 1) {
  fMCTrackArray  = NULL;
  fRecoTrackArray = NULL;
  fFitTrackArray = NULL;
  fNofEvents     = 0;
  fNofMCTracks   = 0;
  fNofRecoTracks = 0;
  fNofFitTracks  = 0;
}
// -------------------------------------------------------------------------



// -----   Standard constructor   ------------------------------------------
PndGlobalTrackFitQA::PndGlobalTrackFitQA(Int_t iVerbose) 
  : FairTask("Global Track Fit QA", iVerbose) {
  fMCTrackArray  = NULL;
  fRecoTrackArray = NULL;
  fFitTrackArray = NULL;
  fNofEvents     = 0;
  fNofMCTracks   = 0;
  fNofRecoTracks = 0;
  fNofFitTracks  = 0;
}
// -------------------------------------------------------------------------

// -----   Destructor   ----------------------------------------------------
PndGlobalTrackFitQA::~PndGlobalTrackFitQA() { }

// -----   Init  -----------------------------------------------------------
InitStatus PndGlobalTrackFitQA::Init() {
  
  // Get and check FairRootManager
  FairRootManager* ioman = FairRootManager::Instance();
  if( !ioman ) {
    cout << "-E- "<< GetName() <<"::Init: "
	 << "RootManager not instantised!" << endl;
    return kERROR;
  }
  
  // Get the pointer to the singleton FairRunAna object
  FairRunAna* ana = FairRunAna::Instance();
  if(NULL == ana) {
    cout << "-E- "<< GetName() <<"::Init :"
	 <<" no FairRunAna object!" << endl;
    return kERROR;
  }
  // Get the pointer to run-time data base
  FairRuntimeDb* rtdb = ana->GetRuntimeDb();
  if(NULL == rtdb) {
    cout << "-E- "<< GetName() <<"::Init :"
	 <<" no runtime database!" << endl;
    return kERROR;
  }
  
  // Get MCTrack array
  fMCTrackArray  = (TClonesArray*) ioman->GetObject("MCTrack");
  if( !fMCTrackArray ) {
    cout << "-E- "<< GetName() <<"::Init: No MCTrack array!"
	 << endl;
    return kERROR;
  }

  // Get Global track Array
  fRecoTrackArray  = (TClonesArray*) ioman->GetObject("GlobalTrack");
  if ( !fRecoTrackArray ) {
    cout << "-E- " << GetName() << "::Init: No Global PndTrack array!" << endl;
    return kERROR;
  }
  
  // Get Global Fitted track Array
  fFitTrackArray  = (TClonesArray*) ioman->GetObject("GlobalFitTrack");
  if ( !fFitTrackArray ) {
    cout << "-E- " << GetName() << "::Init: No Global fitted PndTrack array!" << endl;
    return kERROR;
  }
  
  std::cout << "-I- "<< GetName() <<": Intialization successfull" << std::endl;

  CreateHistos();

  return kSUCCESS;
}
// -------------------------------------------------------------------------

// -----   Private method ReInit   -----------------------------------------
InitStatus PndGlobalTrackFitQA::ReInit() {

  return kSUCCESS;
}
// -------------------------------------------------------------------------

// -----   Private method SetParContainers   -------------------------------
void PndGlobalTrackFitQA::SetParContainers() {
  
  // Get run and runtime database
  FairRunAna* run = FairRunAna::Instance();
  if ( ! run ) Fatal("SetParContainers", "No analysis run");

  FairRuntimeDb* db = run->GetRuntimeDb();
  if ( ! db ) Fatal("SetParContainers", "No runtime database");

  fNofEvents = 0;
  fTargetPos.SetXYZ(0.,0.,0.);
}
// -------------------------------------------------------------------------


// -----   Public method Exec   --------------------------------------------
void PndGlobalTrackFitQA::Exec(Option_t*) {
  
  fNofEvents++;

  cout << "=event " << fNofEvents << "========================================================================" << endl;

  PrepareMCTracks();
  MatchRecoTracks();
  MatchFitTracks();

  Int_t nofMCTracks   = fMCTrackArray  ->GetEntriesFast();
  Int_t nofRecoTracks = fRecoTrackArray->GetEntriesFast();
  Int_t nofFitTracks  = fFitTrackArray ->GetEntriesFast();

  fNofMCTracks   += nofMCTracks;
  fNofRecoTracks += nofRecoTracks;
  fNofFitTracks  += nofFitTracks;

  fhNofMCTracksPerEvent  ->Fill(fNofEvents-1,nofMCTracks);
  fhNofRecoTracksPerEvent->Fill(fNofEvents-1,nofRecoTracks);
  fhNofFitTracksPerEvent ->Fill(fNofEvents-1,nofFitTracks);

  PndMCTrack* mcTrack;
  PndTrack*   recoTrack;
  PndTrack*   fitTrack;

  for ( Int_t ift = 0 ; ift < nofFitTracks ; ift++ ) {

    fitTrack  = (PndTrack*)  fFitTrackArray ->At(ift);
    recoTrack = (PndTrack*)  fRecoTrackArray->At(fFitTrackRecoMatch[ift]);
    mcTrack   = (PndMCTrack*)fMCTrackArray  ->At(fRecoTrackMCMatch[fFitTrackRecoMatch[ift]]);
//     cout << "FIT TRACK " << ift 
// 	 << " POINTS TO RECO TRACK " << fFitTrackRecoMatch[ift]
// 	 << " POINTS TO MC TRACK " << fRecoTrackMCMatch[fFitTrackRecoMatch[ift]] << endl;

    if ( !fitTrack  ) cout << "NO FIT TRACK" << endl;
    if ( !recoTrack ) cout << "NO RECO TRACK" << endl;
    if ( !mcTrack   ) cout << "NO MC TRACK" << endl;
    if ( !fitTrack  || 
	 !recoTrack ||
	 !mcTrack   ) continue;

//     cout << ">>>> track " << ift << " momentum" << endl;
//     cout << "MC   -> " << mcTrack  ->                GetMomentum().Mag() << endl;
//     cout << "RECO -> " << recoTrack->GetParamFirst().GetMomentum().Mag() << endl;
//     cout << "FIT  -> " << fitTrack ->GetParamFirst().GetMomentum().Mag() << endl;

    TVector3 mcMom   = mcTrack  ->                GetMomentum();
    TVector3 recoMom = recoTrack->GetParamFirst().GetMomentum();
    TVector3 fitMom  = fitTrack ->GetParamFirst().GetMomentum();

    Double_t mcMomMag     =   mcMom.Mag();
    //Double_t recoMomMag   = recoMom.Mag(); //[R.K. 01/2017] unused variable?
    Double_t fitMomMag    =  fitMom.Mag();
    Double_t mcMomTheta   =   mcMom.Theta()*TMath::RadToDeg();
    //Double_t recoMomTheta = recoMom.Theta()*TMath::RadToDeg(); //[R.K. 01/2017] unused variable?
    Double_t fitMomTheta  =  fitMom.Theta()*TMath::RadToDeg();

    Int_t   magBin = (Int_t)(mcMomMag  /0.5);
    Int_t thetaBin = (Int_t)(mcMomTheta/5.0);

    if (   magBin >= 20 )   magBin = 19;
    if ( thetaBin >= 20 ) thetaBin = 19;

    Double_t   magImpr = (fitMomMag  -mcMomMag  )/(mcMomMag  )*100.;
    Double_t thetaImpr = (fitMomTheta-mcMomTheta)/(mcMomTheta)*100.;

    fhMomentumImprovementVsP                     ->Fill(mcMomMag,  magImpr);
    fhMomentumImprovementVsPThetaBinned[thetaBin]->Fill(mcMomMag,  magImpr);
    fhMomentumImprovementVsT                     ->Fill(mcMomTheta,magImpr);
    fhMomentumImprovementVsTMomBinned  [magBin]  ->Fill(mcMomTheta,magImpr);
    
    fhThetaImprovementVsP                        ->Fill(mcMomMag,  thetaImpr);
    fhThetaImprovementVsPThetaBinned   [thetaBin]->Fill(mcMomMag,  thetaImpr);
    fhThetaImprovementVsT                        ->Fill(mcMomTheta,thetaImpr);
    fhThetaImprovementVsTMomBinned     [magBin]  ->Fill(mcMomTheta,thetaImpr);

    fhMomentumImprovementVsPDet                  ->Fill(mcMomMag,  magImpr);
    fhMomentumImprovementVsTDet                  ->Fill(mcMomTheta,magImpr);
    fhThetaImprovementVsPDet                     ->Fill(mcMomMag,  thetaImpr);
    fhThetaImprovementVsTDet                     ->Fill(mcMomTheta,thetaImpr);
  }

  if ( fVerbose ) {
    cout << "---------------------------------------------------------------" << endl; 
  }
  
}
// ------------------------------------------------------------

// -----   Private method PrepareMCTrack   --------------------------------------------
void PndGlobalTrackFitQA::PrepareMCTracks() {
  
  const Int_t nofMCTracks    = fMCTrackArray->GetEntriesFast();

  if ( fVerbose ) 
    cout << "There are " << nofMCTracks << " MC tracks" << endl;
}
// ------------------------------------------------------------

// -----   Private method MatchRecoTracks   --------------------------------------------
void PndGlobalTrackFitQA::MatchRecoTracks() {
  
  Int_t nofRecoTracks = fRecoTrackArray->GetEntriesFast();

  if ( fVerbose ) 
    cout << "There are " << nofRecoTracks << " Reco tracks" << endl;

  fRecoTrackMCMatch.clear();
  fRecoTrackMCMatch.resize(nofRecoTracks,-1);

  PndTrack* recoTrack;

  for ( Int_t irtr = 0 ; irtr < nofRecoTracks ; irtr++ ) {
    recoTrack = (PndTrack*) fRecoTrackArray->At(irtr);

    Int_t bestMCId = recoTrack->GetRefIndex();

    if ( bestMCId == -1 ) { continue; }

    fRecoTrackMCMatch[irtr] = bestMCId;

    //    cout << "reco " << irtr << " -> mc " << bestMCId << endl;
  }
  
}
// ------------------------------------------------------------

// -----   Private method MatchFitTracks   --------------------------------------------
void PndGlobalTrackFitQA::MatchFitTracks() {
  
  Int_t nofFitTracks = fFitTrackArray->GetEntriesFast();

  if ( fVerbose ) 
    cout << "There are " << nofFitTracks << " Fit tracks" << endl;

  fFitTrackRecoMatch.clear();
  fFitTrackRecoMatch.resize(nofFitTracks,-1);

  PndTrack* fitTrack;

  for ( Int_t iftr = 0 ; iftr < nofFitTracks ; iftr++ ) {
    fitTrack = (PndTrack*) fFitTrackArray->At(iftr);

    Int_t recoId = fitTrack->GetRefIndex();

    if ( recoId == -1 ) { continue; }

    fFitTrackRecoMatch[iftr] = recoId;

    //    cout << "fit " << iftr << " -> reco " << recoId << endl;
  }
  
}
// ------------------------------------------------------------

// -----   Private method CreateHistos   --------------------------------------------
void PndGlobalTrackFitQA::CreateHistos() {
  fHistoFList = new TList();
  // number of mc tracks, reco tracks, efficiency as function of MOMENTUM

  Double_t minMom =  0.;
  Double_t maxMom = 10.;
  Int_t   momBins = 40;

  Double_t minThe =   0.;
  Double_t maxThe = 150.;
  Int_t   theBins = 150;

  Double_t minYAx = -20.;
  Double_t maxYAx =  20.;
  Int_t   yAxBins = 400;

  //----------------------------------------------------------------------------------------------------------------
  fhMomentumImprovementVsP = new TH2F("hMomentumImprovementVsP",
				      "momentum improvement;p_{mc} [GeV/c];(p_{fit}-p{mc})/p_{mc} [%]",
				      momBins,minMom,maxMom,yAxBins,minYAx,maxYAx);
  fHistoFList->Add(fhMomentumImprovementVsP);

  fhMomentumImprovementVsT = new TH2F("hMomentumImprovementVsT",
				      "momentum improvement;#theta_{mc} [deg];(p_{fit}-p{mc})/p_{mc} [%]",
				      theBins,minThe,maxThe,yAxBins,minYAx,maxYAx);
  fHistoFList->Add(fhMomentumImprovementVsT);

  fhThetaImprovementVsP = new TH2F("hThetaImprovementVsP",
				   "theta improvement;p [GeV/c];(T_{fit}-T{mc})/T_{mc} [%]",
				   momBins,minMom,maxMom,yAxBins,minYAx,maxYAx);
  fHistoFList->Add(fhThetaImprovementVsP);

  fhThetaImprovementVsT = new TH2F("hThetaImprovementVsT",
				   "theta improvement;#theta_{mc} [deg];(T_{fit}-T{mc})/T_{mc} [%]",
				   theBins,minThe,maxThe,yAxBins,minYAx,maxYAx);
  fHistoFList->Add(fhThetaImprovementVsT);
  //----------------------------------------------------------------------------------------------------------------
  fhMomentumImprovementVsPDet = new TH2F("hMomentumImprovementVsPDet",
					 "momentum improvement;p_{mc} [GeV/c];(p_{fit}-p{mc})/p_{mc} [%]",
					 momBins,minMom,maxMom,yAxBins*10,minYAx/10.,maxYAx/10.);
  fHistoFList->Add(fhMomentumImprovementVsPDet);

  fhMomentumImprovementVsTDet = new TH2F("hMomentumImprovementVsTDet",
					 "momentum improvement;#theta_{mc} [deg];(p_{fit}-p{mc})/p_{mc} [%]",
					 theBins,minThe,maxThe,yAxBins*10,minYAx/10.,maxYAx/10.);
  fHistoFList->Add(fhMomentumImprovementVsTDet);
  
  fhThetaImprovementVsPDet = new TH2F("hThetaImprovementVsPDet",
				      "theta improvement;p [GeV/c];(T_{fit}-T{mc})/T_{mc} [%]",
				      momBins,minMom,maxMom,yAxBins*10,minYAx/10.,maxYAx/10.);
  fHistoFList->Add(fhThetaImprovementVsPDet);

  fhThetaImprovementVsTDet = new TH2F("hThetaImprovementVsTDet",
				      "theta improvement;#theta_{mc} [deg];(T_{fit}-T{mc})/T_{mc} [%]",
				      theBins,minThe,maxThe,yAxBins*10,minYAx/10.,maxYAx/10.);
  fHistoFList->Add(fhThetaImprovementVsTDet);
  //----------------------------------------------------------------------------------------------------------------
  for ( Int_t ithe = 0 ; ithe < 20 ; ithe++ ) {
    Double_t ttm = ithe*5.;
    Double_t ttp = ttm+5.;
    fhMomentumImprovementVsPThetaBinned[ithe] = new TH2F(Form("hMomentumImprovementVsPThetaBin%d",ithe),
							 Form("momentum improvement, %.1f < #theta < %.1f;p_{mc} [GeV/c];(p_{fit}-p{mc})/p_{mc} [%%]",ttm,ttp),
							 momBins,minMom,maxMom,yAxBins,minYAx,maxYAx);
    fHistoFList->Add(fhMomentumImprovementVsPThetaBinned[ithe]);

    fhThetaImprovementVsPThetaBinned[ithe] = new TH2F(Form("hThetaImprovementVsPThetaBin%d",ithe),
						      Form("theta improvement, %.1f < #theta < %.1f;p_{mc} [GeV/c];(T_{fit}-T{mc})/T_{mc} [%%]",ttm,ttp),
						      momBins,minMom,maxMom,yAxBins,minYAx,maxYAx);
    fHistoFList->Add(fhThetaImprovementVsPThetaBinned[ithe]);
  }
  
  for ( Int_t imom = 0 ; imom < 20 ; imom++ ) {
    Double_t tmm = imom*.5;
    Double_t tmp = tmm+.5;
    fhMomentumImprovementVsTMomBinned[imom] = new TH2F(Form("hMomentumImprovementVsTMomBin%d",imom),
						       Form("momentum improvement, %.1f < |p| < %.1f;#theta_{mc} [deg];(p_{fit}-p{mc})/p_{mc} [%%]",tmm,tmp),
						       theBins,minThe,maxThe,yAxBins,minYAx,maxYAx);
    fHistoFList->Add(fhMomentumImprovementVsTMomBinned[imom]);

    fhThetaImprovementVsTMomBinned[imom] = new TH2F(Form("hThetaImprovementVsTMomBin%d",imom),
						    Form("theta improvement, %.1f < |p| < %.1f;#theta_{mc} [deg];(T_{fit}-T{mc})/T_{mc} [%%]",tmm,tmp),
						    theBins,minThe,maxThe,yAxBins,minYAx,maxYAx);
    fHistoFList->Add(fhThetaImprovementVsTMomBinned[imom]);
  }


  fhNofMCTracksPerEvent   = new TH1F("hNofMCTracksPerEvent","nof MC tracks per event;event no;nof mc tracks",100001,-0.5,100000.5);
  fhNofRecoTracksPerEvent = new TH1F("hNofRecoTracksPerEvent","nof reco tracks per event;event no;nof reco tracks",100001,-0.5,100000.5);
  fhNofFitTracksPerEvent  = new TH1F("hNofFitTracksPerEvent","nof fitted tracks per event;event no;nof fitted tracks",100001,-0.5,100000.5);
  fHistoFList->Add(fhNofMCTracksPerEvent);
  fHistoFList->Add(fhNofRecoTracksPerEvent);
  fHistoFList->Add(fhNofFitTracksPerEvent);
}
// ------------------------------------------------------------

// -----   Private method DivideHistos  --------------------------------------------
void PndGlobalTrackFitQA::DivideHistos(TH1* hist1, TH1* hist2, TH1* hist3) {
  hist1->Sumw2();
  hist2->Sumw2();
  hist3->Divide(hist1,hist2,1,1,"B");
}
// ------------------------------------------------------------

// -----   Private method Finish   --------------------------------------------
void PndGlobalTrackFitQA::Finish() {
  cout << "-------------------- PndGlobalTrackFitQA : Summary ------------------" << endl;
  cout << " Events:        " << setw(10) << fNofEvents << endl;
  cout << " MC Tracks:     " << setw(10) << fNofMCTracks << endl;
  cout << " Reco Tracks:   " << setw(10) << fNofRecoTracks << endl;
  cout << " Fit Tracks:    " << setw(10) << fNofFitTracks << endl;
  cout << "---------------------------------------------------------------------" << endl; 

  TFile* temp = gFile;
  FairRootManager* ioman = FairRootManager::Instance();
  gFile = ioman->GetOutFile();
  gDirectory = (TDirectory*)gFile;

  gDirectory->mkdir("GlobalTrackFitQA");
  gDirectory->cd("GlobalTrackFitQA");
  TIter next(fHistoFList);
  while ( TH1* histo = ((TH1*)next()) ) histo->Write();
  gDirectory->cd("..");

  gFile = temp;
}
// ------------------------------------------------------------
 

ClassImp(PndGlobalTrackFitQA)
