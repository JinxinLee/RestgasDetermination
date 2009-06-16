//-----------------------------------------------------------
// File and Version Information:
// $Id$
//
// Description:
//      Implementation of class PndTpcIdealTrackingTask
//      see PndTpcIdealTrackingTask.hh for details
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
#include "PndTpcIdealTrackingTask.h"

// C/C++ Headers ----------------------
#include <vector>
#include <map>
#include <algorithm>

// Collaborating Class Headers --------
#include "FairRootManager.h"
#include "TClonesArray.h"
#include "RecoHitFactory.h"
#include "FitterExceptions.h"
#include "PndTpcCluster.h"
#include "PndTpcClusterZ.h"
#include "PndTpcClusterDist.h"
#include "TrackCand.h"
#include "Track.h"
#include "LSLTrackRep.h"
#include "GeaneTrackRep.h"
#include "FairGeanePro.h"
#include "TH1I.h"
#include "TH1D.h"
#include "McIdCollection.h"
#include "TVector3.h"
#include "PndMCTrack.h"

//#include "AbsBFieldIfc.h"
//#include "FairFieldAdaptor.h"

#include <cmath>

using std::fabs;

// Class Member definitions -----------

ClassImp(PndTpcIdealTrackingTask)

PndTpcIdealTrackingTask::PndTpcIdealTrackingTask()
  : FairTask("PndTpc Ideal Pattern Reco"),
    _persistence(kFALSE),_useGeane(kFALSE),_geanePro(NULL), 
    _useDistSorting(kFALSE)
{
  myrandom.SetSeed(1);
  _clusterBranchName = "PndTpcCluster";
}

PndTpcIdealTrackingTask::~PndTpcIdealTrackingTask()
{
  if(_multiplicityHisto!=NULL)delete _multiplicityHisto;
  if(_trackPurityH!=NULL)delete _trackPurityH;
  if(_trackSizeH!=NULL)delete _trackSizeH;
}


InitStatus
PndTpcIdealTrackingTask::Init()
{
//Get ROOT Manager
  FairRootManager* ioman= FairRootManager::Instance();

  if(ioman==0)
    {
      Error("PndTpcIdealTrackingTask::Init","RootManager not instantiated!");
      return kERROR;
    }

  // Get input collection
  _clusterArray=(TClonesArray*) ioman->GetObject(_clusterBranchName);

  if(_clusterArray==0)
    {
      Error("PndTpcIdealTrackingTask::Init","Cluster-array not found!");
      return kERROR;
    }
  _mcTrackArray=(TClonesArray*) ioman->GetObject("MCTrack");

  if(_mcTrackArray==0)
    {
      Error("PndTpcIdealTrackingTask::Init","MCTrack-array not found!");
      return kERROR;
    }
  // create and register output array
  _trackArray = new TClonesArray("Track");
  ioman->Register("TrackPreFit","GenFit",_trackArray,_persistence);
 // GeanePro will get Geometry and BField from the Run
  if(_useGeane)_geanePro=new FairGeanePro();
  
  // init histos
  _multiplicityHisto=new TH1I("multipl","# track candidates",20,0,20);
  _trackSizeH=new TH1I("trksize","# hits in track",100,0,100);
  _trackPurityH=new TH1D("trkpurity","trackPurity",25,0,1.01);
  _trackMcIdsH=new TH1D("trkmcids","# mcids in track",25,0,25);

  return kSUCCESS;
}

void
PndTpcIdealTrackingTask::Exec(Option_t* opt)
{
  std::cout<<"PndTpcIdealTrackingTask::Exec"<<std::endl;
  // Reset output Arrays
  if(_trackArray==0) Fatal("PndTpcIdealTracking::Exec)","No TrackArray");
  _trackArray->Delete();
  
  // copy into vector
  std::vector<PndTpcCluster*> cll;
  unsigned int n=_clusterArray->GetEntriesFast();
  for(unsigned int i=0;i<n;++i){ // loop over clusters
    PndTpcCluster* cl=(PndTpcCluster*)_clusterArray->At(i);
    cl->SetIndex(i);
    cll.push_back(cl);
  }
  
  // sort dir=false -> forward sorting
  if(_useDistSorting) {
    std::sort(cll.begin(),cll.end(),PndTpcClusterDist(false)); 
    std::cout<<"\n **** using DISTANCE presorting of PndTpcClusters ****"<<std::endl;
    int errcount=0;
    double mag=0;
    for(int i=0; i<cll.size(); i++)
      if (((cll[i])->pos()).Mag() < mag)
	errcount++;
    if (errcount>0) //debug
      std::cout<<"\n      WARNING: SORTING NOT RIGHT ! ! ! ! ("
	       <<errcount<<" errors)"<<std::endl;
  }
  else
    std::sort(cll.begin(),cll.end(),PndTpcClusterZ(false)); 

  // build trackcands
  std::map<unsigned int,TrackCand*> candlist;
  for(unsigned int i=0;i<n;i++){ // loop over clusters   
    PndTpcCluster* cl=cll[i];
    int trackid=cl->mcId().DominantID().mctrackID();
    TrackCand* cand=candlist[trackid];
    if(cand==NULL){
      cand=new TrackCand();
      candlist[trackid]=cand;
      
      //security check for faulty MC events
      if(trackid < 0 || trackid > _mcTrackArray->GetEntriesFast()-1) 
	break;
      
      PndMCTrack* trk=(PndMCTrack*)_mcTrackArray->At(trackid);
      TVector3 mom=trk->GetMomentum();
      double curv=0.3*2.0/mom.Perp(); // 1/R=0.3*B/pt   pt in GeV, B in Tesla
      cand->setCurv(curv);
      cand->setDip(mom.Theta());
    }
    cand->addHit(2,cl->index());
  }// end loop over clusters

  std::cout<<"PndTpcIdealTrackingTask::Exec:: "
	   <<candlist.size()<<" track candidates found."<<std::endl;
  _multiplicityHisto->Fill(candlist.size());

  //loop over track candidates
  std::map<unsigned int,TrackCand*>::iterator candit=candlist.begin();
  int count =0;
  while(candit!=candlist.end()){
    count++;
    std::cout<<count<<std::endl;
    TrackCand* cand=candit->second;
    if(cand->getNHits()<10){
      ++candit;
      continue;
    }
    unsigned int trackid=candit->first;
    PndMCTrack* mc=(PndMCTrack*)_mcTrackArray->At(trackid);
    int pdg=mc->GetPdgCode();
    //avoid geane crash for deuterons
    if(pdg>1000000000) {
      ++candit;
      continue;
    }
    double q=1.;
    if(TDatabasePDG::Instance()->GetParticle(mc->GetPdgCode()))	{
      q=TDatabasePDG::Instance()->GetParticle(mc->GetPdgCode())->Charge()/3.;
    }
    else	{
      std::cout << "PndTpcIdealTrackingTask: Can' t get Particle for PDG " << mc->GetPdgCode() <<
	"q just set to 1" << std::endl;
    }
    
    
    TVector3 pos=mc->GetStartVertex()+TVector3(myrandom.Uniform(-0.1,0.1),
					       myrandom.Uniform(-0.1,0.1),
					       myrandom.Uniform(-0.1,0.1));
    if(pos.Mag()<1.E-3) pos.SetMag(1.E-3);
    TVector3 mom=mc->GetMomentum();
    mom.SetMag(mom.Mag()*myrandom.Uniform(0.9,1.1));
    mom.SetTheta(mom.Theta() + myrandom.Uniform(-2./180.*TMath::Pi(),
						2./180.*TMath::Pi()));
    mom.SetPhi(mom.Phi() + myrandom.Uniform(-2./180.*TMath::Pi(),
					    2./180.*TMath::Pi()));
    
    TVector3 poserr(1.,1.,1.);
    TVector3 momerr=0.3*mom; //  10% error
    momerr+=TVector3(0.1,0.1,0.1);
  
    //TVector3 u(1.,0.,0.);
    TVector3 u=mom.Orthogonal();
    u.SetMag(1.);
    //TVector3 v(0.,1.,0.);
    TVector3 v=mom.Cross(u);
    v.SetMag(1.);
    
    // create track-representation object and initialize with start values
    AbsTrackRep* rep=0;
    if(_useGeane){
      DetPlane pl(pos,u,v);
      
      GeaneTrackRep* grep=new GeaneTrackRep(_geanePro,pl,mom,poserr,momerr,q,pdg);
      grep->setPropDir(1); // propagate in flight direction!
      rep=grep;
      
    }
    else { // use LSLTrackRep
      // calc momentum projections
      std::cout<<"\nUsing LSL Track Representation"<<std::endl;
      TVector3 dir=mom.Unit();
      double dxdz=dir.X()/dir.Z();
      double dydz=dir.Y()/dir.Z();
      double qp=q/mom.Mag();
      rep=new LSLTrackRep(pos.Z(),pos.X(),pos.Y(),dxdz,dydz,qp,
			  poserr.X(),poserr.Y(),0.1,0.1,0.1,NULL);
      
    }
    
    
    // create track object
    Track* trk=new((*_trackArray)[_trackArray->GetEntriesFast()]) Track(rep);
    trk->setCandidate(*cand); // here the candidate is copied! 
    ++candit;
  
  } // end loop over track candidates
  

  std::cout<<_trackArray->GetEntriesFast()<<" tracks created"<<std::endl;
  return;
}


void
PndTpcIdealTrackingTask::WriteHistograms(const TString& filename) {
  TFile* file=FairRootManager::Instance()->GetOutFile();
  file->mkdir("IdealTracking");
  file->cd("IdealTracking");

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
