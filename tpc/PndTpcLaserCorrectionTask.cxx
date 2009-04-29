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
#include "PndTpcClusterizerTask.h"

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
  : FairTask("TPC LaserCorrection"), _persistence(kFALSE)
{
  _clusterBranchName = "PndTpcCluster";
  _recoFileName = "laser.new.reco.root";
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
  _clusterArray=(TClonesArray*) ioman->GetObject(_clusterBranchName);
  
  if(_clusterArray==0)
    {
      Error("PndTpcLaserCorrectionTask::Init","Cluster-array not found!");
      return kERROR;
    }
  
  // create and register output array
  _backupBranchName = _clusterBranchName.Append("_uncorrected");
  _backupArray = new TClonesArray("PndTpcPrimaryCluster"); 
  ioman->Register(_backupBranchName,"PndTpc",_backupArray,_persistence);

  _zMin=_par->getZGem();
  _zMax=_par->getZMax();
  _rMin=_par->getRMin();
  _rMax=_par->getRMax();

  _recoFile = new TFile(_recoFileName);
  
  if(_recoFile->IsZombie()) {
    Error("PndTpcLaserCorrectionTask::Init","LaserReco-File not valid! Aborting ...");
    return kERROR;
  }
  
  TTree* tr = (TTree*) _recoFile->Get("cbmsim");
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
  _recoMapR = stat->getRecoMapR_Spline();
  _recoMapPerp = stat->getRecoMapPerp_Spline();
  if(_recoMapR == NULL || _recoMapPerp == NULL) {
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
  _par= (PndTpcDigiPar*) db->getContainer("PndTpcDigiPar");
  if (! _par ) Fatal("SetParContainers", "PndTpcDigiPar not found");
}




void
PndTpcLaserCorrectionTask::Exec(Option_t* opt)
{
  // Reset output Array
  if(_backupArray==0) Fatal("PndTpcLaserCorrectionTask::Exec)","No BackupClusterArray");
  _backupArray->Delete();

  Int_t ncl=_clusterArray->GetEntriesFast();

  TVector3 clpos;
  
  //begin looping over clusters
  for(unsigned int clust=0; clust<ncl; ++clust) {
    
    PndTpcCluster* cl = (PndTpcCluster*) _clusterArray->At(clust);
        
    //make and store copy of uncorrected cluster
    
    //PndTpcCluster* cl_back = new((*_backupArray)[clust]) PndTpcCluster(*cl);
    
    clpos = cl->pos();

    double phi = clpos.Phi();
    
    //rotate cl into X-Z plane in which the deviation map is defined
    clpos.RotateZ(-phi);
    double r = clpos.X();
    double z = clpos.Z();
    
    //read reco-deviations
    double reco_dr = _recoMapR->eval(z,r);
    double reco_dperp = _recoMapPerp->eval(z,r);
    
    clpos.SetXYZ(r-reco_dr,-reco_dperp,z);
    
    //rotate back
    clpos.RotateZ(phi);
    cl->_pos = clpos;
        
  } //end loop over clusters

  return;
}





ClassImp(PndTpcLaserCorrectionTask)
