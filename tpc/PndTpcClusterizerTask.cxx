//-----------------------------------------------------------
// File and Version Information:
// $Id$
//
// Description:
//      Implementation of class PndTpcClusterizerTask
//      see PndTpcClusterizerTask.hh for details
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
#include "PndTpcClusterizerTask.h"

// C/C++ Headers ----------------------


// Collaborating Class Headers --------
#include "FairRootManager.h"
#include "TClonesArray.h"
#include "PndTpcPoint.h"
#include "PndTpcGas.h"
#include "TRandom.h"
#include "PndTpcPrimaryCluster.h"
#include "LinearInterpolPolicy.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"
#include "PndTpcDigiPar.h"

#include <iostream>
#include <cmath>

using std::cout;
using std::endl;
using std::fabs;
using std::floor;

// Class Member definitions -----------


PndTpcClusterizerTask::PndTpcClusterizerTask()
  : FairTask("TPC Clusterizer"), fpersistence(kFALSE),
    fmereChargeConversion(kFALSE), fPoti(20.77e-9)
{
  fpointBranchName = "PndTpcPoint";
}


PndTpcClusterizerTask::~PndTpcClusterizerTask()
{}

void
PndTpcClusterizerTask::SetParContainers() {

  std::cout<<"PndTpcClusterizerTask::SetParContainers"<<std::endl;
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



InitStatus
PndTpcClusterizerTask::Init()
{
  //Get ROOT Manager
  FairRootManager* ioman= FairRootManager::Instance();

  if(ioman==0)
    {
      Error("PndTpcClusterizerTask::Init","RootManager not instantiated!");
      return kERROR;
    }
  
  // Get input collection
  fpointArray=(TClonesArray*) ioman->GetObject(fpointBranchName);
  
  if(fpointArray==0)
    {
      Error("PndTpcClusterizerTask::Init","Point-array not found!");
      return kERROR;
    }
  
  // create and register output array
  fprimArray = new TClonesArray("PndTpcPrimaryCluster"); 
  ioman->Register("PndTpcPrimaryCluster","PndTpc",fprimArray,fpersistence);

  fgas=fpar->getGas();

  return kSUCCESS;
}

void
PndTpcClusterizerTask::Exec(Option_t* opt)
{
  // Reset output Array
  if(fprimArray==0) Fatal("PndTpcPrimCluster::Exec)","No PrimClusterArray");
  fprimArray->Delete();

  Int_t np=fpointArray->GetEntriesFast();
  if(np<2){
    Warning("PndTpcClusterizerTask::Exec","Not enough Hits in PndTpc for Digitization (<2)");
    return;
  }
  
  if(fmereChargeConversion)	{
  	ChargeConversion();
  	return; //goodbye, you wretched world!
  }
  
  PndTpcPoint* point;
  PndTpcPoint* theLastPoint;
  Int_t icluster=0; 
  theLastPoint= (PndTpcPoint*)fpointArray->At(0);

  for(int ip=1;ip<np;++ip){
    point=(PndTpcPoint*) fpointArray->At(ip);
    //point->Print();

    // check if points are not too far from each other
    TVector3 p1;point->Position(p1);
    TVector3 p2;theLastPoint->Position(p2);

    TVector3 d=p1-p2;
    if(d.Mag()>1){
      theLastPoint=point;
      continue;
    }

    //check if hits ly on the same track
    if(point->GetTrackID()==theLastPoint->GetTrackID()){
      double dE=point->GetEnergyLoss()*1E9; //convert from GeV to eV

      //Step 0: calculate the overall ammount of charge, produced
      if(dE<0){
        Error("PndTpcClusterizerTask::Exec","Note: particle:: negative Energy loss!");
        theLastPoint=point;
        continue;
      }
      unsigned int q_total =(unsigned int)floor(fabs(dE / fgas->W()));
      unsigned int q_cluster=0;
      unsigned int ncluster=0;
      //Step 1: Create Clusters

      //while still charge not used-up distribute charge into next cluster

      while(q_total>0){
        //roll dice for next clustersize
        q_cluster=fgas->GetRandomCS(gRandom->Uniform());
        if(q_cluster>q_total)q_cluster=q_total;
        q_total-=q_cluster;
        // create cluster
        Int_t size = fprimArray->GetEntriesFast();
        new((*fprimArray)[size]) PndTpcPrimaryCluster(point->GetTime(),
						   q_cluster,
						   TVector3(0,0,0),
						   point->GetTrackID(),
						   ip,
						   point->GetSecID());
	
        ++ncluster;
     }// finish loop for cluster creation

      //Step 2: Distribute Clusters along track segment
      LinearInterpolPolicy().Interpolate(theLastPoint,point,fprimArray,icluster,ncluster);
      icluster+=ncluster;
      }//end check for same track
    theLastPoint=point;
  } // finish loop over GHits

  std::cout<<"PndTpcClusterizer:: "<<fprimArray->GetEntriesFast()<<" clusters created"<<std::endl;

  return;
}

void PndTpcClusterizerTask::ChargeConversion()
{
  //hardcoded value from ALICE paper:
  //const Float_t w_ion = 35.97e-9; //mean energy for pair creation 
 
  Float_t w_ion = fgas->W()*1.e-9;
  
  Int_t np=fpointArray->GetEntriesFast();	
  for(int ip=1;ip<np;++ip)
  {
  	PndTpcPoint* point=(PndTpcPoint*) fpointArray->At(ip);
	//Do no clustering just convert energy deposition to ionisation
	
	if(point->GetEnergyLoss() < fPoti)
	  continue;
       
	int nel = int(floor(((point->GetEnergyLoss())-fPoti)/w_ion)) + 1;
	  
	//nel=TMath::Min(nel,300); // 300 electrons corresponds to 10 keV

	Int_t size = fprimArray->GetEntriesFast();
	new((*fprimArray)[size])PndTpcPrimaryCluster(point->GetTime(),
						     nel,
						     TVector3(point->GetX(),
							      point->GetY(),
							      point->GetZ()),
						     point->GetTrackID(),
						     ip,
						     point->GetSecID());
  }
}


ClassImp(PndTpcClusterizerTask)
