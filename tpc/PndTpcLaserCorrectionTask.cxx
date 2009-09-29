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
//      Felix Boehmer    TUM            (original author)
//
//
//-----------------------------------------------------------

// Panda Headers ----------------------

// This Class' Header ------------------
#include "PndTpcLaserCorrectionTask.h"

// C/C++ Headers ----------------------


// Collaborating Class Headers --------
#include "FairRootManager.h"
#include "TClonesArray.h"
#include "TFile.h"
#include "TTree.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"
#include "PndTpcDigiPar.h"
#include "BiCubSpline.h"
#include "PndTpcLaserFitTaskStat.h"
#include "PndTpcCluster.h"

#include <iostream>
#include <cmath>




// Class Member definitions -----------


PndTpcLaserCorrectionTask::PndTpcLaserCorrectionTask()
  : FairTask("TPC LaserCorrection"), fpersistence(kFALSE)
{
  fclusterBranchName = "PndTpcCluster";
  frecoFileName = "laser.new.reco.root";
}


PndTpcLaserCorrectionTask::~PndTpcLaserCorrectionTask()
{}



InitStatus
PndTpcLaserCorrectionTask::Init()
{
  //Get ROOT Manager
  FairRootManager* ioman= FairRootManager::Instance();

  if(ioman==0)
    {
      Error("PndTpcLaserCorrectionTask::Init","RootManager not instantiated!");
      return kERROR;
    }
  
  // Get input collection
  fclusterArray=(TClonesArray*) ioman->GetObject(fclusterBranchName);
  
  if(fclusterArray==0)
    {
      Error("PndTpcLaserCorrectionTask::Init","Cluster-array not found!");
      return kERROR;
    }
  
  // create and register output array
  fbackupBranchName = fclusterBranchName.Append("funcorrected");
  fbackupArray = new TClonesArray("PndTpcPrimaryCluster"); 
  ioman->Register(fbackupBranchName,"PndTpc",fbackupArray,fpersistence);

  fzMin=fpar->getZGem();
  fzMax=fpar->getZMax();
  frMin=fpar->getRMin();
  frMax=fpar->getRMax();

  frecoFile = new TFile(frecoFileName);
  
  if(frecoFile->IsZombie()) {
    Error("PndTpcLaserCorrectionTask::Init","LaserReco-File not valid! Aborting ...");
    return kERROR;
  }
  
  TTree* tr = (TTree*) frecoFile->Get("cbmsim");
  if(tr==NULL) {
    Error("PndTpcLaserCorrectionTask::Init",
	  "Could not read tree from LaserReco-File! Aborting ...");
    return kERROR;
  }
  TClonesArray* arr = new TClonesArray("PndTpcLaserFitTaskStat");
  tr->SetBranchAddress("PndTpcLaserFitTaskStat", &arr);
  if(tr->GetEntries()!=1) {
    Error("PndTpcLaserCorrectionTask::Init",
	  "More / less than 1 event in LaserReco-Tree! Aborting ...");
    return kERROR;
  }
  tr->GetEvent(0);
  PndTpcLaserFitTaskStat* stat = (PndTpcLaserFitTaskStat*) arr->At(0);
  frecoMapR = stat->getRecoMapR_Spline();
  frecoMapPerp = stat->getRecoMapPerp_Spline();
  if(frecoMapR == NULL || frecoMapPerp == NULL) {
    Error("PndTpcLaserCorrectionTask::Init",
	  "Could not read Spline objects from LaserReco-File! Aborting ...");
  }
  
  return kSUCCESS;
}



void
PndTpcLaserCorrectionTask::SetParContainers() {

  std::cout<<"PndTpcLaserCorrectionTask::SetParContainers"<<std::endl;
  std::cout.flush();

  // Get run and runtime database
  FairRun* run = FairRun::Instance();
  if ( ! run ) Fatal("SetParContainers", "No analysis run");

  FairRuntimeDb* db = run->GetRuntimeDb();
  if ( ! db ) Fatal("SetParContainers", "No runtime database");

  // Get PndTpc digitisation parameter container
  fpar= (PndTpcDigiPar*) db->getContainer("PndTpcDigiPar");
  if (! fpar ) Fatal("SetParContainers", "PndTpcDigiPar not found");
}




void
PndTpcLaserCorrectionTask::Exec(Option_t* opt)
{
  // Reset output Array
  if(fbackupArray==0) Fatal("PndTpcLaserCorrectionTask::Exec)","No BackupClusterArray");
  fbackupArray->Delete();

  Int_t ncl=fclusterArray->GetEntriesFast();

  TVector3 clpos;
  
  //begin looping over clusters
  for(unsigned int clust=0; clust<ncl; ++clust) {
    
    PndTpcCluster* cl = (PndTpcCluster*) fclusterArray->At(clust);
        
    //make and store copy of uncorrected cluster
    
    //PndTpcCluster* cl_back = new((*fbackupArray)[clust]) PndTpcCluster(*cl);
    
    clpos = cl->pos();

    double phi = clpos.Phi();
    
    //rotate cl into X-Z plane in which the deviation map is defined
    clpos.RotateZ(-phi);
    double r = clpos.X();
    double z = clpos.Z();
    
    //read reco-deviations
    double reco_dr = frecoMapR->eval(z,r);
    double reco_dperp = frecoMapPerp->eval(z,r);
    
    clpos.SetXYZ(r-reco_dr,-reco_dperp,z);
    
    //rotate back
    clpos.RotateZ(phi);
    cl->fpos = clpos;
        
  } //end loop over clusters

  return;
}





ClassImp(PndTpcLaserCorrectionTask)
