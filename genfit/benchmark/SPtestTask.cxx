#include <iostream>
#include "TClonesArray.h"
#include "FairRootManager.h"
#include "SPtestTask.h"
#include "TGeant3TGeo.h"
#include "TGeant3.h"
#include "TVector2.h"
#include "TVector3.h"
#include "TCanvas.h"
#include "TRandom.h"
#include "TH1.h"
#include "TFile.h"
#include "TTree.h"
#include "TDatabasePDG.h"
#include "FairTrackParH.h"

#include "DetPlane.h"
#include "GeaneTrackRep.h"
#include "PndTpcPoint.h"

#include "StdoutKiller.h"
#include "Track.h"
#include "Kalman.h"
#include "SPhit.h"

using namespace std;

// -----   Default constructor   -------------------------------------------
SPtestTask::SPtestTask() :
  FairTask("Test") { 
  _nEv=500;
  _th=TMath::Pi()/4.;
  _posSig=0.1;
  _thSm=2.;
  _phiSm=2.;
  _mom=0.5;
  _momSm=0.1;
  _res=0.05;
}
// -------------------------------------------------------------------------



// -----   Destructor   ----------------------------------------------------
SPtestTask::~SPtestTask() { }
// -------------------------------------------------------------------------


// -----   Public method Init   --------------------------------------------
InitStatus SPtestTask::Init() {

  // Get RootManager
  FairRootManager* ioman = FairRootManager::Instance();
  if ( ! ioman ) {
    cout << "-E- SPtestTask::Init: "
	 << "RootManager not instantised!" << endl;
    return kFATAL;
  }
  
  // Get input array

  fPointArray = (TClonesArray*) ioman->GetObject("PndTpcPoint");
  // Create and register output array

  fPro = new FairGeanePro();

  return kSUCCESS;

}
// -------------------------------------------------------------------------

void SPtestTask::WriteToFile(std::string fileName){
  TFile f(fileName.c_str(),"RECREATE");

  //l1->Write("track");
  m1->Write("points");
  //m2->Write("result");
  //_plane->Write("plane");
  //uline->Write("uline");
  //vline->Write("vline");

  tree->Write();
  f.Close();
}




// -----   Public method Exec   --------------------------------------------
void SPtestTask::Exec(Option_t* opt) {
  // 	cout << "SPtestTask::Exec" << endl;
  gRandom->SetSeed(0);  

  Int_t PDGCode= -13;//mu+ tested
    
  tree = new TTree("t","SPtestTask output");
  tree->Branch("momRe",&momRe,"momRe/D");
  tree->Branch("momTr",&momTr,"momTr/D");
  tree->Branch("momSi",&momSi,"momSi/D");
  tree->Branch("momPu",&momPu,"momPu/D");
  tree->Branch("uRe",&uRe,"uRe/D");
  tree->Branch("uTr",&uTr,"uTr/D");
  tree->Branch("uSi",&uSi,"uSi/D");
  tree->Branch("uPu",&uPu,"uPu/D");

  tree->Branch("vRe",&vRe,"vRe/D");
  tree->Branch("vTr",&vTr,"vTr/D");
  tree->Branch("vSi",&vSi,"vSi/D");
  tree->Branch("vPu",&vPu,"vPu/D");
  tree->Branch("upRe",&upRe,"upRe/D");
  tree->Branch("upTr",&upTr,"upTr/D");
  tree->Branch("upSi",&upSi,"upSi/D");
  tree->Branch("upPu",&upPu,"upPu/D");
  tree->Branch("vpRe",&vpRe,"vpRe/D");
  tree->Branch("vpTr",&vpTr,"vpTr/D");
  tree->Branch("vpSi",&vpSi,"vpSi/D");
  tree->Branch("vpPu",&vpPu,"vpPu/D");
  tree->Branch("thTr",&thTr,"thTr/D");
  tree->Branch("thSt",&thSt,"thSt/D");
  tree->Branch("phiTr",&phiTr,"phiTr/D");
  tree->Branch("phiSt",&phiSt,"phiSt/D");

  for(int counter=0;counter<_nEv;++counter){


	TVector3 StartPos    = TVector3 (0.1,20.,1.);
	TVector3 StartPosChanged    = StartPos;
	StartPosChanged.SetX(gRandom->Gaus(StartPosChanged.X(),_posSig));
	StartPosChanged.SetY(gRandom->Gaus(StartPosChanged.Y(),_posSig));
	StartPosChanged.SetZ(gRandom->Gaus(StartPosChanged.Z(),_posSig));
	TVector3 StartPosErr = TVector3(1.,1.,1.);
	TVector3 StartMom    = TVector3 (1.,0.,1.);
	double phi = gRandom->Uniform()*2.*TMath::Pi();
	StartMom.SetTheta(_th);
	StartMom.SetPhi(phi);


	TVector3 StartMomChanged    = StartMom;
	double rand1 = (gRandom->Uniform()-0.5)/TMath::Pi()*2.*_thSm;
	double rand2 = (gRandom->Uniform()-0.5)/TMath::Pi()*2.*_phiSm;
	StartMomChanged.SetTheta(StartMomChanged.Theta()+rand1);
	StartMomChanged.SetPhi(StartMomChanged.Phi()+rand2);

	thTr=StartMom.Theta();
	phiTr=StartMom.Phi();
	thSt=StartMomChanged.Theta();
	phiSt=StartMomChanged.Phi();
	

	double u1 =   gRandom->Uniform();
	double u2 =   gRandom->Uniform();
	StartMom.SetMag(_mom);
	StartMomChanged.SetMag(StartMom.Mag()+(u2-0.5)*2.*_mom*_momSm);
	TVector3 StartMomErr = TVector3(0.5,0.5,0.5);

	std::cout << "counter" << counter << " " << StartMomChanged.Mag() << std::endl;	
	
	TDatabasePDG *fdbPDG= TDatabasePDG::Instance();
	TParticlePDG *fParticle= fdbPDG->GetParticle(PDGCode);
	Double_t  fCharge= fParticle->Charge();
	


	DetPlane start_pl(StartPos,StartMom);
	DetPlane start_plChanged(StartPosChanged,StartMomChanged);
	
	TVector3 errors(_res,_res,_res);
	std::vector<TVector3> points;
	
	
	AbsTrackRep* rep = new GeaneTrackRep(fPro,
										 start_plChanged,StartMomChanged,
										 StartPosErr,StartMomErr,
										 fCharge,PDGCode);
	AbsTrackRep* rephits = new GeaneTrackRep(fPro,
											 start_pl,StartMom,
											 StartPosErr,StartMomErr,
											 fCharge,PDGCode);
	
	/*
	  rep->getState().Print();
	  SPhit *h1 = new SPhit(1.,1.,1.,0.1,0.1,0.1);
	  DetPlane plane =h1->getDetPlane(rep);
	  TVector3 p1,m1;
	  rep->getPosMom(plane,p1,m1);
	  TVector3 d = plane.getO()-p1;
	  d.SetMag(1.);
	  m1.SetMag(1.);
	  printf("%10.10f\n",TMath::ACos(d*m1)/TMath::Pi() *180.);
	  
	  d.Print();
	*/
	
	std::cout << "INIT MOMENTUM: " << rep->getMom().Mag() << std::endl;
	
	
	std::cout << "TRACK MOMENTUM: " << rephits->getMom().Mag() << std::endl;
	

	const int NPOINTS=50;
	DetPlane lastPlane;
	for(int i=0;i<NPOINTS;++i){
	  std::cout << "eeeee" << std::endl;
	  
	  TVector3 pos,mom;
	  
	  {
		StdoutKiller k;
		pos = rephits->getPos();
		mom = rephits->getMom();
	  }
	  rephits->getReferencePlane().Print();
	  pos.Print();
	  mom.Print();
	  mom.SetMag(1.5);
	  DetPlane d(pos+mom,mom);
	  d.Print();
	  //rephits->getState().Print();
	  TVector3 posR,momR;
	  {
		StdoutKiller k;
		rephits->getPosMom(d,posR,momR);
	  }
	  TMatrixT<double> statePred(5,1);
	  TMatrixT<double> covPred(5,5);
	  TMatrixT<double> jac(5,5);
	  {
		StdoutKiller k; 
		rephits->extrapolate(d,statePred,covPred,jac);
	  }
	  rephits->setState(statePred);
	  rephits->setCov(covPred);
	  rephits->setReferencePlane(d);
	  statePred.Print();
	  
	  points.push_back(posR);
	  //std::cout << "############" << std::endl;
	  std::cout << "fffff" << std::endl;
	  if(i==(NPOINTS-1)){
		lastPlane = d;
	  }
	}  
	
	DetPlane targetPlane = lastPlane;
	targetPlane.setO(targetPlane.getO()+targetPlane.getNormal());
	lastPlane.Print();
	targetPlane.Print();



	std::vector<AbsRecoHit*> hits;
	
	m1 = new TPolyMarker3D(points.size(),23);
	



	for(int i=0;i<(int)points.size();++i){
	  //pointsReverse.at(i).Print();
	  AbsRecoHit *aHit = new SPhit(points.at(i),errors);
	  //aHit->getRawHitCoord().Print();
	  m1->SetPoint(i,(aHit->getRawHitCoord())[0][0],(aHit->getRawHitCoord())[1][0],(aHit->getRawHitCoord())[2][0]);
	  m1->SetMarkerColor(kBlack);
	  
	  hits.push_back(aHit);
	}
	
	
	
	
	Track *tr = new Track( rep );
	tr->addHitVector(hits);
	Kalman k;
	//std::cout << __FILE__ << __LINE__ << std::endl;
	try{
	  k.processTrack(tr);
	}
	catch(int e){
	  std::cerr << "counter some int exception caught: " << e << std::endl;
	  continue;
	}
	//std::cout << __FILE__ << __LINE__ << std::endl;
	AbsTrackRep* result =   tr->getCardinalRep();
	//result->getReferencePlane().Print();
	if(result->getStatusFlag()!=0) {
	  std::cerr << "counter result->getStatusFlag()!=0)" << std::endl;
	  continue;
	}
	printf("hitsmom %10.10f\n",StartMom.Mag());
	printf("startmomchanged %10.10f\n",StartMomChanged.Mag());
	printf("fitresult %10.10f\n",result->getMom().Mag());
	printf("relative difference: %10.10f %\n",100*(StartMom.Mag()-result->getMom().Mag())/StartMom.Mag());

	
	double invmom = (result->getState())[0][0];
	double sigmasqustate = (result->getCov())[0][0];

	bool saveTag = false;

	if(sigmasqustate>1.E-10){

	  
	  double sigma_p = 1/pow(invmom,4.) * sigmasqustate;
	  momSi=TMath::Sqrt(sigma_p);
	  momRe=result->getMom().Mag();
	  momTr=StartMom.Mag();
	  momPu=(momTr-momRe)/momSi;
	  
	  saveTag = true;
	}

	TVector2 UV_MCT,UV_FIT,UV_SIGMA;
	TVector2 UpVp_MCT,UpVp_FIT,UpVp_SIGMA;

	TMatrixT<double> statePred(5,1);
	TMatrixT<double> covPred(5,5);
	TMatrixT<double> jac(5,5);
	{
	  StdoutKiller k; 
	  rephits->extrapolate(targetPlane,statePred,covPred,jac);
	}
	UV_MCT.Set(statePred[3][0],statePred[4][0]);
	UpVp_MCT.Set(statePred[1][0],statePred[2][0]);

	{
	  StdoutKiller k; 
	  result->extrapolate(targetPlane,statePred,covPred,jac);
	}
	UV_FIT.Set(statePred[3][0],statePred[4][0]);
	UpVp_FIT.Set(statePred[1][0],statePred[2][0]);
	UV_SIGMA.Set(TMath::Sqrt(covPred[3][3]),TMath::Sqrt(covPred[4][4]));
	UpVp_SIGMA.Set(TMath::Sqrt(covPred[1][1]),TMath::Sqrt(covPred[2][2]));
	if(fabs(UV_SIGMA.X())>1.E-20 && fabs(UV_SIGMA.Y())>1.E-20
	   &&
	   fabs(UpVp_SIGMA.X())>1.E-20 && fabs(UpVp_SIGMA.Y())>1.E-20
	   ){
	  uRe=UV_FIT.X();
	  uTr=UV_MCT.X();
	  uSi=UV_SIGMA.X();
	  uPu=(uRe-uTr)/uSi;
	  vRe=UV_FIT.Y();
	  vTr=UV_MCT.Y();
	  vSi=UV_SIGMA.Y();
	  vPu=(vRe-vTr)/vSi;
	  upRe=UpVp_FIT.X();
	  upTr=UpVp_MCT.X();
	  upSi=UpVp_SIGMA.X();
	  upPu=(upRe-upTr)/upSi;
	  vpRe=UpVp_FIT.Y();
	  vpTr=UpVp_MCT.Y();
	  vpSi=UpVp_SIGMA.Y();
	  vpPu=(vpRe-vpTr)/vpSi;
	}
	else{
	  saveTag=false;
	}
	if(saveTag) tree->Fill();
	//////////////////
	
	delete rep;
	delete rephits;
  }
  
}




ClassImp(SPtestTask)
