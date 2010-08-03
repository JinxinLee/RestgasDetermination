//-----------------------------------------------------------
// File and Version Information:
// $Id$
//
// Description:
//      Implementation of class PndTpcRiemannTrackingTask
//      see PndTpcRiemannTrackingTask.hh for details
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
#include "PndTpcRiemannTrackingTask.h"

// C/C++ Headers ----------------------


// Collaborating Class Headers --------
#include "FairRootManager.h"
#include "TClonesArray.h"
#include "GFRecoHitFactory.h"
#include "GFException.h"
#include "PndTpcCluster.h"
#include "PndTpcRiemannTrackFinder.h"
#include "PndTpcRiemannTrack.h"
#include "PndTpcRiemannHough.h"
#include "PndTpcRiemannHit.h"
#include "PndTpcRiemannHTCorrelator.h"
#include "PndTpcProximityHTCorrelator.h"
#include "GFTrackCand.h"
#include "GFTrack.h"
#include "LSLTrackRep.h"
#include "GeaneTrackRep.h"
#include "TH1I.h"
#include "TH1D.h"
#include "McIdCollection.h"
#include "TVector3.h"
#include "FairMCPoint.h"
#include "GFDetPlane.h"
#include "TDatabasePDG.h"
//#include "AbsBFieldIfc.h"
//#include "FairFieldAdaptor.h"

#include <cmath>

using std::fabs;

// Class Member definitions -----------

ClassImp(PndTpcRiemannTrackingTask)

PndTpcRiemannTrackingTask::PndTpcRiemannTrackingTask()
  : FairTask("PndTpc Pattern Reco"), _persistence(kFALSE)
{
  // default values for Riemann TrackFinder
  _proxcut=2;
  _riproxcut=0.01;
  _planecut=1E-4;
  _minpoints=4;
  _szcut=2.0;

  _clusterBranchName = "PndTpcCluster";
}

PndTpcRiemannTrackingTask::~PndTpcRiemannTrackingTask()
{
  if(_multiplicityHisto!=NULL)delete _multiplicityHisto;
  if(_trackPurityH!=NULL)delete _trackPurityH;
  if(_trackSizeH!=NULL)delete _trackSizeH;
}

void 
PndTpcRiemannTrackingTask::SetTrkFinderParameters(double proxcut,
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
PndTpcRiemannTrackingTask::Init()
{
//Get ROOT Manager
  FairRootManager* ioman= FairRootManager::Instance();

  if(ioman==0)
    {
      Error("PndTpcRiemannTrackingTask::Init","RootManager not instantiated!");
      return kERROR;
    }

  // Get input collection
  _clusterArray=(TClonesArray*) ioman->GetObject(_clusterBranchName);

  if(_clusterArray==0)
    {
      Error("PndTpcRiemannTrackingTask::Init","Cluster-array not found!");
      return kERROR;
    }
// Get input collection
  _mvdArray=(TClonesArray*) ioman->GetObject("MVDPoint");

  if(_mvdArray==0)
    {
      Error("PndTpcRiemannTrackingTask::Init","mvd-array not found!");
    }



  // create and register output array
  _trackArray = new TClonesArray("GFTrack");
  ioman->Register("TrackPreFit","GenFit",_trackArray,_persistence);

  _riemannTrackArray = new TClonesArray("PndTpcRiemannTrack");
  ioman->Register("RiemannTrack","Tpc",_riemannTrackArray,_persistence);

  _riemannHitArray = new TClonesArray("PndTpcRiemannHit");
  ioman->Register("RiemannHit","Tpc",_riemannHitArray,_persistence);
    


  //if(_field==NULL){
  //  Error("DemoRiemannTrackingTask::Init","BField not found!");
  //  return kERROR;
  // 
  //_fieldIfc=new FairFieldAdaptor(_field);

  _trackfinder= new PndTpcRiemannTrackFinder();
  _trackfinder->setMinHitsForFit(_minpoints);
  _trackfinder->addCorrelator(new PndTpcRiemannHTCorrelator(_planecut,_riproxcut,_szcut));
  _trackfinder->addCorrelator(new PndTpcProximityHTCorrelator(_proxcut));

    
  //_trackfinder->configure(_xcut,_ycut,_zcut, // proximity cuts
  //			  _chi2cut,     // chi2cut
  //			  _minpoints,    // minpoints for fit
  //			  true,          // do merge
  //			  0.1,0.1, 3.);    // merge cuts (fractional)

  
  // init histos
  _multiplicityHisto=new TH1I("multipl","# track candidates",20,0,20);
  _trackSizeH=new TH1I("trksize","# hits in track",100,0,100);
  _trackPurityH=new TH1D("trkpurity","trackPurity",25,0,1.01);
  _trackMcIdsH=new TH1D("trkmcids","# mcids in track",25,0,25);
  
  // GeanePro will get Geometry and BField from the Run
  _geanePro = new FairGeanePro();

  return kSUCCESS;
}

void
PndTpcRiemannTrackingTask::Exec(Option_t* opt)
{
  std::cout<<"PndTpcRiemannTrackingTask::Exec"<<std::endl;
  // Reset output Arrays
  if(_trackArray==0) Fatal("PndTpcSimpleRiemannTracking::Exec)","No TrackArray");
   _trackArray->Delete();
   
if(_riemannTrackArray==0) Fatal("PndTpcSimpleRiemannTracking::Exec)","No RiemannTrackArray");
   _riemannTrackArray->Delete();
if(_riemannHitArray==0) Fatal("PndTpcSimpleRiemannTracking::Exec)","No RiemannHitArray");
   _riemannHitArray->Delete();
  
  std::vector<PndTpcCluster*> clusterlist;
  
  unsigned int ncl=_clusterArray->GetEntriesFast();
  for(unsigned int icl=0; icl<ncl; ++icl){
    clusterlist.push_back((PndTpcCluster*)_clusterArray->At(icl));
  }

  std::cout<<"RiemannTask ncl="<<clusterlist.size()<<std::endl;

  //if(_mvdArray!=NULL){
  //  unsigned int nmvd=_mvdArray->GetEntriesFast();
  //  for(unsigned int imvd=0; imvd<nmvd; ++imvd){
  //    FairMCPoint* mvd=(FairMCPoint*)_mvdArray->At(imvd);
  //    TVector3 pos;
  //    mvd->Position(pos);
  //    clusterlist.push_back(new PndTpcCluster(pos,1,imvd));
  //  }
  //}

  std::vector<PndTpcRiemannTrack*> riemannlist;
  PndTpcRiemannHough hough;
  hough.buildTracks(clusterlist,riemannlist);
  
  return;

  //_trackfinder->buildTracks(clusterlist,riemannlist);

  // build trackcands
  std::vector<GFTrackCand*> candlist;
  unsigned int nr=riemannlist.size();
  for(unsigned int ir=0;ir<nr;++ir){
    // store pattern reco information
    PndTpcRiemannTrack* trk=riemannlist[ir];
    new((*_riemannTrackArray)[_riemannTrackArray->GetEntriesFast()]) PndTpcRiemannTrack(*trk);
    unsigned int nhits=trk->getNumHits();
    for(unsigned int ih=0;ih<nhits;++ih){
      PndTpcRiemannHit* hit=trk->getHit(ih);
      new ((*_riemannHitArray)[_riemannHitArray->GetEntriesFast()]) PndTpcRiemannHit(*hit);
    }
    

    // build tracks
    if(nhits<_minpoints)continue;
    trk->szFit();
    GFTrackCand* cand=new GFTrackCand();
    // reverse order!
    std::cout<<"nhits="<<nhits<<std::endl;


    // at this point hits should be sorted by decreasing z
    // look at radius to decide how to go on
    double r1=trk->getHit(0)->cluster()->pos().Perp(); // biggest z
    double r2=trk->getHit(nhits-1)->cluster()->pos().Perp(); // smallest z

    // decide how to sort
    // this will probably go wrong for some secondaries
    for(unsigned int ih=nhits-1;ih>0;--ih){
      cand->addHit(2,trk->getHit(ih)->cluster()->index());
    }
    cand->addHit(2,trk->getHit(0)->cluster()->index());   
    if(r1<r2)cand->setInverted();
   
    cand->setCurv(0.01/fabs(trk->r())*trk->sign());
    cand->setDip(trk->dip());

    candlist.push_back(cand);
  }


  std::cout<<"PndTpcRiemannTrackingTask::Exec:: "
	   <<candlist.size()<<" track candidates found."<<std::endl;
  _multiplicityHisto->Fill(candlist.size());

  

  
  // -----------------------------------------------
  // build tracks
  unsigned int ncand=candlist.size();
  for(unsigned int ic=0; ic<ncand; ++ic){
    GFTrackCand* cand=candlist[ic];
    if(cand->getNHits()<6){
      std::cout<<"Track initialization went wrong not enough hits in track"<<std::endl;
      continue;
    }

    // check Monte Carlo Truth
    McIdCollection mcid;
    mcid.ClearData();
    for(unsigned int i=0;i<cand->getNHits();++i){
      unsigned int detId;
      unsigned int hitId;
      cand->getHit(i,detId,hitId);
      mcid.AddIDCollection(clusterlist[hitId]->mcId());
    }
    _trackPurityH->Fill(mcid.MaxRelWeight());
    _trackMcIdsH->Fill(mcid.nIDs());
    
    _trackSizeH->Fill(cand->getNHits());

    // Todo: Use R from pattern reco to initialize track rep!
    // create track object

    // calcualte start values
    unsigned int detID;
    unsigned int hitID;
    cand->getHit(0,detID,hitID);
    PndTpcCluster* cl1=(PndTpcCluster*)_clusterArray->At(hitID);
    TVector3 pos1=cl1->pos();
    TVector3 pos2;
    TVector3 delta;
    bool ok=false;
    unsigned int index=1;
    while(!ok && index<cand->getNHits()){
    cand->getHit(index,detID,hitID);
      ++index;
      PndTpcCluster* cl2=(PndTpcCluster*)_clusterArray->At(hitID);
      pos2=cl2->pos();
      delta=pos2-pos1;
      if(fabs(delta.Z())>0.1 && delta.X()!=0 && delta.Y()!=0)ok=true;
    }
    // if(!ok){
//       std::cout<<"Track initialization went wrong dz<1mm"<<std::endl;
//       trk->getTrackRep(0)->setStatusFlag(2);
//       continue;
//    }
    double mx=delta.X()/delta.Z();
    double my=delta.Y()/delta.Z();
    if(fabs(mx)<1E-12)mx<0 ? mx=-1E-12 : mx=+1E-12;
    if(fabs(my)<1E-12)my<0 ? my=-1E-12 : my=+1E-12;
    
    std::cout<<"mx="<<mx<<"  my="<<my<<std::endl;
    TMatrixT<double> state(5,1);
    state[0][0]=pos1.X();
    state[1][0]=pos1.Y();
    state[2][0]=mx;
    state[3][0]=my;
    // p=0.3BR/dip -- assuming 2T BField R in meters -> convert to cm!
    double one_o_p=cand->getCurv()*fabs(cand->getDip())*166.67; 
    state[4][0]=one_o_p; 

    GFAbsTrackRep* rep=0;
    if(_geane) {
      GFDetPlane pl(pos1, pos1.Orthogonal(), pos1.Cross(pos1.Orthogonal()));
      TVector3 poserr(2,2,2);
      TVector3 mom = delta*(1/one_o_p);
      TVector3 momerr = 0.5*mom;
      int pdg = 211; //pions hardcoded atm
      double q=TDatabasePDG::Instance()->GetParticle(pdg)->Charge()/3.;
      GeaneTrackRep* grep = new GeaneTrackRep(_geanePro, pl, mom, poserr, momerr,q,pdg);
      grep->setPropDir(1);
      rep=grep; }
    else {	
      LSLTrackRep* lrep=new LSLTrackRep();
    
      lrep->setInverted(cand->inverted());
      //lrep->SetBField(_fieldIfc);
      GFTrack* trk=new((*_trackArray)[_trackArray->GetEntriesFast()]) GFTrack(lrep);
      trk->setCandidate(*cand); // here the candidate is copied!
      //Is this what we want?
      
      std::cout<<"Setting initial p="<<1/state[4][0]<<std::endl;
      TMatrixT<double> cov(5,5);
      cov[0][0]=100;
      cov[1][1]=100;
      cov[2][2]=16;
      cov[3][3]=16;
      cov[4][4]=5;
      GFDetPlane pl(pos1+TVector3(0,0,-10E-4),TVector3(1,0,0),TVector3(0,1,0));
      trk->getTrackRep(0)->setData(state, pl, &cov);
      //      trk->getTrackRep(0)->setStartS(pos1.Z()-10E-4);
      rep=lrep;
    }
  }// end loop over tracks
  
  std::cout<<_trackArray->GetEntriesFast()<<" tracks created"<<std::endl;
  
  return;
}


void
PndTpcRiemannTrackingTask::WriteHistograms(const TString& filename) {
  TFile* file=FairRootManager::Instance()->GetOutFile();
  file->mkdir("RiemannTracking");
  file->cd("RiemannTracking");

  _multiplicityHisto->Write();
  delete _multiplicityHisto;
  _multiplicityHisto=NULL;

  _trackSizeH->Write();
  delete _trackSizeH;
  _trackSizeH=NULL;

  _trackPurityH->Write();
  delete _trackPurityH;
  _trackPurityH=NULL;

  _trackMcIdsH->Write();
  delete _trackMcIdsH;
  _trackMcIdsH=NULL;
}
