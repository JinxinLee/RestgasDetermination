//-----------------------------------------------------------
// File and Version Information:
// $Id$
//
// Description:
//      Implementation of class PndTpcPatternRecoTask
//      see PndTpcPatternRecoTask.hh for details
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
#include "PndTpcPatternRecoTask.h"

// C/C++ Headers ----------------------


// Collaborating Class Headers --------
#include "FairRootManager.h"
#include "TClonesArray.h"
#include "RecoHitFactory.h"
#include "FitterExceptions.h"
#include "PndTpcConfTrackFinder.h"
#include "TrackCand.h"
#include "Track.h"
#include "LSLTrackRep.h"
#include "TH1I.h"
#include "TH1D.h"
#include "McIdCollection.h"
//#include "AbsBFieldIfc.h"
//#include "FairFieldAdaptor.h"

#include <cmath>

using std::fabs;

// Class Member definitions -----------

ClassImp(PndTpcPatternRecoTask)

PndTpcPatternRecoTask::PndTpcPatternRecoTask()
  : FairTask("PndTpc Pattern Reco"), _persistence(kFALSE)
{
  // default values for Conformal Map TrackFinder
  _xcut=1.;
  _ycut=1.;
  _zcut=2.; 
  _chi2cut=10;     
  _minpoints=4;

  _clusterBranchName = "PndTpcCluster";
}

PndTpcPatternRecoTask::~PndTpcPatternRecoTask()
{
  if(_multiplicityHisto!=NULL)delete _multiplicityHisto;
  if(_trackPurityH!=NULL)delete _trackPurityH;
  if(_trackSizeH!=NULL)delete _trackSizeH;
}

void 
PndTpcPatternRecoTask::SetTrkFinderParameters(double xcut, double ycut, 
					   double zcut,
					   double chi2cut,
					   unsigned int minpointsforfit)
{
  _xcut=xcut;
  _ycut=ycut;
  _zcut=zcut; 
  _chi2cut=chi2cut;     
  _minpoints=minpointsforfit;
}


InitStatus
PndTpcPatternRecoTask::Init()
{
//Get ROOT Manager
  FairRootManager* ioman= FairRootManager::Instance();

  if(ioman==0)
    {
      Error("PndTpcPatternRecoTask::Init","RootManager not instantiated!");
      return kERROR;
    }

  // Get input collection
  _clusterArray=(TClonesArray*) ioman->GetObject(_clusterBranchName);

  if(_clusterArray==0)
    {
      Error("PndTpcPatternRecoTask::Init","Cluster-array not found!");
      return kERROR;
    }

  // create and register output array
  _trackArray = new TClonesArray("Track");
  ioman->Register("TrackPreFit","GenFit",_trackArray,_persistence);

  
  //if(_field==NULL){
  //  Error("DemoPatternRecoTask::Init","BField not found!");
  //  return kERROR;
  // 
  //_fieldIfc=new FairFieldAdaptor(_field);

  _trackfinder= new PndTpcConfTrackFinder();

    
  _trackfinder->configure(_xcut,_ycut,_zcut, // proximity cuts
			  _chi2cut,     // chi2cut
			  _minpoints,    // minpoints for fit
			  true,          // do merge
			  0.1,0.1, 3.);    // merge cuts (fractional)

  
  // init histos
  _multiplicityHisto=new TH1I("multipl","# track candidates",20,0,20);
  _trackSizeH=new TH1I("trksize","# hits in track",100,0,100);
  _trackPurityH=new TH1D("trkpurity","trackPurity",25,0,1);

  return kSUCCESS;
}

void
PndTpcPatternRecoTask::Exec(Option_t* opt)
{
  std::cout<<"PndTpcPatternRecoTask::Exec"<<std::endl;
  // Reset output Array
  if(_trackArray==0) Fatal("PndTpcSimplePatternReco::Exec)","No TrackArray");
   _trackArray->Delete();
  
  std::vector<PndTpcCluster*> clusterlist;
  
  unsigned int ncl=_clusterArray->GetEntriesFast();
  for(unsigned int icl=0; icl<ncl; ++icl){
    clusterlist.push_back((PndTpcCluster*)_clusterArray->At(icl));
  }

  std::vector<TrackCand*> candlist;
  _trackfinder->buildTracks(clusterlist,candlist);

  std::cout<<"PndTpcPatternRecoTask::Exec:: "
	   <<candlist.size()<<" track candidates found."<<std::endl;
  _multiplicityHisto->Fill(candlist.size());


  
  // -----------------------------------------------
  // build tracks
  unsigned int ncand=candlist.size();
  for(unsigned int ic=0; ic<ncand; ++ic){
    TrackCand* cand=candlist[ic];
    if(cand->getNHits()<6){
      std::cout<<"Track initialization went wrong not enough hits in track"<<std::endl;
      continue;
    }

    // check Monte Carlo Truth
    McIdCollection mcid;
    for(unsigned int i=0;i<cand->getNHits();++i){
      unsigned int detId;
      unsigned int hitId;
      cand->getHit(i,detId,hitId);
      mcid.AddIDCollection(clusterlist[hitId]->mcId());
    }
    _trackPurityH->Fill(mcid.MaxRelWeight());
    
    _trackSizeH->Fill(cand->getNHits());

    // Todo: Use R from pattern reco to initialize track rep!
    // create track object
    LSLTrackRep* rep=new LSLTrackRep();
    //rep->SetBField(_fieldIfc);
    Track* trk=new((*_trackArray)[_trackArray->GetEntriesFast()]) Track(rep);
    trk->setCandidate(*cand); // here the candidate is copied!
    //Is this what we want?

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
      if(fabs(delta.Z())>0.1)ok=true;
    }
    if(!ok){
      std::cout<<"Track initialization went wrong dz<1mm"<<std::endl;
      trk->getTrackRep(0)->setStatusFlag(2);
      continue;
    }
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
    state[4][0]=0.006/cand->getCurv()/cand->getDip(); 
    std::cout<<"Setting initial p="<<state[4][0]<<std::endl;
    trk->getTrackRep(0)->setState(state);
    trk->getTrackRep(0)->setStartState(state);
    TMatrixT<double> cov(5,5);
    cov[0][0]=100;
    cov[1][1]=100;
    cov[2][2]=16;
    cov[3][3]=16;
    cov[4][4]=5;
    trk->getTrackRep(0)->setCov(cov);
    trk->getTrackRep(0)->setStartCov(cov);
    DetPlane pl(pos1+TVector3(0,0,-10E-4),TVector3(1,0,0),TVector3(0,1,0));
    trk->getTrackRep(0)->setReferencePlane(pl);
    //    trk->getTrackRep(0)->setStartS(pos1.Z()-10E-4);
  }// end loop over tracks
  
  std::cout<<_trackArray->GetEntriesFast()<<" tracks created"<<std::endl;
  
  return;
}


void
PndTpcPatternRecoTask::WriteHistograms(const TString& filename) {
  TFile* file = new TFile(filename,"UPDATE");
  file->mkdir("PatternReco");
  file->cd("PatternReco");
  
  _multiplicityHisto->Write();
  delete _multiplicityHisto;
  _multiplicityHisto=NULL;

  _trackSizeH->Write();
  delete _trackSizeH;
  _trackSizeH=NULL;

  _trackPurityH->Write();
  delete _trackPurityH;
  _trackPurityH=NULL;

  file->Close();
  delete file;
}
