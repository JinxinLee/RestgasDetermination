#include <iostream>
#include "math.h"
#include "TClonesArray.h"
#include "FairRootManager.h"
#include "GeaneTestTask.h"
#include "TGeant3TGeo.h"
#include "TGeant3.h"
#include "TVector3.h"
#include "TCanvas.h"
#include "TRandom.h"
#include "TDatabasePDG.h"
#include "FairTrackParH.h"

#include "DetPlane.h"
#include "TestGeaneTrackRep.h"
#include "PndTpcPoint.h"
#include "FitterExceptions.h"

#include "Track.h"
#include "Kalman.h"
#include "SPhit.h"

using namespace std;


// -----   Default constructor   -------------------------------------------
GeaneTestTask::GeaneTestTask() :
  FairTask("Test") { 
}
// -------------------------------------------------------------------------



// -----   Destructor   ----------------------------------------------------
GeaneTestTask::~GeaneTestTask() { }
// -------------------------------------------------------------------------
#include"signal.h"
#include"stdlib.h"
void geaneHandler(int sig){
  std::cerr << "myhandler for sig " << sig << std::endl;
  abort();
}
// -----   Public method Init   --------------------------------------------
InitStatus GeaneTestTask::Init() {
  //handle FPE signals
  signal(8,geaneHandler);
  // Get RootManager
  FairRootManager* ioman = FairRootManager::Instance();
  if ( ! ioman ) {
    cout << "-E- GeaneTestTask::Init: "
	 << "RootManager not instantised!" << endl;
    return kFATAL;
  }
  
  fPro = new FairGeanePro();

  return kSUCCESS;

}
// -------------------------------------------------------------------------


// -----   Public method Exec   --------------------------------------------
void GeaneTestTask::Exec(Option_t* opt) {

  //all magnitudes with Changed suffix are used as seed for track fit
  TVector3 StartPos    = TVector3 (0.1,20.,1.);
  TVector3 StartPosChanged    = StartPos;
  TVector3 StartPosErr = TVector3(1.,1.,1.);

  TVector3 StartMom    = TVector3 (1.,0.,1.);
  StartMom.SetTheta(45./180.*TMath::Pi() );
  StartMom.SetPhi(40./180.*TMath::Pi() );
  StartMom.SetMag(0.6);//600MeV

  TVector3 StartMomChanged    = StartMom;
  StartMomChanged.SetTheta(0.00001/180.*TMath::Pi() );
  StartMomChanged.SetPhi(45./180.*TMath::Pi() );
  StartMomChanged.SetMag(0.7);//700MeV
  
  TVector3 StartMomErr = TVector3(0.5,0.5,0.5);
  
  TDatabasePDG *fdbPDG= TDatabasePDG::Instance();
  TParticlePDG *fParticle= fdbPDG->GetParticle(-13);
  Double_t  fCharge= fParticle->Charge();
  
  DetPlane start_pl(StartPos,StartMom);
  DetPlane start_plChanged(StartPosChanged,StartMomChanged);
  
  TVector3 errors(0.2,0.2,0.4);
  std::vector<TVector3> points;
  
  //this rep is used as seed for fit
  AbsTrackRep* rep = new TestGeaneTrackRep(fPro,
										   start_plChanged,StartMomChanged,
										   StartPosErr,StartMomErr,
										   fCharge,-13);

  //this is used for getting hit positions on track
  AbsTrackRep* rephits = new TestGeaneTrackRep(fPro,
											   start_pl,StartMom,
											   StartPosErr,StartMomErr,
											   fCharge,-13);
  
  const int NPOINTS=25;
  for(int i=0;i<NPOINTS;++i){
	TVector3 pos,mom;
	
	
	try{
	  pos = rephits->getPos();
	  mom = rephits->getMom();
	}
	catch(FitterException& e){
	  e.what();
	  std::cerr<<"Exceptions in GeaneTestTask wont be further handled _.exit(1)"<<std::endl;
	  exit(1);
	}
	
	
	rephits->getReferencePlane().Print();
	mom.SetMag(1.5);
	DetPlane d(pos+mom,mom);
	TVector3 posR,momR;
	
	try{
	  rephits->getPosMom(d,posR,momR);
	  
	}
	catch(FitterException& e){
	  e.what();
	  std::cerr<<"Exceptions in GeaneTestTask wont be further handled _.exit(1)"<<std::endl;
	  exit(1);
	}
	
	TMatrixT<double> statePred(5,1);
	TMatrixT<double> covPred(5,5);
	
	try{
	  rephits->extrapolate(d,statePred,covPred);
	}
	catch(FitterException& e){
	  e.what();
	  std::cerr<<"Exceptions in GeaneTestTask wont be further handled _.exit(1)"<<std::endl;
		exit(1);
	}
	
	rephits->setState(statePred);
	rephits->setCov(covPred);
	rephits->setReferencePlane(d);
	
	
	points.push_back(posR);
  }
  
  Track *tr = new Track( rep );
  
  for(int i=0;i<(int)points.size();++i){
	AbsRecoHit* aHit;
	aHit = new SPhit(points.at(i),errors);
	tr->addHit(aHit,3,i);//setting det ID 3
  }
  
  
  std::cerr<<"Calling Kalman Filter"<<std::endl;  
  Kalman k;

  k.processTrack(tr);
	
  
  
}




ClassImp(GeaneTestTask)
