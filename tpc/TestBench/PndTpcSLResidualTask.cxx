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
#include "GFTrackCand.h"
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
  if(fFitStatArray==0) Fatal("TrackFitStat::Exec()","No FitStat OutputArray");
  fFitStatArray->Delete();
  if(fResArray==0) Fatal("TrackFitStat::Exec()","No McAnnex OutputArray");
  fResArray->Delete();

  //get fit results
  //assert(fTrackArray->GetEntriesFast()<2);
  
  unsigned int nTr = fTrackArray->GetEntriesFast();

  std::vector<unsigned int> candIDs;
  
  for(unsigned int n=0; n<nTr; n++) {
    GFTrack* tr = (GFTrack*) fTrackArray->At(n);
    int failedHits = tr->getFailedHits();
    unsigned int NDF = tr->getNDF();
    double Chi2 = tr->getChiSqu();

    GFTrackCand cand = tr->getCand();
    candIDs.clear(); 
    candIDs = cand.GetHitIDs(2);
    
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
      std::vector<double> resZ;
      std::vector<double> sigX;
      std::vector<double> sigY;
      std::vector<double> sigZ;

      std::vector<double> chi2X;
      std::vector<double> chi2Y;
      std::vector<double> chi2Z;
      std::vector<double> amps;
      std::vector<unsigned int> clSize;
      std::vector<unsigned int> cl2DSize;
      std::vector<TVector3> pps;
      std::vector<double> posX;	
      std::vector<double> posY;	
      std::vector<double> posZ;	
      
      
      //Loop over clusters
      for(unsigned int k=0; k<candIDs.size(); k++){
	PndTpcCluster* cl = (PndTpcCluster*)fClusterArray->At(candIDs[k]);
	TVector3 cl_pos = cl->pos();
			
	//calculate residual
	TVector3 cl_rel = cl_pos-sl_pos;
	double l = sl_dir*cl_rel; //length along SL to projection point
	TVector3 pp = l*sl_dir;
	pps.push_back(pp);
	TVector3 res = cl_rel - l*sl_dir;
	//calculate chi2
	TVector3 cl_err= cl->sig();

	TVector3 chi2;
	unsigned int nDim = 4;
	chi2.SetX((res.X()*res.X())/(cl_err.X()*cl_err.X())/nDim);	
	chi2.SetY((res.Y()*res.Y())/(cl_err.Y()*cl_err.Y())/nDim);	
	chi2.SetZ((res.Z()*res.Z())/(cl_err.Z()*cl_err.Z())/nDim);
	//chi2.SetZ(res*res/cl_err*cl_err/(candIDs.size()-4));//candif.size-4 is ndf
      
	//book residual
	unsigned int nRes = fResArray->GetEntriesFast();
	new ((*fResArray)[nRes]) TVector3(res);

	resX.push_back(res.X());
	resY.push_back(res.Y());
	resZ.push_back(res.Z());
	sigX.push_back(cl_err.X());
	sigY.push_back(cl_err.Y());
	sigZ.push_back(cl_err.Z());




	chi2X.push_back(chi2.X());
	chi2Y.push_back(chi2.Y());
	chi2Z.push_back(chi2.Z());
	amps.push_back(cl->amp());
	clSize.push_back(cl->size());	
	cl2DSize.push_back(cl->get2DSize());
	posX.push_back(cl_pos.X());
	posY.push_back(cl_pos.Y());
	posZ.push_back(cl_pos.Z());
	
	
      }//End loop over clusters

      fitstat->fillPndTpcResX(resX);
      fitstat->fillPndTpcResY(resY);
      fitstat->fillPndTpcResZ(resZ);
      fitstat->fillPndTpcSigX(sigX);
      fitstat->fillPndTpcSigY(sigY);
      fitstat->fillPndTpcSigZ(sigZ);

      fitstat->fillPndTpcChi2X(chi2X);
      fitstat->fillPndTpcChi2Y(chi2Y);
      fitstat->fillPndTpcChi2Z(chi2Z);
      fitstat->fillPndTpcClusterSize(clSize);
      fitstat->fillPndTpc2DClusterSize(cl2DSize);
      fitstat->fillPndTpcClusterAmp(amps);
      fitstat->fillPndTpcProjPoints(pps);
      fitstat->fillPndTpcHitPositionsX(posX);
      fitstat->fillPndTpcHitPositionsY(posY);
      fitstat->fillPndTpcHitPositionsZ(posZ);
      fitstat->setChi2(Chi2);
      fitstat->setNDF(NDF);
      
    }
  }
}

ClassImp(PndTpcSLResidualTask)
