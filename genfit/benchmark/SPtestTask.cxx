#include <iostream>
#include "math.h"
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


#include "Track.h"
#include "Kalman.h"
#include "SPhit.h"
#include "StripHit.h"

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

void SPtestTask::setFileName(std::string _fileName){
  fileName = _fileName;
}



// -----   Public method Exec   --------------------------------------------
void SPtestTask::Exec(Option_t* opt) {

  TFile *file = TFile::Open(fileName.c_str(),"RECREATE");

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
  tree->Branch("chi2",&chi2,"chi2/D");

  for(int counter=0;counter<_nEv;++counter){

    std::cerr << "@@@@@@@@@@@@@@@@ Doing event #" << counter << std::endl;
    std::cout << "@@@@@@@@@@@@@@@@ Doing event #" << counter << std::endl;

	TVector3 StartPos    = TVector3 (0.01,20.,10.);
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
	
	TVector3 errors(_res,_res,2.*_res);
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
	

	const int NPOINTS=25;
	DetPlane lastPlane;
	for(int i=0;i<NPOINTS;++i){
	  std::cout << "eeeee" << std::endl;
	  
	  TVector3 pos,mom;
	  
	  {
		//StdoutKiller k;
		pos = rephits->getPos();
		mom = rephits->getMom();
	  }
	  //	  rephits->getReferencePlane().Print();
	  //pos.Print();
	  //mom.Print();
	  mom.SetMag(1.5);
	  DetPlane d(pos+mom,mom);
	  //d.Print();
	  //rephits->getState().Print();
	  TVector3 posR,momR;
	  {
		//StdoutKiller k;
		rephits->getPosMom(d,posR,momR);
	  }
	  TMatrixT<double> statePred(5,1);
	  TMatrixT<double> covPred(5,5);
	  {
		//StdoutKiller k; 
		rephits->extrapolate(d,statePred,covPred);
	  }
	  rephits->setState(statePred);
	  rephits->setCov(covPred);
	  rephits->setReferencePlane(d);
	  //statePred.Print();
	  
	  points.push_back(posR);
	  //std::cout << "############" << std::endl;
	  std::cout << "fffff" << std::endl;
	  if(i==(NPOINTS-1)){
		lastPlane = d;
	  }
	}  
	
	DetPlane targetPlane = lastPlane;
	targetPlane.setO(targetPlane.getO()+targetPlane.getNormal());
	//lastPlane.Print();

	std::cout << std::endl << std::endl << std::endl<<"targetPlane" << std::endl;
	targetPlane.Print();



	//	std::vector<AbsRecoHit*> hits;
	
	Track *tr = new Track( rep );

	for(int i=0;i<(int)points.size();++i){
	  AbsRecoHit* aHit;
	  if(i>=points.size()-12){
	    aHit = new StripHit(points.at(i),i,_res);
	  }
	  else{
	    //pointsReverse.at(i).Print();
	    aHit = new SPhit(points.at(i),errors);
	  }
	  tr->addHit(aHit,3,i);
	}
	
	

	//tr->addHitVector(hits);
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
	std::cout << std::endl << std::endl << std::endl<<"resultPlane" << std::endl;
	result->getReferencePlane().Print();
	if(result->getStatusFlag()!=0) {
	  std::cerr << "counter result->getStatusFlag()!=0)" << std::endl;
	  delete rephits;
	  delete tr;
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

	//TAG
	//DetPlane targetPlane(result->getReferencePlane());

	{
	  //StdoutKiller k; 
	  rephits->extrapolate(targetPlane,statePred,covPred);
	}
	UV_MCT.Set(statePred[3][0],statePred[4][0]);
	UpVp_MCT.Set(statePred[1][0],statePred[2][0]);

	//TAG
	//statePred=result->getState();
	//covPred=result->getCov();
	{
	  //StdoutKiller k; 
	  result->extrapolate(targetPlane,statePred,covPred);
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
	  chi2=result->getRedChiSqu();
	  std::cerr<<"NDF " << result->getNDF() << std::endl;
	}
	else{
	  saveTag=false;
	}
	if(saveTag) tree->Fill();
	//////////////////
	

	std::cerr << "%%%%%%%%%%%%% " << tr->getNumHits() << " " << tr->getFailedHits(3) << std::endl;


	delete rephits;
	delete tr;
  }

  tree->Write();
  file->Close();
  
}




ClassImp(SPtestTask)
