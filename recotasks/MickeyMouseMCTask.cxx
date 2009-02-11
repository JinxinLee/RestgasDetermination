// This Class' Header ------------------
#include "MickeyMouseMCTask.h"

// C/C++ Headers ----------------------
#include <map>
#include <algorithm>
#include <string>

// Collaborating Class Headers --------
#include "FairRootManager.h"
#include "TClonesArray.h"
#include "PndMCTrack.h"
#include "PndTpcPoint.h"
#include "Track.h"

#include "AliceHelixTrackRep.h"
#include "LSLTrackRep.h"
#include "PndTpcRecoHit.h"
#include "Slt2DHit.h"
#include "SltHit.h"
#include "Kalman.h"
#include "RecoHitFactory.h"

#include "TApplication.h"
#include "TROOT.h"
#include "TSystem.h"
#include "MCHitGenerator.h"
#include "TCanvas.h"
#include "TView.h"
#include "TPolyLine3D.h"
#include "TPolyMarker3D.h"

// Class Member definitions -----------

                   
MickeyMouseMCTask::MickeyMouseMCTask()
  : FairTask("MickeyMouseMC"), _persistence(kFALSE)
{
}


MickeyMouseMCTask::~MickeyMouseMCTask()
{
}


InitStatus
MickeyMouseMCTask::Init()
{

  counter = 0;
  //Get ROOT Manager
  FairRootManager* ioman= FairRootManager::Instance();

  if(ioman==0)
    {
      Error("MickeyMouseMCTask::Init","RootManager not instantiated!");
      return kERROR;
    }
  

  // create and register output array
  // persistency to be implemented
  _trackOutputArray = new TClonesArray("Track"); 
  ioman->Register("TrackMickeyMouseMC","GenFit",_trackOutputArray,_persistence);

  return kSUCCESS;
}


void
MickeyMouseMCTask::Exec(Option_t* opt)
{
  // Reset output Array
  if(_trackOutputArray==0) Fatal("MickeyMouseMC::Exec)","No TrackOutputArray");
  _trackOutputArray->Delete();
  std::cout << "Event: " << counter++ << std::endl;
  //Track* pretrk = createTrack();
  Track* pretrk = createLSLTrack();
  Track* posttrk = new ((*_trackOutputArray)[0]) Track(*pretrk);
  Kalman k;
  k.processTrack(posttrk);

 
  /*
  // BUILD TRACKS
  std::map<int, Track*> trackMap;
  std::vector<TMatrixT<double> > mctruth;
  std::vector<Track*> trackvec;


  

  Int_t nTracks=_trackInputArray->GetEntriesFast();
  for(Int_t itrack=0;itrack<nTracks;++itrack){
    Track* inTrack=(Track*)_trackInputArray->At(itrack);



	// getting start parameters from MonteCarloTruth
	AliceHelixTrackRep* rep=new AliceHelixTrackRep();

	mctruth.push_back(rep->getStartState());
	
	Int_t size = _trackOutputArray->GetEntriesFast();
	//make copy of inTrack in map and TClonesArray


	trackMap[itrack]=new ((*_trackOutputArray)[size]) Track(*inTrack);
	
    // create a recoHit object from hit
	//    trackMap[MCid]->addHit(recoHit);
    trackMap[itrack]->
	  addHitVector(
				   theRecoHitFactory->createMany(
												 inTrack->getDetIDs(),
												 inTrack->getHitIndices()
												 ) 
				   );
	std::cout << "AAAAAAAAAAAAAAAAAA "  <<	trackMap[itrack]->getNumHits() << std::endl;
	
  }

  std::map<int, Track*>::iterator it=trackMap.begin();
  while(it!=trackMap.end()){
	trackvec.push_back(it->second);
	//    trackvec.push_back((Track*)_trackInputArray->At(0));
	//	trackvec.at(0)->addHitVector( theRecoHitFactory->createMany(
	//																trackvec.at(0)->getDetIDs(),
	//																trackvec.at(0)->getHitIndices()
	//																) );
    ++it;
  }

  std::for_each(trackMap.begin(),trackMap.end(),Kalman());


  std::cout<<_trackOutputArray->GetEntriesFast()
		   <<" Tracks created"<<std::endl;

  */
  return;


}

       
Track* MickeyMouseMCTask::createTrack() {


  TMatrixT<double> MCtruth(5,1);
  
  //phi
  MCtruth[0][0] = 5. /180. * TMath::Pi();//gRandom->Uniform(5. /180. * TMath::Pi(),350. /180. * TMath::Pi());
  //z in centimeters
  
  MCtruth[1][0] = 1.;//gRandom->Uniform(1.,2.);
  //	std::cout << MCtruth[1][0] << std::endl;
  //lambda
  MCtruth[2][0] = 20. /180. * TMath::Pi();//gRandom->Uniform(20. /180. * TMath::Pi(),70. /180. * TMath::Pi());
  //C=1/(2*R) in centimeters^-1
  MCtruth[3][0] = 0.5/150.;//gRandom->Uniform(0.5 / 150.,0.5 / 140.);
  
  //D0 in centimeters
  MCtruth[4][0] = 1.e-4;//gRandom->Uniform(1.e-3,2.e-2);
  
  
  //save the track params for later residual calculations
  
  
  TMatrixT<double> cov(5,5);
  /*
  cov[0][0] = 50.*1. /180. * TMath::Pi();
  cov[0][0] *= cov[0][0];
  cov[1][1] = 50.*5.e-1;
  cov[1][1] *= cov[1][1];
  cov[2][2] = 50.*1. /180. * TMath::Pi();
  cov[2][2] *= cov[2][2];
  cov[3][3] = 50.*0.01*MCtruth[3][0];
  cov[3][3] *= cov[3][3];
  cov[4][4] = 50.*100. /180. * TMath::Pi();
  cov[4][4] *= cov[4][4];
  */
  for(int i=0;i<5;++i)cov[i][i]=10*MCtruth[i][0]*MCtruth[i][0]*0.25; // 20% error
  cov[4][4]=1e-1;

  // offdiagonals
  //cov[0][4]=-cov[0][0];
  //cov[1][3]=-cov[1][1];
  //cov[3][1]=-cov[3][3];
  //cov[4][0]=-cov[4][4];
  
  //cov.Print();
  
  
  double phi_off    = gRandom->Uniform(-2. /180. * TMath::Pi(),2. /180. * TMath::Pi());
  double z_off      = gRandom->Uniform(-1.,1.);
  double lambda_off = gRandom->Uniform(-1. /180. * TMath::Pi(),1. /180. * TMath::Pi());
  //Kruemmungskreis-Radius
  double R_off      = gRandom->Uniform(-5.,5.);
  double D0_off     = gRandom->Uniform(-1.e-4,1.e-4);
  
  
  
  
  TMatrixT<double> startState = MCtruth;
  
  startState[0][0] += phi_off;
  startState[1][0] += z_off;
  startState[2][0] += lambda_off;
  startState[3][0]  = 0.5/(0.5/MCtruth[3][0]+R_off);
  //  startState[3][0] *= -1.;
  startState[4][0] += D0_off;
  
  
  // generate hits 
  double rSpacing= 1.; // r-spacing
  double rStartOfTrack = 5.;
  
  double dr = 1.e-11; 
  double dphi = .01/180. * TMath::Pi(); 
  double dz = 5.e-2; 
  
  AliceHelixTrackRep* rep=new AliceHelixTrackRep(rStartOfTrack,startState,cov);
  rep->getFitParams()->setMCtruth(MCtruth);
  rep->getFitParams()->setStartState(startState);
  rep->getFitParams()->setMCtruth_s(rStartOfTrack-1.e-4);
  rep->getFitParams()->setStartState_s(rStartOfTrack-1.e-4);


  Track* trk=new Track(rep);
  
  gen.initHelix(rStartOfTrack,
				MCtruth,
				cov);
  
  for(int ihit=1; ihit<50; ihit++){
	double r=(ihit*rSpacing) + rStartOfTrack;;
	PndTpcRecoHit* hit=gen.generateTPC(r,dr,dphi,dz);
	trk->addHit(hit);
  }
  return trk;
}


Track* MickeyMouseMCTask::createLSLTrack() {

    
//   double _phi = gRandom->Uniform(0.,2.*TMath::Pi());
//   double _xprim = 0.1*sin(_phi);
//   double _yprim = 0.1*cos(_phi);
  LSLTrackRep* rep=new LSLTrackRep(0,10,10,0.1,0.1,1./0.3,1.,1.,1.,1.,1.,0);
  LSLTrackRep repstart(*rep);
  
  Track* trk=new Track(rep);
  double dz=25.;
  double zStart=1.e-6;
  for(int ihit=0; ihit<24; ++ihit){
	
	//if(ihit<150)z+=dz;
	//else z-=dz;
	double z=zStart+ihit*dz;
	TMatrixT<double> pred(5,1);
	rep->extrapolate(z,pred);
	rep->setState(pred);
	rep->setS(z);
	double x=pred[0][0]; double y=pred[1][0];
	//std::cout<< "x="<<x<<" y="<<y<<std::endl;
	// smear
	x+=gRandom->Gaus(0.,0.01);
	y+=gRandom->Gaus(0.,0.01);
	//double r=sqrt(x*x+y*y);
	//double phi=TMath::ATan2(y,x);
	Slt2DHit* hit=new Slt2DHit(x,y,z,0.01,0.01);
	trk->addHit(hit);
	std::cout << z << "   " << ihit << std::endl;
  }
  // set back to starting point:
  *rep=repstart;
  rep->setS(0);
  
  // smear starting values
  
  TMatrixT<double> start=rep->getState();

  rep->getFitParams()->setMCtruth(start);
  

  start[0][0] += gRandom->Gaus(0.,0.01);
  start[1][0] += gRandom->Gaus(0.,0.01);
  start[2][0] += gRandom->Gaus(0.,0.05*start[2][0]);
  start[3][0] += gRandom->Gaus(0.,0.05*start[3][0]);
  double _Pstart = 1./start[4][0];
  _Pstart += gRandom->Gaus(0.,_Pstart*0.05);
  start[4][0] = 1./_Pstart;
  /*
  double dx=start[2][0];
  double dy=start[3][0];
  dx+=0.2*dx;
  dy+=0.2*dy;
  start[2][0]=dx;
  start[3][0]=dy;
  start[4][0]*=1.1;
  */

  rep->setState(start);
  
  
  
  rep->setStartState(rep->getState());
  //  rep->setStartState(start);
  rep->setStartCov(rep->getCov());
  rep->setStartS(rep->getS());
    

  rep->getFitParams()->setStartState(start);
  rep->getFitParams()->setMCtruth_s(rep->getS());
  rep->getFitParams()->setStartState_s(rep->getS());


  return trk;
}








ClassImp(MickeyMouseMCTask)
