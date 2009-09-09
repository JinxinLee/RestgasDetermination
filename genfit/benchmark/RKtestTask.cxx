#include <iostream>
#include "math.h"
#include "RKtestTask.h"
#include "TVector3.h"
#include "TRandom.h"
#include "TH1.h"
#include "TFile.h"
#include "TTree.h"
#include "TApplication.h"
#include "TSystem.h"
#include "TCanvas.h"
#include "TPolyMarker3D.h"

#include "FairRootManager.h"
#include "DetPlane.h"
#include "RKtrackRep.h"
#include "FitterExceptions.h"

#include "Track.h"
#include "Kalman.h"
#include "SPhit.h"
#include "StripHit.h"




// -----   Default constructor   -------------------------------------------
RKtestTask::RKtestTask() :
  FairTask("Test"),field(NULL) { 
}
// -------------------------------------------------------------------------



// -----   Destructor   ----------------------------------------------------
RKtestTask::~RKtestTask() { }
// -------------------------------------------------------------------------
//#include"signal.h"
//#include"stdlib.h"
//void myhandler(int sig){
//  std::cerr << "myhandler for sig " << sig << std::endl;
//  abort();
//}
// -----   Public method Init   --------------------------------------------
InitStatus RKtestTask::Init() {
  //signal(8,myhandler);

  //signal(11,myhandler);
  // Get RootManager
  FairRootManager* ioman = FairRootManager::Instance();
  if ( ! ioman ) {
    std::cout << "-E- RKtestTask::Init: "
	      << "RootManager not instantised!" << std::endl;
    return kFATAL;
  }
  
  // Get input array

  return kSUCCESS;
}
// -------------------------------------------------------------------------



// -----   Public method Exec   --------------------------------------------
void RKtestTask::Exec(Option_t* opt) {
  assert(field!=NULL);

  // 	cout << "RKtestTask::Exec" << endl;
  gRandom->SetSeed(4);  

  int _nEv = 1;
  for(int counter=0;counter<_nEv;++counter){

    std::cerr << "@@@@@@@@@@@@@@@@ Doing event #" << counter << std::endl;
    
    TVector3 pos    = TVector3 (0.,0.1,0.);
    TVector3 posErr = TVector3(1.,1.,1.);
    TVector3 mom    = TVector3 (0.,0.,1.);
    TVector3 momErr    = TVector3 (.3,0.3,.3);
    
    AbsTrackRep* rephits = new RKtrackRep(pos,mom,posErr,momErr,1.,13,field);

    
    
	

    const int NPOINTS=25;
    DetPlane tarPlane;
    tarPlane.setU(1.,0.,0.);
    tarPlane.setV(0.,1.,0.);
    tarPlane.setO(0.,0.,1.);
    TPolyMarker3D *points = new TPolyMarker3D(NPOINTS,20);
    for(int i=0;i<NPOINTS;++i){
      //std::cout << "eeeee" << std::endl;
      
      tarPlane.setO(0.,0.,tarPlane.getO().Z()+10.);
      TMatrixT<double> statePred(5,1);
      TMatrixT<double> covPred(5,5);
      {
	//StdoutKiller k; 
	try{
	  rephits->extrapolate(tarPlane,statePred,covPred);
	}
	catch(FitterException& e){
	  e.what();
	  std::cerr<<"Exceptions in RKtestTask wont be further handled _.exit(1)"<<std::endl;
	  exit(1);
	}
      }

      rephits->setState(statePred);
      rephits->setCov(covPred);
      rephits->setReferencePlane(tarPlane);
      //points.push_back(rephits->getPos());
      TVector3 pos = rephits->getPos();
      points->SetPoint(i,pos.X(),pos.Y(),pos.Z());
      std::cout<<"#"<<i<<std::endl;
      rephits->getPos().Print();
      rephits->getMom().Print();
      statePred.Print();
    }  
    TCanvas *c1 = new TCanvas("c1");
    points->Draw();
    gApplication->SetReturnFromRun(kTRUE);
    gSystem->Run();
    
  } 

}




ClassImp(RKtestTask)
