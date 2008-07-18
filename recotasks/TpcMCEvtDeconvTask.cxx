//-----------------------------------------------------------
// File and Version Information:
// $Id$
//
// Description:
//     
//
// Environment:
//      Software developed for the PANDA Detector at FAIR.
//
// Author List:
//      Sebastian Neubert    TUM            (original author)
//
//
//-----------------------------------------------------------

// Panda Headers ----------------------

// This Class' Header ------------------
#include "TpcMCEvtDeconvTask.h"

// C/C++ Headers ----------------------
#include "TMatrixT.h"
#include "TMatrixD.h"
#include <vector>
#include <algorithm>
#include <assert.h>
#include <iostream>
#include <cmath>

// Collaborating Class Headers --------
#include "CbmRootManager.h"
#include "TClonesArray.h"
#include "GeaneTrackRep.h"
#include "CbmGeanePro.h"
#include "DetPlane.h"
#include "PndTpcMCTracklet.h"
#include "TMath.h"
#include "PndMvdHit.h"
#include "PndMvdRecoHit.h"
#include "FitterExceptions.h"

// Class Member definitions -----------

TpcMCEvtDeconvTask::TpcMCEvtDeconvTask()
  : CbmTask("TpcMCEvtDeconv"), _persistence(kFALSE),
    _vdrift(0.0027314), _dt(100), _dx(1),
    _minMVDHits(2)
{
  _trackletBranchName = "PndTpcMCTracklet";
  _trackletOutBranchName = "DeconvTrkl";
}


TpcMCEvtDeconvTask::~TpcMCEvtDeconvTask()
{
  //delete theRecoHitFactory;
}


InitStatus
TpcMCEvtDeconvTask::Init()
{


  //Get ROOT Manager
  CbmRootManager* ioman= CbmRootManager::Instance();

  if(ioman==0)
    {
      Error("TpcMCEvtDeconvTask::Init","RootManager not instantiated!");
      return kERROR;
    }
  
  // Get input collection
  _trackletArray=(TClonesArray*) ioman->GetObject(_trackletBranchName);
  
  if(_trackletArray==0)
    {
      Error("TpcMCEvtDeconvTask::Init","tracklet-array not found!");
      return kERROR;
    }

  _mvdArray=(TClonesArray*) ioman->GetObject("MVDHit");

   // create and register output array
  // persistency to be implemented
  _trackletOutArray = new TClonesArray("PndTpcMCTracklet"); 
  ioman->Register(_trackletOutBranchName,"PndTpc",
		  _trackletOutArray,_persistence);

  _geanePro=new CbmGeanePro();

   return kSUCCESS;
}


void
TpcMCEvtDeconvTask::Exec(Option_t* opt)
{
  std::cout<<"TpcMCEvtDeconvTask::Exec"<<std::endl;

  // Reset output Array
  if(_trackletOutArray==0) Fatal("TpcMCEvtDeconv::Exec)",
				 "No TrackletOutArray");
  _trackletOutArray->Delete();
  
  TVector3 poserr(0.05,0.05,0.05);
  TVector3 za1(0,0,-200);
  TVector3 za2(0,0,200);

  std::vector<PndTpcMCTracklet*> trklts;

  int ntr=_trackletArray->GetEntriesFast();
  for(int i=0;i<ntr;++i){// begin loop over tracklets
    PndTpcMCTracklet* trk=(PndTpcMCTracklet*)_trackletArray->At(i);
    trklts.push_back(trk);
    // setup a trackrepresentation
    TVector3 mom=trk->mom();
    // setup detplane orthogonal to momentum vector
    DetPlane pl(trk->pos(),mom);
    //pl.Print();

    TVector3 momerr=momerr*0.01; // 1% error
    GeaneTrackRep* rep=new GeaneTrackRep(_geanePro,
					 pl,mom,
					 poserr,momerr,
					 trk->q(),trk->pdg());
    //rep->Print();
    // extrapolate to z-axis --> get z0 --> get event time
    rep->setPropDir(-1);
    int properror=0;
    TVector3 v0(0,0,-9911);
    try{
      v0=rep->getPocaOnLine(za1,za2,true); 
    }
    catch (FitterException e){
      std::cout<<e.what();
      properror=1;
    }

    if(TMath::Abs(v0.Z())<1E-6){
      std::cout<<"z0===0!"<<std::endl;
      v0.SetZ(-9997);
    }
    // we always assume BACK propagation to the zaxis!
    //std::cout<<"PCA on Z-Axis:"<<std::endl;
    //v0.Print();
    // calculate t0 -- the event time
    double t0=v0.Z()/_vdrift;
    //std::cout<<"t0="<<t0<<std::endl;
    // if in (enlarged) timewindow -> store this tracklet
    // and if there are MvdHits to connect to
   
      int size=_trackletOutArray->GetEntriesFast();
      PndTpcMCTracklet* trkl=new((*_trackletOutArray)[size]) PndTpcMCTracklet(*trk);
      trkl->setT0(t0);
      trkl->setZPoca(v0);
      if(TMath::Abs(t0)<10*_dt){
	rep->setPropDir(-1); 
	//we always assume backpropagation when propagating into MVD!
	try{
	  trkl->setMvdHits(ConnectMVD(rep));
	}
	catch (FitterException e){
	  std::cout<<e.what();
	  properror=2;
	}
      }
      trkl->setStatus(properror);
      delete rep;
  }//end loop over tracklets

  // try to construct V0s and extrapolate to z-axis
  // sort tracklets in voxels
  std::cout<<"Starting sorting tracklets!"<<std::endl;
  std::sort(trklts.begin(),trklts.end(),PndTpcMCTrackletSort());
  std::cout<<"Finished sorting tracklets!"<<std::endl;

  int n=trklts.size();
  for(int i=0;i<n;++i){
    // try to build a V0 out of this tracklets close in space
    PndTpcMCTracklet* atrkl=trklts[i];
    for(int j=i+1;j<TMath::Min(i+20,n);++j){
      PndTpcMCTracklet* btrkl=trklts[j];
      if(atrkl->q()*btrkl->q()<0){// we have two tracks of opposite sign
	double res=(atrkl->pos()-btrkl->pos()).Mag();
	if(res<5){// not more than 5cm appart
	  TVector3 pos=0.5*(atrkl->pos()+btrkl->pos());
	  TVector3 mom=atrkl->mom()+btrkl->mom();
	  // extrapolate to z--Axis --> PCA for two lines
	  TVector3 O(0,0,0); TVector3 za(0,0,1);
	  TVector3 u=mom.Unit();
	  TVector3 poca1, poca2;
	  linepoca(pos,u,O,za,poca1,poca2);
	  double uz=u.Z();
	  double z0=pos.Z()-uz*(u*pos);
	  z0/=1-uz*uz;
	  double z02=poca2.Z();
	  assert(fabs(z0-z02)<1E-6);
	  double t0=z0/_vdrift;
	  if(TMath::Abs(t0)<10*_dt){ // V0 is inside large timewindow
	    Int_t trackid=9999;
	    Int_t evtid=atrkl->mcid().mceventID();
	    if(evtid!=btrkl->mcid().mceventID())evtid=9998;
	    int size=_trackletOutArray->GetEntriesFast();
	    PndTpcMCTracklet* trkl=
	      new((*_trackletOutArray)[size]) 
	      PndTpcMCTracklet(pos,mom,0,666,trackid,evtid);
	    trkl->setT0(t0);
	    trkl->setV0Res(res);
	    trkl->setZPoca(poca1);
	  }// endif time check
	}//endif distance check
      }// endif charge-check
    }// end loop over neighbouring tracklets
  }// end loop over tracklets



  std::cout<<"Found "<<_trackletOutArray->GetEntriesFast()
	   <<" out of "<<_trackletArray->GetEntriesFast()
	   <<" tracklets in time window for this event"
	   <<std::endl;

  return;
}

// return number of found hits fopr this track
std::vector<double>
TpcMCEvtDeconvTask::ConnectMVD(GeaneTrackRep* rep){
  std::vector<double> res;
  // extrapolate tracklet to all mvd points 
  // in the event and compute residual
  if(_mvdArray==NULL) return res;
  int n=_mvdArray->GetEntriesFast();
  for(int i=0;i<n;++i){
    PndMvdHit* hit=(PndMvdHit*)_mvdArray->At(i);
    TVector3 pos=hit->GetPosition();
    TMatrixD state(5,1);
    TMatrixD cov(5,5);
    DetPlane p;
    TVector3 d=rep->extrapolateToPoca(pos,state,cov,p);
    //pos.Print();
    //d.Print();
    double dx=(pos-d).Mag();
    res.push_back(dx);
    /*
    PndMvdRecoHit recohit(hit);
    recohit.Print();
    // do extrapolation to hit
    int repDim=rep->getDim();
    TMatrixT<double> state(repDim,1);
    TMatrixT<double> cov(repDim,repDim);;
    TMatrixT<double> jacobian(repDim,repDim);
    DetPlane pl=recohit.getDetPlane(rep);
    rep->predict(pl,state,cov,jacobian);
    recohit.setHMatrix(rep,state);
    //hit->setHMatrix(s,pred,);
    TMatrixT<double> H=recohit.getHMatrix();
    // get hit covariances
    TMatrixT<double> V=recohit.getHitCov(pl);
    TMatrixT<double> r=recohit.residualVector(rep,state);
    r.Print();
    */
  }

  return res;

}

void
TpcMCEvtDeconvTask::linepoca(const TVector3& x1, const TVector3& d1,
			     const TVector3& x2, const TVector3& d2,
			     TVector3& poca1, TVector3& poca2){
  TVector3 x=x1-x2;
  double c=d1*d2;
  double l1=x*(c*d2-d1)/(1-c*c);
  double l2=x*d2+l1*c;
  std::cout<<"l1="<<l1<<"    l2="<<l2<<std::endl;
  poca1=x1+l1*d1;
  poca2=x2+l2*d2;
  return;
}











ClassImp(TpcMCEvtDeconvTask)
