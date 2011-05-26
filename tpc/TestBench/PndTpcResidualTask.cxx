//-----------------------------------------------------------
// File and Version Information:
// $Id$
//
// Description:
//      Residual calculation for curved tracks
//
//
// Environment:
//      Software developed for the PANDA Detector at FAIR.
//
// Author List:
//      Felix Boehmer           TUM      (original author)
//      Johannes Rauch          TUM
//
//-----------------------------------------------------------



// This Class' Header ------------------
#include "PndTpcResidualTask.h"

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
#include "PndTpcCluster.h"
#include "TrackFitStat.h"

#include "TGeoManager.h"

#include "GFAbsTrackRep.h"
#include "GFAbsRecoHit.h"
#include "GFDetPlane.h"
#include "GFException.h"
#include "GFKalman.h"
//#include "GFTools.h"
#include "GFTrack.h"
#include "GFTrackCand.h"

#include "RKTrackRep.h"

#define DEBUG 0


PndTpcResidualTask::PndTpcResidualTask()
  : fClusterBranchName("PndTpcCluster"),
    fTrackBranchName("TrackPostFit"),
    fPersistence(kTRUE),
    fSecondarySupp(kFALSE),
    fNumReps(1)
{;}

PndTpcResidualTask::~PndTpcResidualTask(){
  delete fPar;
}


InitStatus
PndTpcResidualTask::Init() {

  //Get ROOT Manager ------------------
  FairRootManager* ioman= FairRootManager::Instance();
  if(ioman==0)
    {
      Error("PndTpcResidualTask::Init","RootManager not instantiated!");
      return kERROR;
    }
  
  // Get input ------------------------
  fClusterArray=(TClonesArray*) ioman->GetObject(fClusterBranchName);
  if(fClusterArray==0) {
    Error("PndTpcResidualTask::Init","PndTpcCluster-Array not found!");
    return kERROR;
  }
  fTrackArray=(TClonesArray*) ioman->GetObject(fTrackBranchName);
  if(fTrackArray==0) {
    Error("PndTpcResidualTask::Init","GFTrack-Array not found!");
    return kERROR;
  }
  std::cout<<"PRE INIT"<<std::endl;
  
  //GFTrack* tr = (GFTrack*)fTrackArray->At(0);
  //unsigned int nReps = tr->getNumReps();
  for(unsigned int r=0; r<fNumReps; r++) {
    fFitStatArray = new TClonesArray("TrackFitStat");
    TString title = "TrackFitStat_";
    char buffer[5];
    sprintf(buffer,"%i",r);
    title.Append(buffer);
    ioman->Register(title,"GenFit",fFitStatArray,fPersistence);
    fOutArrayMap[r] = fFitStatArray;
  }
  
  std::cout<<"INIT OUT ARRAYS"<<std::endl;
  
  // Create & register output
  fResArray = new TClonesArray("TVector3");
  ioman->Register("PndTpcResiduals","PndTpc",fResArray,fPersistence);
   
  

  return kSUCCESS;
}


void 
PndTpcResidualTask::SetParContainers() {

  std::cout<<"PndTpcResidualTask::SetParContainers"<<std::endl;
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
PndTpcResidualTask::Exec(Option_t* opt) {
  
  //clear output
  if(fFitStatArray==0) Fatal("TrackFitStat::Exec()","No FitStat OutputArray");
  fFitStatArray->Delete();
  if(fResArray==0) Fatal("TrackFitStat::Exec()","Residual output array missing");
  fResArray->Delete();

  //get fit results
  //assert(fTrackArray->GetEntriesFast()<2);
  
  unsigned int nTr = fTrackArray->GetEntriesFast();

  std::vector<unsigned int> candIDs;
  
  for(unsigned int n=0; n<nTr; n++) {
    GFTrack* track = (GFTrack*) fTrackArray->At(n);
    int failedHits = track->getFailedHits();
    unsigned int NDF = track->getNDF();
    double Chi2 = track->getChiSqu();

    GFTrackCand cand = track->getCand();
    candIDs.clear(); 
    candIDs = cand.GetHitIDs(2);
    
    /*if(fSecondarySupp==true  && n>0)
      continue;*/
    
    if(track!=NULL) {
      
      for(unsigned int r=0; r<fOutArrayMap.size(); r++) {
	fFitStatArray = fOutArrayMap[r];
	unsigned int nFS = fFitStatArray->GetEntriesFast();
	TrackFitStat* fitstat = new ((*fFitStatArray)[nFS]) TrackFitStat();
	if(DEBUG) {
	  std::cout<<"PndTpcResidualTask::Exec(): Processing track #"
		   <<nFS<<std::endl;
	}
      
	GFAbsTrackRep* rep = track->getTrackRep(r);
	
	TVector3 mom = track->getMom();     //momentum after fit

	fitstat->setp(mom.Mag());
	fitstat->setmom(mom);
	fitstat->addFailedHits(failedHits);
	
	std::vector<double> resX;
	std::vector<double> resY;
	std::vector<double> resZ;
	std::vector<double> resU;
	std::vector<double> resV;
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
	
	
	TVector3 track_pos;
	GFAbsRecoHit* hit;
	GFDetPlane plane;
	
	//Loop over clusters
	for(unsigned int k=0; k<candIDs.size(); k++){
	  PndTpcCluster* cl = (PndTpcCluster*)fClusterArray->At(candIDs[k]);
	  TVector3 cl_pos = cl->pos();
	  
	  hit = track->getHit(k);
	  
	  try{
	    plane = hit->getDetPlane(rep);
	    rep->extrapolate(plane);
	  }
	  catch(GFException& e) {
	    std::cerr << "Error: Exception caught" << std::endl;
	    std::cerr << e.what();
	    break;
	    /*if (e.isFatal()) {
	      std::cerr<<"Fatal exception, skipping track"<<std::endl;
	      break;
	      }
	      else{
	      std::cerr<<"Exception, skipping hit"<<std::endl;
	      continue;
	      }*/
	  }
	  track_pos = rep->getPos(plane);
	  
	  pps.push_back(track_pos);
	  //calculate residual
	  TVector3 res = cl_pos-track_pos;
	  TVector3 resUV;
	  resUV.SetX(res*plane.getU());
	  resUV.SetY(res*plane.getV());
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
	  resU.push_back(resUV.X());
	  resV.push_back(resUV.Y());
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
	fitstat->fillPndTpcResU(resU);
	fitstat->fillPndTpcResV(resV);
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
	fitstat->fillPndTpcHitIDs(candIDs);
	fitstat->setChi2(Chi2);
	fitstat->setNDF(NDF);
	
      }
    }
  }
}

ClassImp(PndTpcResidualTask)
