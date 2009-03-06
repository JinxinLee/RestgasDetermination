#include <iostream>
#include "TClonesArray.h"
#include "FairRootManager.h"
#include "IllustrateTask.h"
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
IllustrateTask::IllustrateTask() :
  FairTask("Test") { 
}
// -------------------------------------------------------------------------



// -----   Destructor   ----------------------------------------------------
IllustrateTask::~IllustrateTask() { }
// -------------------------------------------------------------------------


// -----   Public method Init   --------------------------------------------
InitStatus IllustrateTask::Init() {

  // Get RootManager
  FairRootManager* ioman = FairRootManager::Instance();
  if ( ! ioman ) {
    cout << "-E- IllustrateTask::Init: "
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

void getPoints(GeaneTrackRep GeaneRep,double dist,int nSteps,std::vector<TVector3>* infront,std::vector<TVector3>* behind=NULL,DetPlane* targetPlane=NULL){

  AbsTrackRep *rep = &GeaneRep;

  infront->clear();
  if(behind!=NULL)behind->clear();

  TMatrixT<double> statePred(5,1);
  TMatrixT<double> covPred(5,5);
  TMatrixT<double> jac(5,5);

  std::vector<TVector3>* points=infront;


  bool switched = false;

  for(int i=0;i<nSteps;++i){
	TVector3 pos,mom;
	
	{
	  //StdoutKiller k;
	  pos = rep->getPos();
	  mom = rep->getMom();
	}

	mom.SetMag(dist);
	DetPlane d(pos+mom,mom);

	if(targetPlane!=NULL){
	  if(!switched){
		if(targetPlane->distance(pos)<2.*dist){
		  d=*targetPlane;
		  points=behind;
		  switched=true;
		}
	  }	  
	}

	TVector3 posR,momR;
	{
	  //StdoutKiller k;
	  rep->getPosMom(d,posR,momR);
	}
	TMatrixT<double> statePred(5,1);
	TMatrixT<double> covPred(5,5);
	TMatrixT<double> jac(5,5);
	{
	  //StdoutKiller k; 
	  rep->extrapolate(d,statePred,covPred,jac);
	}
	rep->setState(statePred);
	rep->setCov(covPred);
	rep->setReferencePlane(d);
	
	points->push_back(posR);
  }  


}


void IllustrateTask::getLine(TPolyLine3D **infront,TPolyLine3D **behind,GeaneTrackRep GeaneRep,double dist,int nSteps,DetPlane* targetPlane){

  std::vector<TVector3> points;
  std::vector<TVector3> pointsBehind;
  getPoints(GeaneRep,dist,nSteps,&points,&pointsBehind,targetPlane);


  TPolyLine3D  *returnL = new TPolyLine3D(points.size());
  for(int i=0;i<points.size();++i){
	returnL->SetPoint(i,points.at(i).X(),points.at(i).Y(),points.at(i).Z());
  }
  *infront=returnL;

  TPolyLine3D  *returnLBehind = new TPolyLine3D(pointsBehind.size());
  for(int i=0;i<pointsBehind.size();++i){
	returnLBehind->SetPoint(i,pointsBehind.at(i).X(),pointsBehind.at(i).Y(),pointsBehind.at(i).Z());
  }
  *behind=returnLBehind;
};


// -----   Public method Exec   --------------------------------------------
void IllustrateTask::Exec(Option_t* opt) {


  // 	cout << "IllustrateTask::Exec" << endl;

  Int_t PDGCode= -13;//mu+ tested
    
  TFile *outfile=TFile::Open("out.root","RECREATE");

  
  TVector3 StartPos    = TVector3 (0.1,20.,1.);
  TVector3 StartPosChanged    = StartPos;
  StartPosChanged.SetX(StartPosChanged.X()-0.3);
  StartPosChanged.SetY(StartPosChanged.Y()+0.3);
  StartPosChanged.SetZ(StartPosChanged.Z()-0.8);
  TVector3 StartPosErr = TVector3(1.,1.,1.);

  TVector3 StartMom    = TVector3 (1.,0.,1.);

  StartMom.SetTheta(85./180. * TMath::Pi());
  StartMom.SetPhi(.5*TMath::Pi());
  
  
  TVector3 StartMomChanged    = StartMom;
  StartMomChanged.SetTheta(StartMomChanged.Theta()+1./180.*TMath::Pi());
  StartMomChanged.SetPhi(StartMomChanged.Phi()-1./180.*TMath::Pi());
  
  StartMom.SetMag(0.5);
  StartMomChanged.SetMag(StartMom.Mag()-0.07);
  TVector3 StartMomErr = TVector3(0.5,0.5,0.5);
  
  TDatabasePDG *fdbPDG= TDatabasePDG::Instance();
  TParticlePDG *fParticle= fdbPDG->GetParticle(PDGCode);
  Double_t  fCharge= fParticle->Charge();
  
  
  
  DetPlane start_pl(StartPos,StartMom);
  DetPlane start_plChanged(StartPosChanged,StartMomChanged);
  
  TVector3 errors(0.07,0.07,0.07);
  std::vector<TVector3> points;
  std::vector<TVector3> pointsTr;
  
  
  AbsTrackRep* rep = new GeaneTrackRep(fPro,
									   start_plChanged,StartMomChanged,
									   StartPosErr,StartMomErr,
									   fCharge,PDGCode);
  GeaneTrackRep* rep2 = new GeaneTrackRep(fPro,
									   start_plChanged,StartMomChanged,
									   StartPosErr,StartMomErr,
									   fCharge,PDGCode);
  AbsTrackRep* rephits = new GeaneTrackRep(fPro,
										   start_pl,StartMom,
										   StartPosErr,StartMomErr,
										   fCharge,PDGCode);
  GeaneTrackRep* rephitsTr = new GeaneTrackRep(fPro,
										   start_pl,StartMom,
										   StartPosErr,StartMomErr,
										   fCharge,PDGCode);

  std::cout << "INIT MOMENTUM: " << rep->getMom().Mag() << std::endl;
    
  std::cout << "TRACK MOMENTUM: " << rephits->getMom().Mag() << std::endl;
  
  
  const int NPOINTS=30;
  DetPlane lastPlane;
  for(int i=0;i<NPOINTS;++i){
	std::cout << "eeeee" << std::endl;
	
	TVector3 pos,mom;
	
	{
	  //StdoutKiller k;
	  pos = rephits->getPos();
		mom = rephits->getMom();
	}
	rephits->getReferencePlane().Print();
	pos.Print();
	mom.Print();
	mom.SetMag(2.5);
	DetPlane d(pos+mom,mom);
	d.Print();
	//rephits->getState().Print();
	TVector3 posR,momR;
	{
	  //StdoutKiller k;
	  rephits->getPosMom(d,posR,momR);
	}
	TMatrixT<double> statePred(5,1);
	TMatrixT<double> covPred(5,5);
	TMatrixT<double> jac(5,5);
	{
	  //StdoutKiller k; 
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

  getLine(&Ltr,&LtrBehind,*(dynamic_cast<GeaneTrackRep*>(rephits)),1.,100,&targetPlane);
  Ltr->Write("Ltr");
  LtrBehind->Write("LtrBehind");

  getLine(&Lst,&LstBehind,*(dynamic_cast<GeaneTrackRep*>(rep)),1.,100,&targetPlane);
  Lst->Write("Lst");
  LstBehind->Write("LstBehind");

  targetPlane.getGraphics(1.5,5.,&Mpl,&Lpl,&LplU,&LplV,&LplN);
  Mpl->Write("Mpl");
  Lpl->Write("Lpl");
  LplU->Write("LplU");
  LplV->Write("LplV");
  LplN->Write("LplN");

  
  std::vector<AbsRecoHit*> hits;
  
  Msm = new TPolyMarker3D(points.size(),23);
  
  for(int i=0;i<(int)points.size();++i){
	//pointsReverse.at(i).Print();
	AbsRecoHit *aHit = new SPhit(points.at(i),errors);
	//aHit->getRawHitCoord().Print();
	Msm->SetPoint(i,(aHit->getRawHitCoord())[0][0],(aHit->getRawHitCoord())[1][0],(aHit->getRawHitCoord())[2][0]);
	Msm->SetMarkerColor(kBlack);
	
	hits.push_back(aHit);
  }


  /////////////////////////////////////////
  
  Mtr = new TPolyMarker3D(points.size(),24);
  
  for(int i=0;i<(int)points.size();++i){
	Mtr->SetPoint(i,points.at(i).X(),points.at(i).Y(),points.at(i).Z());
	Mtr->SetMarkerColor(kBlack);
  }
  //////////////////////////////////  
  
  Msm->Write("Msm");
  Mtr->Write("Mtr");
  
  Track *tr = new Track( rep );
  tr->addHitVector(hits);
  Kalman k;
  //std::cout << __FILE__ << __LINE__ << std::endl;
  try{
	k.processTrack(tr);
  }
  catch(int e){
	std::cerr << "counter some int exception caught: " << e << std::endl;
	throw;
  }
  //std::cout << __FILE__ << __LINE__ << std::endl;
  AbsTrackRep* result =   tr->getCardinalRep();
  //result->getReferencePlane().Print();
  if(result->getStatusFlag()!=0) {
	std::cerr << "counter result->getStatusFlag()!=0)" << std::endl;
	throw;
  }
  getLine(&Lfit,&LfitBehind,*(dynamic_cast<GeaneTrackRep*>(result)),.5,200,&targetPlane);
  Lfit->Write("Lfit");
  LfitBehind->Write("LfitBehind");


}






ClassImp(IllustrateTask)
