
// Includes from base
#include "FairRootManager.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"

// Includes from ROOT
#include "TClonesArray.h"
#include "TObjArray.h"
#include "TMath.h"
#include "TGeoManager.h"
#include "TGeoNode.h"
#include "TApplication.h"
#include "TSystem.h"
#include "TCanvas.h"
#include "TPolyMarker3D.h"
#include "RKtrackingTask.h"


#include "PndGemMCPoint.h"
#include "PndMvdMCPoint.h"
#include "PndDchPoint.h"

#include "AbsRecoHit.h"
#include "PixHit.h"
#include "RKtrackRep.h"
#include "Track.h"
#include "Kalman.h"
#include "FitterExceptions.h"

#include"assert.h"
#include<algorithm>

// -----   Default constructor   -------------------------------------------
RKtrackingTask::RKtrackingTask() :
  FairTask("RK tracking"),field(NULL) { 
}

// -----   Constructor   ---------------------------------------------------
RKtrackingTask::RKtrackingTask(const char* name, Int_t iVerbose) 
  : FairTask(name, iVerbose),field(NULL) { 
}


// -----   Destructor   ----------------------------------------------------
RKtrackingTask::~RKtrackingTask() {
}

// -----   Private method SetParContainers   -------------------------------
void RKtrackingTask::SetParContainers() {

  // Get run and runtime database
  FairRunAna* run = FairRunAna::Instance();
  if ( ! run ) Fatal("SetParContainers", "No analysis run");

  FairRuntimeDb* db = run->GetRuntimeDb();
  if ( ! db ) Fatal("SetParContainers", "No runtime database");

}
// -------------------------------------------------------------------------


// -----   Public method Init   --------------------------------------------
InitStatus RKtrackingTask::Init() {
  gRandom->SetSeed(4);

  // Get RootManager
  FairRootManager* ioman = FairRootManager::Instance();
  if( !ioman ) {
    std::cout << "-E- RKtrackingTask::Init: "
	      << "RootManager not instantiated!" << std::endl;
    return kFATAL;
  }
  
  // Get input array
  fMvdPointArray = (TClonesArray*) ioman->GetObject("MVDPoint");
  if( !fMvdPointArray ) {
    std::cout << "-W- RKtrackingTask::Init: "
	      << "Array of MVDPoints not found!" << std::endl;
    return kERROR;
  }
  fGemPointArray = (TClonesArray*) ioman->GetObject("GEMPoint");
  if( !fGemPointArray ) {
    std::cout << "-W- RKtrackingTask::Init: "
	      << "Array of GEMPoints not found!" << std::endl;
    return kERROR;
  }
  fDchPointArray = (TClonesArray*) ioman->GetObject("PndDchPoint");
  if( !fDchPointArray ) {
    std::cout << "-W- RKtrackingTask::Init: "
	      << "Array of PndDchPoints not found!" << std::endl;
    return kERROR;
  }
  
  file = new TFile("dreggn.root","RECREATE");
  tree = new TTree("t","RKtrackingTask output");
  tree->Branch("momRe",&momRe,"momRe/D");
  tree->Branch("momTr",&momTr,"momTr/D");
  tree->Branch("momSi",&momSi,"momSi/D");
  tree->Branch("momPu",&momPu,"momPu/D");
  tree->Branch("xRe",&xRe,"xRe/D");
  tree->Branch("xTr",&xTr,"xTr/D");
  tree->Branch("xSi",&xSi,"xSi/D");
  tree->Branch("xPu",&xPu,"xPu/D");

  tree->Branch("yRe",&yRe,"yRe/D");
  tree->Branch("yTr",&yTr,"yTr/D");
  tree->Branch("ySi",&ySi,"ySi/D");
  tree->Branch("yPu",&yPu,"yPu/D");
  tree->Branch("xpRe",&xpRe,"xpRe/D");
  tree->Branch("xpTr",&xpTr,"xpTr/D");
  tree->Branch("xpSi",&xpSi,"xpSi/D");
  tree->Branch("xpPu",&xpPu,"xpPu/D");
  tree->Branch("ypRe",&ypRe,"ypRe/D");
  tree->Branch("ypTr",&ypTr,"ypTr/D");
  tree->Branch("ypSi",&ypSi,"ypSi/D");
  tree->Branch("ypPu",&ypPu,"ypPu/D");
  tree->Branch("chi2",&chi2,"chi2/D");
  
  std::cout << "-I- RKtrackingTask: Intialization successfull" << std::endl;
  return kSUCCESS;

}

bool vecSort(const TVector3& v1,const TVector3& v2){
  return v1.Z() < v2.Z();
}

// -----   Public method Exec   --------------------------------------------
void RKtrackingTask::Exec(Option_t* opt) {

  static int counter(0);
  std::cout << "RKtrackingTask::Exec for event " << counter++ << std::endl;

  assert(field!=NULL);
  std::vector<TVector3> points;

  bool foundStartValues(false);
  TVector3 startPos;
  TVector3 startMom;

  for ( Int_t iPoint = 0 ; iPoint < fMvdPointArray->GetEntriesFast() ; iPoint++ ) {

    FairMCPoint* point = (PndMvdMCPoint*)fMvdPointArray->At(iPoint);
    if(!foundStartValues){
      foundStartValues = true;
      point->Position(startPos);
      point->Momentum(startMom);
    }
    TVector3 pos(point->GetX(),point->GetY(),point->GetZ());
    points.push_back(pos);
  }  // end of loop over Points
  for ( Int_t iPoint = 0 ; iPoint < fGemPointArray->GetEntriesFast() ; iPoint++ ) {
    FairMCPoint* point = (PndGemMCPoint*)fGemPointArray->At(iPoint);
    if(!foundStartValues){
      foundStartValues = true;
      point->Position(startPos);
      point->Momentum(startMom);
    }
    TVector3 pos(point->GetX(),point->GetY(),point->GetZ());
    points.push_back(pos);
  }  // end of loop over Points
  for ( Int_t iPoint = 0 ; iPoint < fDchPointArray->GetEntriesFast() ; iPoint++ ) {
    FairMCPoint* point = (PndDchPoint*)fDchPointArray->At(iPoint);
    if(!foundStartValues){
      foundStartValues = true;
      point->Position(startPos);
      point->Momentum(startMom);
    }
    TVector3 pos(point->GetX(),point->GetY(),point->GetZ());
    points.push_back(pos);
  }  // end of loop over Points


  sort(points.begin(),points.end(),vecSort);

  //filter out points which are closer than 0.1cm otgether in Z
  std::vector<TVector3> pointsFilt;
  double lastZ=-1.E100;
  for(int i=0;i<points.size();++i){
    assert(points.at(i).Z()-lastZ >= 0.);//check if sorted
    if(points.at(i).Z()-lastZ > 0.1){
      pointsFilt.push_back(points.at(i));
    }
    lastZ = points.at(i).Z();
  }

  //make PolyMarker3D out of filtered points for VIS
  //and RecoHits for fitting
  std::vector<AbsRecoHit*> recoHits;
  TPolyMarker3D *drawpoints = new TPolyMarker3D(pointsFilt.size(),20);
  static const double RESOLUTION = 0.1;// in cm
  for(int i=0;i<pointsFilt.size();++i){
    drawpoints->SetPoint(i,pointsFilt.at(i).X(),
			 pointsFilt.at(i).Y(),
			 pointsFilt.at(i).Z());
    recoHits.push_back( new PixHit(pointsFilt.at(i),RESOLUTION));
  }

  

  TVector3 posErr(1.,1.,1.);
  TVector3 momErr(1.,1.,1.);
  TVector3 startPosMod = startPos;
  TVector3 startMomMod = startMom;
  TVector3 momUnit = startMom;
  momUnit.SetMag(1.);
  //startPosMod.SetX(startPosMod.X()-momUnit.X());
  //startPosMod.SetY(startPosMod.Y()-momUnit.Y());
  //startPosMod.SetZ(startPosMod.Z()-momUnit.Z());
  startPosMod.SetX(gRandom->Gaus(startPosMod.X(),0.9));
  startPosMod.SetY(gRandom->Gaus(startPosMod.Y(),0.9));
  startPosMod.SetZ(gRandom->Gaus(startPosMod.Z(),0.9));
  startMomMod.SetX(gRandom->Gaus(startMomMod.X(),0.3));
  startMomMod.SetY(gRandom->Gaus(startMomMod.Y(),0.3));
  startMomMod.SetZ(gRandom->Gaus(startMomMod.Z(),0.3));
  
  AbsTrackRep* rep = new RKtrackRep(startPosMod,startMomMod,posErr,momErr,-1.,2212,field);
  TMatrixT<double> startState = rep->getState();
  Track t(rep);

  for(int i=0;i<recoHits.size();++i){
    t.addHit(recoHits.at(i),3,i);
  }
  t.addHitVector(recoHits);


  Kalman k;
  try{
    k.processTrack(&t);
  }
  catch(FitterException& e){
    std::cout << e.what() << std::endl;
    throw e;
  }

  TMatrixT<double> finalState = rep->getState();

  if(rep->getStatusFlag()==0){
    double invmom = (rep->getState())[4][0];
    double sigmasqustate = (rep->getCov())[4][4];
    double sigma_p = 1/pow(invmom,4.) * sigmasqustate;
    momSi=TMath::Sqrt(sigma_p);
    momRe=rep->getMom().Mag();
    momTr=startMom.Mag();
    momPu=(momRe-momTr)/momSi;

    //make sure we are comparing at the same z position
    if(fabs(rep->getPos().Z()-startPos.Z())>1.e-3) return;
    xRe = rep->getPos().X();
    xTr = startPos.X();
    xSi = TMath::Sqrt((rep->getCov())[0][0]);
    xPu = (xRe-xTr)/xSi;
    yRe = rep->getPos().Y();
    yTr = startPos.Y();
    ySi = TMath::Sqrt((rep->getCov())[1][1]);
    yPu = (yRe-yTr)/ySi;
    xpRe = (rep->getState())[2][0];
    xpTr = startMom.X()/startMom.Z();
    xpSi = TMath::Sqrt((rep->getCov())[2][2]);
    xpPu = (xRe-xTr)/xSi;
    ypRe = (rep->getState())[3][0];
    ypTr = startMom.Y()/startMom.Z();
    ypSi = TMath::Sqrt((rep->getCov())[3][3]);
    ypPu = (yRe-yTr)/ySi;
    chi2 = rep->getRedChiSqu();
    std::cout << "fill" << std::endl;
    tree->Fill();
  }

  //startPos.Print();
  //startMom.Print();
  //startPosMod.Print();
  //startMomMod.Print();
  //rep->getPos().Print();
  //rep->getMom().Print();
  //startState.Print();
  //finalState.Print();
  //TCanvas *c1 = new TCanvas("c1");
  //drawpoints->Draw();
  //gApplication->SetReturnFromRun(kTRUE);
  //gSystem->Run();


}

// -----   Private method Finish   -----------------------------------------
void RKtrackingTask::Finish() {
}

ClassImp(RKtrackingTask)
