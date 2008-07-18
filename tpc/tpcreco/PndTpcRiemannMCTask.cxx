//-----------------------------------------------------------
// File and Version Information:
// $Id$
//
// Description:
//      Implementation of class PndTpcRiemannMCTask
//      see PndTpcRiemannMCTask.hh for details
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
#include "PndTpcRiemannMCTask.h"

// C/C++ Headers ----------------------
#include <iostream>
#include <algorithm>

// Collaborating Class Headers --------
#include "CbmRootManager.h"
#include "CbmMCTrack.h"
#include "TClonesArray.h"
#include "PndTpcCluster.h"
#include "PndTpcClusterZ.h"

#include "PndTpcRiemannTrack.h"
#include "PndTpcRiemannHit.h"
#include "PndTpcRiemannMCResidual.h"
#include "TH1I.h"
#include "TH1D.h"
#include "TH2D.h"
#include "McIdCollection.h"
#include "McId.h"
#include "TVector3.h"
#include "TTree.h"
#include "TFile.h"
#include "TVector2.h"
#include "TVector3.h"
#include "TDatabasePDG.h"
//#include "AbsBFieldIfc.h"
//#include "CbmFieldAdaptor.h"

#include <cmath>
#include <fenv.h>

using std::fabs;

// Class Member definitions -----------

ClassImp(PndTpcRiemannMCTask)

PndTpcRiemannMCTask::PndTpcRiemannMCTask()
  : CbmTask("PndTpc Pattern Reco"), _persistence(kFALSE), _bkgTree(NULL)
{
  // default values for Riemann TrackFinder
  _proxcut=2;
  _riproxcut=0.01;
  _planecut=1E-4;
  _minpoints=4;
  _szcut=2.0;

  _clusterBranchName = "PndTpcCluster";
  _mctrackBranchName = "MCTrack";
  _bkgFileName= "";
  _bkgBranchName= "MCTrack";
}

PndTpcRiemannMCTask::~PndTpcRiemannMCTask()
{
  if(_distH!=NULL)delete _distH;
  if(_distTrueH!=NULL)delete _distTrueH;
  if(_distTrueRH!=NULL)delete _distTrueRH;
}

void 
PndTpcRiemannMCTask::SetTrkFinderParameters(double proxcut,
					       double riproxcut,
					       double planecut,
					       double szcut,
					       unsigned int minpointsforfit)
{
  _proxcut=proxcut;
  _riproxcut=riproxcut;
  _planecut=planecut;
  _minpoints=minpointsforfit;
  _szcut=szcut;
}


InitStatus
PndTpcRiemannMCTask::Init()
{

  //feenableexcept(FE_INVALID|FE_DIVBYZERO|FE_OVERFLOW);

//Get ROOT Manager
  CbmRootManager* ioman= CbmRootManager::Instance();

  if(ioman==0)
    {
      Error("PndTpcRiemannMCTask::Init","RootManager not instantiated!");
      return kERROR;
    }

  
  //ioman->GetInTree()->AddFriend("cbmsim1 = cbmsim","/afs/e18/data/panda/MC/FAIRRoot/DPM5/test1.mc.root");
  //ioman->GetInTree()->Print();

  // Get input collection
  _clusterArray=(TClonesArray*) ioman->GetObject(_clusterBranchName);

  if(_clusterArray==0)
    {
      Error("PndTpcRiemannMCTask::Init","Cluster-array not found!");
      return kERROR;
    }

 
  _mctrackArray=(TClonesArray*) ioman->GetObject(_mctrackBranchName);
  if(_mctrackArray==0)
    {
      Error("PndTpcRiemannMCTask::Init","MCTrack-array not found!");
      return kERROR;
    }

  _riemannTrackArray = new TClonesArray("PndTpcRiemannTrack");
  ioman->Register("RiemannTrack","Tpc",_riemannTrackArray,_persistence);

  _riemannHitArray = new TClonesArray("PndTpcRiemannHit");
  ioman->Register("RiemannHit","Tpc",_riemannHitArray,_persistence);
   
  _riemannResArray = new TClonesArray("PndTpcRiemannMCResidual");
   ioman->Register("RiemannRes","Tpc",_riemannResArray,_persistence);

  
  
  // access background file if set
  if(!_bkgFileName.IsNull()){
    TFile* bkgfile=TFile::Open(_bkgFileName,"READ");
    _bkgTree=(TTree*)bkgfile->Get("cbmsim");
    if(_bkgTree==NULL)
    {
      Error("Init","cbmsim tree not found in bkgfile");
      return kERROR;
    }
    else std::cout<<"PndTpcRiemannMCTask::Init: Using background file "
		  <<_bkgFileName<<std::endl;

    // Create bkgArray
    _bkgArray = new TClonesArray("CbmMCTrack");
    _bkgTree->SetBranchAddress(_bkgBranchName,&_bkgArray);

  }
  else {
    _bkgTree=NULL;
    _bkgArray=NULL;
  }


  // init histos
  _distH=new TH1D("riemanndist","distance to riemann plane",200,-0.1,0.1);
  _distTrueH=new TH1D("truedist","distance to true riemann plane",200,-0.1,0.1);
  _distTrueRH=new TH2D("truedistR","true dist vs R",200,-0.1,0.1,50,0,200);
  _distTrueIDH=new TH2D("truedistID","true dist vs ID",200,-0.1,0.1,20,0,20);

  _sdistH=new TH1D("sdist","z distanz at s",1000,-200,200); 
  _sdistTrueH=new TH1D("trueSdist","true z distance at s",1000,-200,200);
  _sdistTrueZH=new TH2D("trueSdistZ","true z distance vs z",100,-40,40,100,0,100);

  _2dDist=new TH2D("DistRS","riemann vs sz dist",200,-0.1,0.1,200,-200,200);
  _2dTrueDist=new TH2D("TrueDistRS","riemann vs sz dist truth",200,-0.1,0.1,200,-200,200);

  return kSUCCESS;
}

void
PndTpcRiemannMCTask::Exec(Option_t* opt)
{
  std::cout<<"PndTpcRiemannMCTask::Exec"<<std::endl;
  
  // clear output array
  _riemannHitArray->Delete();
  _riemannTrackArray->Delete();
  _riemannResArray->Delete();

  std::vector<PndTpcCluster*> clusterlist;
  McIdCollection mastercoll;
  
  unsigned int ncl=_clusterArray->GetEntriesFast();
  for(unsigned int icl=0; icl<ncl; ++icl){
    PndTpcCluster* cl=(PndTpcCluster*)_clusterArray->At(icl);
    clusterlist.push_back(cl);
    // build mc-id list
    mastercoll.AddID(cl->mcId().DominantID());
  }

  std::sort(clusterlist.begin(),clusterlist.end(),PndTpcClusterZ());


  unsigned int n=mastercoll.nIDs();
  std::cout<<n<<" MCIDs found in event"<<std::endl;

  //std::vector<PndTpcRiemannTrack*> riemannlist;
  // loop over mcids and build tracks
  for(unsigned int i=0; i<n; ++i){
    McId id=mastercoll.ID(i);
    unsigned int evtid=id.mceventID();
    unsigned int trkid=id.mctrackID();
    CbmMCTrack* mctrk=NULL;
    if(evtid==0){ // get mctrack from this event
          mctrk=(CbmMCTrack*)_mctrackArray->At(trkid);
        }
    else if(_bkgTree!=NULL){// this means background!
      _bkgTree->GetEntry(evtid-1);
      mctrk=(CbmMCTrack*)_bkgArray->At(trkid);
    }
    
    if(mctrk!=NULL && mctrk->GetMomentum().Mag()>0.2){
      int size=_riemannTrackArray->GetEntriesFast();
      PndTpcRiemannTrack* rtrk=new((*_riemannTrackArray)[size]) PndTpcRiemannTrack(*McToRiemann(mctrk));
    }

  }// end loop over mcids

  unsigned int nrt=_riemannTrackArray->GetEntriesFast();
  std::cout<<nrt<<" RiemannMCTracks created"<<std::endl;
  
  // fill true hits
  //std::vector<PndTpcRiemannHit*> hitlist;

  for(unsigned int icl=0; icl<ncl; ++icl){ // loop over clusters
    PndTpcCluster* cl=clusterlist[icl];
    PndTpcRiemannHit* rhit=new ((*_riemannHitArray)[icl])PndTpcRiemannHit(cl);
    if(cl->maxMcWeight()<0.9)continue;
    // loop over riemanntracks and fill in true hits
    for(unsigned int irt=0;irt<nrt;++irt){
      PndTpcRiemannTrack* rtrk=(PndTpcRiemannTrack*)_riemannTrackArray->At(irt);
      // check MCID
      McId id=mastercoll.ID(irt);
      if(id==cl->mcId().DominantID()){
	rtrk->addHit(rhit);
      }
     }
  }//end loop over clusters
  std::vector<McIdCollection> mcidstrack(nrt);
  // loop again over tracks to do sz fit and build mcid
  for(unsigned int irt=0;irt<nrt;++irt){
      PndTpcRiemannTrack* rtrk=(PndTpcRiemannTrack*)_riemannTrackArray->At(irt);
      int nhits=rtrk->getNumHits();
      if(nhits<6)continue;
      double rold=rtrk->r();
      rtrk->refit();
      //std::cout<<"First Fit ... R/Rold="<<rtrk->r()/rold<<std::endl;
      rtrk->szFit();
      for(int h=0;h<nhits;++h){
	PndTpcRiemannHit* hit=rtrk->getHit(h);
	//std::cout<<hit->cluster()->mcId().DominantID()<<std::endl;
	mcidstrack[irt].AddIDCollection(hit->cluster()->mcId());
	double d=rtrk->dist(hit);
	double s=rtrk->szDist(hit);
	//if(hit->cluster()->mcId().DominantID().mceventID()==0){
	  _distTrueH->Fill(d);
	  _sdistTrueH->Fill(s);
	  _sdistTrueZH->Fill(s,irt);
	  _2dTrueDist->Fill(d,s);
	  //}
      }
      //std::cout<<mcidstrack[irt]<<std::endl;
      //rtrk->Plot();
  }

  std::cout<<"Calculating Residuals..."<<std::endl;
  unsigned int nh=_riemannHitArray->GetEntriesFast();
 _riemannResArray->Expand(nh*nrt);
  // calculate residuals
  for(unsigned int ih=0; ih<nh; ++ih){ // loop over hits
    if(ih%1000==0)std::cout<<"Hit#"<<ih<<std::endl;
    PndTpcRiemannHit* rhit=(PndTpcRiemannHit*)_riemannHitArray->At(ih);
    // loop over riemantracks
    for(unsigned int irt=0;irt<nrt;++irt){
      PndTpcRiemannTrack* rtrk=(PndTpcRiemannTrack*)_riemannTrackArray->At(irt);
      if(rtrk->getNumHits()<6)continue;
      double d=rtrk->dist(rhit);
      double s=rtrk->szDist(rhit,true); // calculate s!
      double k=999;
      rtrk->getClosestHit(rhit,k);
      int size=_riemannResArray->GetEntriesFast();
      PndTpcRiemannMCResidual* res=new ((*_riemannResArray)[size]) PndTpcRiemannMCResidual(rhit,rtrk,mcidstrack[irt]);
      res->SetRiemD(d);
      res->SetSZD(s);
      res->SetCloseD(k);

      _distH->Fill(d);
      // check MCID
      McId id=mastercoll.ID(irt);
      if(id==rhit->cluster()->mcId().DominantID()){

	_distTrueRH->Fill(d,rtrk->r()*100);
	_distTrueIDH->Fill(d,id.mctrackID());
      }
      _sdistH->Fill(s);
      _2dDist->Fill(d,s);
     }
  }//end loop over clusters
 
  std::cout<<"...done"<<std::endl;

  return;
}


void
PndTpcRiemannMCTask::WriteHistograms() {
  TFile* file=CbmRootManager::Instance()->GetOutFile();
  file->mkdir("RiemannMC");
  file->cd("RiemannMC");

  _distH->Write();
  delete _distH;
  _distH=NULL;
  
 _sdistH->Write();
  delete _sdistH;
  _sdistH=NULL;

   _sdistTrueH->Write();
  delete _sdistTrueH;
  _sdistTrueH=NULL;

  _sdistTrueZH->Write();
  delete _sdistTrueZH;
  _sdistTrueZH=NULL;

  _2dDist->Write();
  delete _2dDist;
  _2dDist=NULL;

 _2dTrueDist->Write();
  delete _2dTrueDist;
  _2dTrueDist=NULL;

  _distTrueH->Write();
  delete _distTrueH;
  _distTrueH=NULL;

  _distTrueRH->Write();
  delete _distTrueRH;
  _distTrueRH=NULL;

  _distTrueIDH->Write();
  delete _distTrueIDH;
  _distTrueIDH=NULL;
  
  
}

PndTpcRiemannTrack*
PndTpcRiemannMCTask::McToRiemann(CbmMCTrack* mctrk){
  double R,dip,z0;
  TVector3 p=mctrk->GetMomentum();

  dip=p.Z()/p.Perp();
  z0=0;

  int pdg=mctrk->GetPdgCode();
  TParticlePDG* par=TDatabasePDG::Instance()->GetParticle(pdg);
  double q;
  if(par!=NULL)q=par->Charge()/3.;
  else {
    std::cout<<"Invalid PDG-code "<<pdg<<std::endl;
    q=1;
  }
  double sign=q/fabs(q);

  double pt=p.Perp();
  R=pt/(2*0.3)*100; // 100cm/m
  TVector3 x=mctrk->GetStartVertex();  

  TVector2 start(x.X(),x.Y());
  TVector2 dir(p.X(),p.Y());
  TVector2 radial=dir.Unit().Rotate(TMath::Pi()/2)*R;
  radial*=-sign;
  TVector2 origin=start+radial;

  PndTpcRiemannTrack* rtrk=new PndTpcRiemannTrack();
  rtrk->init(origin.X(),origin.Y(),R,dip,z0);
  return rtrk;
}
