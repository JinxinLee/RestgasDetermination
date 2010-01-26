//-----------------------------------------------------------
// File and Version Information:
// $Id$
//
// Description:
//      Implementation of class PndTpcIdealTrackingTask2
//      see PndTpcIdealTrackingTask2.hh for details
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
#include "PndTpcIdealTrackingTask2.h"

// C/C++ Headers ----------------------
#include <vector>
#include <map>
#include <algorithm>

// Collaborating Class Headers --------
#include "FairRootManager.h"
#include "TClonesArray.h"
#include "GFException.h"
#include "PndTpcCluster.h"
#include "PndTpcClusterZ.h"
#include "PndTpcClusterDist.h"
#include "PndTrackCand.h"
#include "GFTrack.h"
#include "LSLTrackRep.h"
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

ClassImp(PndTpcIdealTrackingTask2)

PndTpcIdealTrackingTask2::PndTpcIdealTrackingTask2()
  : FairTask("PndTpc Ideal Pattern Reco"),
  _persistence(kFALSE), _secondarySuppression(kFALSE)
{
  _clusterBranchName = "PndTpcCluster";
}

PndTpcIdealTrackingTask2::~PndTpcIdealTrackingTask2()
{
  if(_multiplicityHisto!=NULL)delete _multiplicityHisto;
  if(_trackPurityH!=NULL)delete _trackPurityH;
  if(_trackSizeH!=NULL)delete _trackSizeH;
}


InitStatus
PndTpcIdealTrackingTask2::Init()
{
//Get ROOT Manager
  FairRootManager* ioman= FairRootManager::Instance();

  if(ioman==0)
    {
      Error("PndTpcIdealTrackingTask2::Init","RootManager not instantiated!");
      return kERROR;
    }

  // Get input collection
  _clusterArray=(TClonesArray*) ioman->GetObject(_clusterBranchName);

  if(_clusterArray==0)
    {
      Error("PndTpcIdealTrackingTask2::Init","Cluster-array not found!");
      return kERROR;
    }
  _mcTrackArray=(TClonesArray*) ioman->GetObject("MCTrack");

  if(_mcTrackArray==0)
    {
      Error("PndTpcIdealTrackingTask2::Init","MCTrack-array not found!");
      return kERROR;
    }
  // create and register output array
  _candArray = new TClonesArray("PndTrackCand");
  ioman->Register("TrackCand","GenFit",_candArray,_persistence);
 
  
  // init histos
  _multiplicityHisto=new TH1I("multipl","# track candidates",20,0,20);
  _trackSizeH=new TH1I("trksize","# hits in track",100,0,100);
  _trackPurityH=new TH1D("trkpurity","trackPurity",25,0,1.01);
  _trackMcIdsH=new TH1D("trkmcids","# mcids in track",25,0,25);

  return kSUCCESS;
}

void
PndTpcIdealTrackingTask2::Exec(Option_t* opt)
{
  std::cout<<"PndTpcIdealTrackingTask2::Exec"<<std::endl;
  // Reset output Arrays
  if(_candArray==0) Fatal("PndTpcIdealTracking::Exec)","No TrackArray");
  _candArray->Delete();
  
  // copy into vector
  std::vector<PndTpcCluster*> cll;
  unsigned int n=_clusterArray->GetEntriesFast();
  for(unsigned int i=0;i<n;++i){ // loop over clusters
    PndTpcCluster* cl=(PndTpcCluster*)_clusterArray->At(i);
    cl->SetIndex(i);
    cll.push_back(cl);
  }
  
  //register track candidates in a map
  //required to cope with negative track IDs
  std::map<unsigned int,PndTrackCand*> candlist;

  for(unsigned int i=0;i<n;i++){ // loop over clusters   
    PndTpcCluster* cl=cll[i];
    int trackid=cl->mcId().DominantID().mctrackID();
    
    if(_secondarySuppression)
      if(trackid!=0)
	continue;
    
    //PndTrackCand* cand=(PndTrackCand*)_candArray->At(trackid);
    PndTrackCand* cand = candlist[trackid];
    
    if(cand==NULL){
      cand = new PndTrackCand();
      candlist[trackid]=cand;
      
      //security check for faulty MC events
      if(trackid < 0 || trackid > _mcTrackArray->GetEntriesFast()-1) 
	break;
      
      PndMCTrack* trk=(PndMCTrack*)_mcTrackArray->At(trackid);
      
      TVector3 mom=trk->GetMomentum();
      TVector3 pos=trk->GetStartVertex();
      TVector3 dir=mom;
      double p=mom.Mag();  //momentum magnitude
      dir=dir*(1/p);
      
      int pdg=trk->GetPdgCode();
      
      double q=1.;
      if(TDatabasePDG::Instance()->GetParticle(trk->GetPdgCode()))	{
	q=TDatabasePDG::Instance()->GetParticle(trk->GetPdgCode())->Charge()/3.;
      }
      else	{
	std::cout << "PndTpcIdealTrackingTask2: Can' t get Particle for PDG " << trk->GetPdgCode() <<
	  "q just set to 1" << std::endl;
      }

      //Set charge to 1 initially
      cand->setTrackSeed(pos,dir,q/p);
      
    }

    //register the hit
    cand->AddHit(2,cl->index(),cl->pos().Mag());
    
  }// end loop over clusters

  //register found candidates in output array
  std::map<unsigned int,PndTrackCand*>::iterator candit=candlist.begin();
  int count=0;
  while(candit!=candlist.end()){
    PndTrackCand* cand=candit->second;
    
    //throw away too small candidates
    if(cand->GetNHits()<5){
      ++candit;
      continue;
    }
    int size = _candArray->GetEntriesFast();
    PndTrackCand* nc=new ((*_candArray)[size]) PndTrackCand(*cand); 
    count++;
    candit++;
  }
  
  std::cout<<"PndTpcIdealTrackingTask2::Exec:: "
	   <<count<<" track candidates found."<<std::endl;
  _multiplicityHisto->Fill(count);
  
  return;
}


void
PndTpcIdealTrackingTask2::WriteHistograms(const TString& filename) {
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
