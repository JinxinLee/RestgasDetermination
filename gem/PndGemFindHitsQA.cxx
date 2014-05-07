// -------------------------------------------------------------------------
// -----                PndGemFindHitsQA source file             -----
// -----                Created 02.06.2009 by R. Karabowicz            -----
// -------------------------------------------------------------------------

#include "PndGemFindHitsQA.h"

// FairRoot includes
#include "FairRootManager.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"
#include "FairBaseParSet.h"
#include "FairTrackParam.h"
#include "FairRootManager.h"
// Pnd includes
#include "PndGemMCPoint.h"
#include "PndGemDigiPar.h"
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
using std::map;

// -----   Default constructor   ------------------------------------------
PndGemFindHitsQA::PndGemFindHitsQA() : FairTask("GEM Find Hits QA", 1) {
  fDigiPar       = NULL;
  fMCPointArray  = NULL;
  fGemHitArray   = NULL;
  fNofEvents    = 0;

  fHistoList = NULL;

  fHistPlaneDivs = 0;

  fPointEffDist = 0.1;

  for ( Int_t istat = 0 ; istat < 4 ; istat++ ) {
    for ( Int_t isens = 0 ; isens < 2 ; isens++ ) {
      fHistWidth[istat][isens]=0.;
      for ( Int_t iregx = 0 ; iregx < 4 ; iregx++ ) {
	for ( Int_t iregy = 0 ; iregy < 4 ; iregy++ ) {
	  fhPointToHit[istat][isens][iregx][iregy] = NULL;
	}
      }
      //      fhPointClosest     [istat][isens] = NULL;
      fhPointNof         [istat][isens] = NULL;
      fhPointReco        [istat][isens] = NULL;
      fhPointRecoEff     [istat][isens] = NULL;
      fhPointRadNof      [istat][isens] = NULL;
      fhPointRadReco     [istat][isens] = NULL;
      fhPointRadRecoEff  [istat][isens] = NULL;
      fhHitNof           [istat][isens] = NULL;
      fhHitFake          [istat][isens] = NULL;
      fhHitFakeProb      [istat][isens] = NULL;
      fhHitRadNof        [istat][isens] = NULL;
      fhHitRadFake       [istat][isens] = NULL;
      fhHitRadFakeProb   [istat][isens] = NULL;
      fhHitMultipleRate  [istat][isens] = NULL;
      fhCloseHits        [istat][isens] = NULL;
    }
  }

}
// -------------------------------------------------------------------------



// -----   Standard constructor   ------------------------------------------
PndGemFindHitsQA::PndGemFindHitsQA(Int_t iVerbose) 
  : FairTask("GEM Find Hits QA", iVerbose) {
  fDigiPar       = NULL;
  fMCPointArray  = NULL;
  fGemHitArray   = NULL;
  fNofEvents    = 0;

  fHistoList = NULL;

  fHistPlaneDivs = 0;

  fPointEffDist = 0.1;

  for ( Int_t istat = 0 ; istat < 4 ; istat++ ) {
    for ( Int_t isens = 0 ; isens < 2 ; isens++ ) {
      fHistWidth[istat][isens]=0.;
      for ( Int_t iregx = 0 ; iregx < 4 ; iregx++ ) {
	for ( Int_t iregy = 0 ; iregy < 4 ; iregy++ ) {
	  fhPointToHit[istat][isens][iregx][iregy] = NULL;
	}
      }
      //      fhPointClosest     [istat][isens] = NULL;
      fhPointNof         [istat][isens] = NULL;
      fhPointReco        [istat][isens] = NULL;
      fhPointRecoEff     [istat][isens] = NULL;
      fhPointRadNof      [istat][isens] = NULL;
      fhPointRadReco     [istat][isens] = NULL;
      fhPointRadRecoEff  [istat][isens] = NULL;
      fhHitNof           [istat][isens] = NULL;
      fhHitFake          [istat][isens] = NULL;
      fhHitFakeProb      [istat][isens] = NULL;
      fhHitRadNof        [istat][isens] = NULL;
      fhHitRadFake       [istat][isens] = NULL;
      fhHitRadFakeProb   [istat][isens] = NULL;
      fhHitMultipleRate  [istat][isens] = NULL;
      fhCloseHits        [istat][isens] = NULL;
    }
  }
}
// -------------------------------------------------------------------------

// -----   Destructor   ----------------------------------------------------
PndGemFindHitsQA::~PndGemFindHitsQA() { }

// -----   Init  -----------------------------------------------------------
InitStatus PndGemFindHitsQA::Init() {
  
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
  
  // Get PndGemPoint (MCPoint) array
  fMCPointArray  = (TClonesArray*) ioman->GetObject("GEMPoint");
  if( !fMCPointArray ) {
    cout << "-E- "<< GetName() <<"::Init: No MCPoint array!"
	 << endl;
    return kERROR;
  }

  // Get Gem hit Array
  fGemHitArray  = (TClonesArray*) ioman->GetObject("GEMHit");
  if ( !fGemHitArray ) {
    cout << "-E- " << GetName() << "::Init: No PndGemHit array!" << endl;
    return kERROR;
  }

  // Get GEM digitisation parameter container
  fDigiPar = (PndGemDigiPar*)(rtdb->getContainer("PndGemDetectors"));

  cout << "-I- " << fName.Data() << "::Init(). There are " << fDigiPar->GetNStations() << " GEM stations." << endl;
  cout << "-I- " << fName.Data() << "::Init(). Initialization succesfull." << endl;

  CreateHistos();

  return kSUCCESS;
}
// -------------------------------------------------------------------------

// -----   Private method ReInit   -----------------------------------------
InitStatus PndGemFindHitsQA::ReInit() {

  return kSUCCESS;
}
// -------------------------------------------------------------------------

// -----   Private method SetParContainers   -------------------------------
void PndGemFindHitsQA::SetParContainers() {
  
  // Get run and runtime database
  FairRunAna* run = FairRunAna::Instance();
  if ( ! run ) Fatal("SetParContainers", "No analysis run");

  FairRuntimeDb* db = run->GetRuntimeDb();
  if ( ! db ) Fatal("SetParContainers", "No runtime database");

  // Get GEM digitisation parameter container
  fDigiPar = (PndGemDigiPar*)(db->getContainer("PndGemDetectors"));

  fNofEvents = 0;
}
// -------------------------------------------------------------------------


// -----   Public method Exec   --------------------------------------------
void PndGemFindHitsQA::Exec(Option_t* opt) {
  
  fNofEvents++;

  Int_t nofGemHits = fGemHitArray->GetEntries();
  Int_t nofGemPnts = fMCPointArray->GetEntries();

  PndGemHit*     gemHit = NULL;
  PndGemMCPoint* gemPnt = NULL;


  //  cout << "---------------------------------" << endl;
  //  cout << "gem " << fHistWidth[istat][isens] << " wi

  for ( Int_t ipnt = 0 ; ipnt < nofGemPnts ; ipnt++ ) {
    gemPnt = (PndGemMCPoint*)fMCPointArray->At(ipnt);
    Int_t    sensorId = gemPnt->GetSensorId(); 
    Int_t    station = fDigiPar->GetStationNr(sensorId)-1;
    Int_t    sensor  = fDigiPar->GetSensorNr (sensorId)-1;
    Double_t pntX    = (gemPnt->GetX()+gemPnt->GetXOut())/2.;
    Double_t pntY    = (gemPnt->GetY()+gemPnt->GetYOut())/2.;
    Int_t    regX    = fHistPlaneDivs+TMath::Floor(pntX/fHistWidth[station][sensor]);
    Int_t    regY    = fHistPlaneDivs+TMath::Floor(pntY/fHistWidth[station][sensor]);
    
    //    cout << "GEM Point at " << gemPnt->GetX() << " , " << gemPnt->GetY() << " , " << gemPnt->GetZ() << endl;
    //    cout << "----> Will go to [ " << station << " ] [ " << sensor << " ] [ " << regX << " ] [ " << regY << " ]" << endl;
    
    Int_t    nofCloseHits = 0;
    Double_t distToClosestHit = 100000.;
    for ( Int_t ihit = 0 ; ihit < nofGemHits ; ihit++ ) {
      gemHit = (PndGemHit*)fGemHitArray->At(ihit);
      if ( gemHit->GetStationNr()!=station+1 || gemHit->GetSensorNr()!=sensor+1 ) continue;
      
      fhPointToHit[station][sensor][regX][regY]->Fill(pntX-gemHit->GetX(),
						      pntY-gemHit->GetY());
      
      Double_t p2hDistSq = (pntX-gemHit->GetX())*(pntX-gemHit->GetX())+(pntY-gemHit->GetY())*(pntY-gemHit->GetY());
      if ( distToClosestHit > p2hDistSq )
	distToClosestHit = p2hDistSq;
      if ( p2hDistSq < fPointEffDist*fPointEffDist ) 
	nofCloseHits++;
    }
    //    fhPointClosest     [station][sensor]->Fill(pntX,pntY,distToClosestHit);
    fhCloseHits        [station][sensor]->Fill(nofCloseHits);
    fhPointNof         [station][sensor]->Fill(pntX,pntY);
    fhPointRadNof      [station][sensor]->Fill(TMath::Sqrt(pntX*pntX+pntY*pntY));
    if ( nofCloseHits > 0 ) {
      fhPointReco        [station][sensor]->Fill(pntX,pntY);
      fhPointRadReco     [station][sensor]->Fill(TMath::Sqrt(pntX*pntX+pntY*pntY));
      if ( nofCloseHits > 1 ) {
	fhHitMultipleRate  [station][sensor]->Fill(pntX,pntY);
      }
    }
  }


  for ( Int_t ihit = 0 ; ihit < nofGemHits ; ihit++ ) {
    gemHit = (PndGemHit*)fGemHitArray->At(ihit);

    Int_t    station = gemHit->GetStationNr()-1;
    Int_t    sensor  = gemHit->GetSensorNr()-1;
    Double_t hitX    = gemHit->GetX();
    Double_t hitY    = gemHit->GetY();

    Bool_t hitHasMatchingPoint = kFALSE;
    //    Double_t distToClosestPoint = 100000.;
    for ( Int_t ipnt = 0 ; ipnt < nofGemPnts ; ipnt++ ) {
      gemPnt = (PndGemMCPoint*)fMCPointArray->At(ipnt);
      Int_t    sensorId = gemPnt->GetSensorId(); 
      if ( fDigiPar->GetStationNr(sensorId)!=station+1 || fDigiPar->GetSensorNr(sensorId)!=sensor+1 ) continue;

      Double_t pntX    = (gemPnt->GetX()+gemPnt->GetXOut())/2.;
      Double_t pntY    = (gemPnt->GetY()+gemPnt->GetYOut())/2.;

      //      Double_t p2hDistSq = (hitX-gemPnt->GetX())*(hitX-gemPnt->GetX())+(hitY-gemPnt->GetY())*(hitY-gemPnt->GetY());
      //      if ( distToClosestPoint > p2hDistSq )
      //	distToClosestPoint = p2hDistSq;
      if ( TMath::Sqrt((hitX-pntX)*(hitX-pntX)+(hitY-pntY)*(hitY-pntY)) < fPointEffDist )
	hitHasMatchingPoint = kTRUE;
    }
    fhHitNof        [station][sensor]->Fill(hitX,hitY);
    fhHitRadNof     [station][sensor]->Fill(TMath::Sqrt(hitX*hitX+hitY*hitY));
    //    if ( distToClosestPoint > fPointEffDist*fPointEffDist ) 
    if ( !hitHasMatchingPoint ) {
      fhHitFake       [station][sensor]->Fill(hitX,hitY);
      fhHitRadFake    [station][sensor]->Fill(TMath::Sqrt(hitX*hitX+hitY*hitY));
    }
  }

}
// ------------------------------------------------------------

// -----   Private method CreateHistos   --------------------------------------------
void PndGemFindHitsQA::CreateHistos() {
  fHistoList = new TList();
  // number of mc tracks, reco tracks, efficiency as function of MOMENTUM

  Int_t nStations = fDigiPar->GetNStations();

  fHistPlaneDivs = 2;

  for ( Int_t istat = 0 ; istat < nStations ; istat++ ) {
    PndGemStation* station = (PndGemStation*)fDigiPar->GetStation(istat);
    Int_t nSensors = station->GetNSensors();
    for ( Int_t isens = 0 ; isens < nSensors ; isens++ ) {
      PndGemSensor* sensor = (PndGemSensor*)station->GetSensor(isens);
      Double_t sensOutRad = sensor->GetOuterRadius();
      fHistWidth[istat][isens] = TMath::Ceil(sensOutRad/fHistPlaneDivs); // make it an even number
      Double_t histBeginX = -fHistWidth[istat][isens]*fHistPlaneDivs;
      for ( Int_t iregx = 0 ; iregx < 2*fHistPlaneDivs ; iregx++ ) {
	Double_t histBeginY = -fHistWidth[istat][isens]*fHistPlaneDivs;
	for ( Int_t iregy = 0 ; iregy < 2*fHistPlaneDivs ; iregy++ ) {
	  fhPointToHit[istat][isens][iregx][iregy] = new TH2F(Form("fhPointToHit_s%d_s%d_x%d_y%d",istat,isens,iregx,iregy),
							      Form("Points vs hits, station %d, sensor %d at z=%.1fcm, %.1f<x<%.1f, %.1f<y<%.1f",
								   istat,isens,sensor->GetZ0(),
								   histBeginX,histBeginX+fHistWidth[istat][isens],
								   histBeginY,histBeginY+fHistWidth[istat][isens]),
							      2000,-1.,1.,
							      2000,-1.,1.);
	  histBeginY+=fHistWidth[istat][isens];
	  fHistoList->Add(fhPointToHit[istat][isens][iregx][iregy]);
	}
	histBeginX+=fHistWidth[istat][isens];
      }
      // fhPointClosest     [istat][isens] = new TH3F(Form("fhPointClosest_s%d_s%d",istat,isens),
      // 						   Form("Distance from point to closest hits, station %d, sensor %d",istat,isens),
      // 						   2*TMath::Ceil(sensOutRad),-TMath::Ceil(sensOutRad),TMath::Ceil(sensOutRad),
      // 						   2*TMath::Ceil(sensOutRad),-TMath::Ceil(sensOutRad),TMath::Ceil(sensOutRad),
      // 						   1000,0.,1.);
      fhPointNof         [istat][isens] = new TH2F(Form("fhPointNof_s%d_s%d",istat,isens),
						   Form("Number of points, station %d, sensor %d",istat,isens),
						   2*TMath::Ceil(sensOutRad),-TMath::Ceil(sensOutRad),TMath::Ceil(sensOutRad),
						   2*TMath::Ceil(sensOutRad),-TMath::Ceil(sensOutRad),TMath::Ceil(sensOutRad));
      fhPointReco        [istat][isens] = new TH2F(Form("fhPointReco_s%d_s%d",istat,isens),
						   Form("Number of reconstructed points (point-hit %.2f cm), station %d, sensor %d",fPointEffDist,istat,isens),
						   2*TMath::Ceil(sensOutRad),-TMath::Ceil(sensOutRad),TMath::Ceil(sensOutRad),
						   2*TMath::Ceil(sensOutRad),-TMath::Ceil(sensOutRad),TMath::Ceil(sensOutRad));
      fhPointRecoEff     [istat][isens] = new TH2F(Form("fhPointRecoEff_s%d_s%d",istat,isens),
						   Form("Hit finding efficiency (point-hit %.2f cm), station %d, sensor %d",fPointEffDist,istat,isens),
						   2*TMath::Ceil(sensOutRad),-TMath::Ceil(sensOutRad),TMath::Ceil(sensOutRad),
						   2*TMath::Ceil(sensOutRad),-TMath::Ceil(sensOutRad),TMath::Ceil(sensOutRad));
      fhPointRadNof      [istat][isens] = new TH1F(Form("fhPointRadNof_s%d_s%d",istat,isens),
						   Form("Number of points, station %d, sensor %d",istat,isens),
						   1000,0,100);
      fhPointRadReco     [istat][isens] = new TH1F(Form("fhPointRadReco_s%d_s%d",istat,isens),
						   Form("Number of reconstructed points (point-hit %.2f cm), station %d, sensor %d",fPointEffDist,istat,isens),
						   1000,0,100);
      fhPointRadRecoEff  [istat][isens] = new TH1F(Form("fhPointRadRecoEff_s%d_s%d",istat,isens),
						   Form("Hit finding efficiency (point-hit %.2f cm), station %d, sensor %d",fPointEffDist,istat,isens),
						   1000,0,100);


      fhHitNof           [istat][isens] = new TH2F(Form("fhHitNof_s%d_s%d",istat,isens),
						   Form("Number of all hits (point-hit %.2f cm), station %d, sensor %d",fPointEffDist,istat,isens),
						   2*TMath::Ceil(sensOutRad),-TMath::Ceil(sensOutRad),TMath::Ceil(sensOutRad),
						   2*TMath::Ceil(sensOutRad),-TMath::Ceil(sensOutRad),TMath::Ceil(sensOutRad));
      fhHitFake          [istat][isens] = new TH2F(Form("fhHitFake_s%d_s%d",istat,isens),
						   Form("Number of fake hits (point-hit %.2f cm), station %d, sensor %d",fPointEffDist,istat,isens),
						   2*TMath::Ceil(sensOutRad),-TMath::Ceil(sensOutRad),TMath::Ceil(sensOutRad),
						   2*TMath::Ceil(sensOutRad),-TMath::Ceil(sensOutRad),TMath::Ceil(sensOutRad));
      fhHitFakeProb      [istat][isens] = new TH2F(Form("fhHitFakeProb_s%d_s%d",istat,isens),
						   Form("Hit fake probability (point-hit %.2f cm), station %d, sensor %d",fPointEffDist,istat,isens),
						   2*TMath::Ceil(sensOutRad),-TMath::Ceil(sensOutRad),TMath::Ceil(sensOutRad),
						   2*TMath::Ceil(sensOutRad),-TMath::Ceil(sensOutRad),TMath::Ceil(sensOutRad));
      fhHitRadNof        [istat][isens] = new TH1F(Form("fhHitRadNof_s%d_s%d",istat,isens),
						   Form("Number of all hits (point-hit %.2f cm), station %d, sensor %d",fPointEffDist,istat,isens),
						   1000,0,100);
      fhHitRadFake       [istat][isens] = new TH1F(Form("fhHitRadFake_s%d_s%d",istat,isens),
						   Form("Number of fake hits (point-hit %.2f cm), station %d, sensor %d",fPointEffDist,istat,isens),
						   1000,0,100);
      fhHitRadFakeProb   [istat][isens] = new TH1F(Form("fhHitRadFakeProb_s%d_s%d",istat,isens),
						   Form("Hit fake probability (point-hit %.2f cm), station %d, sensor %d",fPointEffDist,istat,isens),
						   1000,0,100);

      fhHitMultipleRate  [istat][isens] = new TH2F(Form("fhHitMultipleRate_s%d_s%d",istat,isens),
						   Form("Distance from point to closest hits (point-hit %.2f cm), station %d, sensor %d",fPointEffDist,istat,isens),
						   2*TMath::Ceil(sensOutRad),-TMath::Ceil(sensOutRad),TMath::Ceil(sensOutRad),
						   2*TMath::Ceil(sensOutRad),-TMath::Ceil(sensOutRad),TMath::Ceil(sensOutRad));
      fhCloseHits        [istat][isens] = new TH1F(Form("fhCloseHits_s%d_s%d",istat,isens),
						   Form("Number of close hits per point (point-hit %.2f cm), station %d, sensor %d",fPointEffDist,istat,isens),
						   101,-0.5,100.5);
      //      fHistoList->Add(fhPointClosest     [istat][isens]);
      fHistoList->Add(fhPointNof         [istat][isens]);
      fHistoList->Add(fhPointReco        [istat][isens]);
      fHistoList->Add(fhPointRecoEff     [istat][isens]);
      fHistoList->Add(fhPointRadNof      [istat][isens]);
      fHistoList->Add(fhPointRadReco     [istat][isens]);
      fHistoList->Add(fhPointRadRecoEff  [istat][isens]);
      fHistoList->Add(fhHitNof           [istat][isens]);
      fHistoList->Add(fhHitFake          [istat][isens]);
      fHistoList->Add(fhHitFakeProb      [istat][isens]);
      fHistoList->Add(fhHitRadNof        [istat][isens]);
      fHistoList->Add(fhHitRadFake       [istat][isens]);
      fHistoList->Add(fhHitRadFakeProb   [istat][isens]);
      fHistoList->Add(fhHitMultipleRate  [istat][isens]);
      fHistoList->Add(fhCloseHits        [istat][isens]);
    }
  }
}
// ------------------------------------------------------------

// -----   Private method DivideHistos  --------------------------------------------
void PndGemFindHitsQA::DivideHistos(TH1* hist1, TH1* hist2, TH1* hist3) {
  hist1->Sumw2();
  hist2->Sumw2();
  hist3->Divide(hist1,hist2,1,1,"B");
}
// ------------------------------------------------------------

// -----   Private method Finish   --------------------------------------------
void PndGemFindHitsQA::Finish() {

  cout << "-------------------- PndGemFindHitsQA : Finish ------------------" << endl;
  cout << " dividing histos" << endl;

  Int_t nStations = fDigiPar->GetNStations();
  for ( Int_t istat = 0 ; istat < nStations ; istat++ ) {
    for ( Int_t isens = 0 ; isens < 2 ; isens++ ) {
      //      cout << "doing " << istat << " / " << isens << endl;
      fhPointReco[istat][isens]->Sumw2();
      fhPointNof[istat][isens]->Sumw2();
      fhPointRecoEff[istat][isens]->Divide(fhPointReco[istat][isens],fhPointNof[istat][isens],1.,1.,"B");
      fhPointRecoEff[istat][isens]->Scale(100.);

      fhPointRadReco[istat][isens]->Sumw2();
      fhPointRadNof[istat][isens]->Sumw2();
      fhPointRadRecoEff[istat][isens]->Divide(fhPointRadReco[istat][isens],fhPointRadNof[istat][isens],1.,1.,"B");
      fhPointRadRecoEff[istat][isens]->Scale(100.);

      fhHitFake          [istat][isens]->Sumw2();
      fhHitNof           [istat][isens]->Sumw2();
      fhHitFakeProb      [istat][isens]->Divide(fhHitFake        [istat][isens],fhHitNof        [istat][isens]);
      fhHitFakeProb      [istat][isens]->Scale(100.);

      fhHitRadFake          [istat][isens]->Sumw2();
      fhHitRadNof           [istat][isens]->Sumw2();
      fhHitRadFakeProb      [istat][isens]->Divide(fhHitRadFake        [istat][isens],fhHitRadNof        [istat][isens]);
      fhHitRadFakeProb      [istat][isens]->Scale(100.);

    }
  }

  cout << "-------------------- PndGemFindHitsQA : Summary ------------------" << endl;

  cout << " Events:        " << setw(10) << fNofEvents << endl;

  TFile* temp = gFile;
  FairRootManager* ioman = FairRootManager::Instance();
  gFile = ioman->GetOutFile();
  gDirectory = (TDirectory*)gFile;

  gDirectory->mkdir("GemFindHitsQA");
  gDirectory->cd("GemFindHitsQA");
  TIter next(fHistoList);
  while ( TH1* histo = ((TH1*)next()) ) histo->Write();
  gDirectory->cd("..");

  gFile = temp;

}
// ------------------------------------------------------------
 

ClassImp(PndGemFindHitsQA)
