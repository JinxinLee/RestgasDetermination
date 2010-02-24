//-----------------------------------------------------------
// File and Version Information:
// $Id$
//
// Description:
//      Implementation of class PndTpcSLResidualTask
//     
// Environment:
//      Software developed for the PANDA Detector at FAIR.
//
// Author List:
//	Felix Boehmer    	TUM     (original author)
//
//-----------------------------------------------------------



// This Class' Header ------------------
#include "PndTpcSLResidualTask.h"

// C++ headers
#include <iostream>
#include <algorithm>
#include <vector>

// Collaborating Class Headers --------
#include "FairRootManager.h"
#include "FairRun.h"
#include "FairRuntimeDb.h"
#include "PndTpcDigiPar.h" 
#include "TVector3.h"
#include "GFTrack.h"
#include "PndTpcCluster.h"
#include "TrackFitStat.h"



PndTpcSLResidualTask::PndTpcSLResidualTask()
  : fClusterBranchName("PndTpcCluster"),
    fTrackBranchName("TrackPostFit"),
    fPersistence(kTRUE),
    fSecondarySupp(kFALSE)
{;}

PndTpcSLResidualTask::~PndTpcSLResidualTask(){
  delete fPar;
}


InitStatus
PndTpcSLResidualTask::Init() {

  //Get ROOT Manager ------------------
  FairRootManager* ioman= FairRootManager::Instance();
  if(ioman==0)
    {
      Error("PndTpcSLResidualTask::Init","RootManager not instantiated!");
      return kERROR;
    }
  
  // Get input ------------------------
  fClusterArray=(TClonesArray*) ioman->GetObject(fClusterBranchName);
  if(fClusterArray==0) {
    Error("PndTpcSLResidualTask::Init","PndTpcCluster-Array not found!");
    return kERROR;
  }
  fTrackArray=(TClonesArray*) ioman->GetObject(fTrackBranchName);
  if(fTrackArray==0) {
    Error("PndTpcSLResidualTask::Init","GFTrack-Array not found!");
    return kERROR;
  }
  
  // Create & register output
  fResArray = new TClonesArray("TVector3");
  ioman->Register("PndTpcSLResiduals","PndTpc",fResArray,fPersistence);

  fFitStatArray = new TClonesArray("TrackFitStat");
  ioman->Register("TrackFitStat","GenFit",fFitStatArray,fPersistence);
  

  return kSUCCESS;
}


void 
PndTpcSLResidualTask::SetParContainers() {

  std::cout<<"PndTpcSLResidualTask::SetParContainers"<<std::endl;
  std::cout.flush();

  // Get run and runtime database
  FairRun* run = FairRun::Instance();
  if ( ! run ) Fatal("SetParContainers", "No analysis run");

  FairRuntimeDb* db = run->GetRuntimeDb();
  if ( ! db ) Fatal("SetParContainers", "No runtime database");

  // Get PndTpc digitisation parameter container
  fPar= (PndTpcDigiPar*) db->getContainer("PndTpcDigiPar");
  if (!fPar ) Fatal("SetParContainers", "PndTpcDigiPar not found");
}


void
PndTpcSLResidualTask::Exec(Option_t* opt) {
  
  //clear output
  if(fFitStatArray==0) Fatal("TrackFitStat::Exec)","No FitStat OutputArray");
  fFitStatArray->Delete();
  if(fResArray==0) Fatal("TrackFitStat::Exec)","No McAnnex OutputArray");
  fResArray->Delete();

  //get fit results
  assert(fTrackArray->GetEntriesFast()<2);
  
  unsigned int nTr = fTrackArray->GetEntriesFast();
  
  for(unsigned int n=0; n<nTr; n++) {
    GFTrack* tr = (GFTrack*) fTrackArray->At(n);
    int failedHits = tr->getFailedHits();

    if(fSecondarySupp==true  && n>0)
      continue;
    
    if(tr!=NULL) {
      
      unsigned int nFS = fFitStatArray->GetEntriesFast();
      TrackFitStat* fitstat = new ((*fFitStatArray)[nFS]) TrackFitStat();
      
      TVector3 mom = tr->getMom();     //momentum after fit
      
      TVector3 sl_pos = tr->getPos();  //SL starting position after fit
      TVector3 sl_dir = mom;
      sl_dir*=(1/mom.Mag());

      fitstat->setp(mom.Mag());
      fitstat->setmom(mom);
      fitstat->addFailedHits(failedHits);
      
      unsigned int nCl = fClusterArray->GetEntriesFast();

      std::vector<double> resX;
      std::vector<double> resY;
      std::vector<double> amps;
      std::vector<unsigned int> clSize;
      
      
      //Loop over clusters
      for(unsigned int k=0; k<nCl; k++){
	PndTpcCluster* cl = (PndTpcCluster*)fClusterArray->At(k);
	TVector3 cl_pos = cl->pos();
			
	//calculate residual
	TVector3 cl_rel = cl_pos-sl_pos;
	double l = sl_dir*cl_rel; //length along SL to projection point
	TVector3 res = cl_rel - l*sl_dir;
	
	//book residual
	unsigned int nRes = fResArray->GetEntriesFast();
	new ((*fResArray)[nRes]) TVector3(res);

	resX.push_back(res.X());
	resY.push_back(res.Y());
	amps.push_back(cl->amp());
	clSize.push_back(cl->size());	
	
      }//End loop over clusters

      fitstat->fillPndTpcResX(resX);
      fitstat->fillPndTpcResY(resY);
      fitstat->fillPndTpcClusterSize(clSize);
      fitstat->fillPndTpcClusterAmp(amps);
      
      
    }
  }
}

ClassImp(PndTpcSLResidualTask)
