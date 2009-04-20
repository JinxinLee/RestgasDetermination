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
  double _th=40/180.*TMath::Pi();
  double _phi=40/180.*TMath::Pi();
  double _mom=1.;

  gRandom->SetSeed(4);
  for(int i=0;i<100;++i){
    std::cerr << "@@@@@@@@@@@@@@@@ Doing event #" << i << std::endl;
	
	TVector3 StartPos    = TVector3 (0.1,20.,1.);
	TVector3 StartPosChanged    = StartPos;
	StartPosChanged.SetX(gRandom->Gaus(StartPosChanged.X(),0.1));
	StartPosChanged.SetY(gRandom->Gaus(StartPosChanged.Y(),0.1));
	StartPosChanged.SetZ(gRandom->Gaus(StartPosChanged.Z(),0.1));
	TVector3 StartPosErr = TVector3(1.,1.,1.);
	TVector3 StartMom    = TVector3 (1.,0.,1.);
	double phi = gRandom->Uniform()*2.*TMath::Pi();
	StartMom.SetTheta(_th);
	StartMom.SetPhi(_phi);


	TVector3 StartMomChanged    = StartMom;
	double rand1 = (gRandom->Uniform()-0.5)/180.*TMath::Pi()*2.*5.;
	double rand2 = (gRandom->Uniform()-0.5)/180.*TMath::Pi()*2.*5.;
	StartMomChanged.SetTheta(gRandom->Uniform(0.,0.01)/180.*TMath::Pi());//StartMomChanged.Theta()+rand1);
	StartMomChanged.SetPhi(StartMomChanged.Phi()+rand2);


	double u1 =   gRandom->Uniform();
	double u2 =   gRandom->Uniform();
	StartMom.SetMag(_mom);
	StartMomChanged.SetMag(StartMom.Mag()+(u2-0.5)*2.*_mom*0.1);
	TVector3 StartMomErr = TVector3(0.5,0.5,0.5);

	
	TDatabasePDG *fdbPDG= TDatabasePDG::Instance();
	TParticlePDG *fParticle= fdbPDG->GetParticle(-13);
	Double_t  fCharge= fParticle->Charge();
	
	DetPlane start_pl(StartPos,StartMom);
	DetPlane start_plChanged(StartPosChanged,StartMomChanged);
	
	TVector3 errors(0.01,0.01,0.02);
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
	
	delete tr;
	delete rephits;
  
  }

}


ClassImp(GeaneTestTask)
