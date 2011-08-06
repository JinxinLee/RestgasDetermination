//-----------------------------------------------------------
// File and Version Information:
// $Id$
//
// Description:
//      Implementation of class PndTpcLaserTask
//      see PndTpcLaserTask.h for details
//
// Environment:
//      Software developed for the PANDA Detector at FAIR.
//
// Author List:
//      Felix Boehmer        TUM            (original author)
//
//
//-----------------------------------------------------------


// This Class' Header ---------------------------------------
#include "PndTpcNoiseTask.h"

// Collaborating Class Headers --------
#include "FairRootManager.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"
#include "TClonesArray.h"
#include "PndTpcPrimaryCluster.h"
#include "TVector3.h"
#include "TMath.h"
#include "TError.h"
#include "PndTpcDigiPar.h"
#include "PndTpcGas.h"

#include <iostream>
#include <string>
#include "TRandom3.h"


PndTpcNoiseTask::PndTpcNoiseTask()
  : FairTask("TPC Noise Generator")
{
 
}

PndTpcNoiseTask::~PndTpcNoiseTask()
{
  ;  
}

InitStatus
PndTpcNoiseTask::Init()
{
  //Get ROOT Manager
  FairRootManager* ioman= FairRootManager::Instance();

  if(ioman==0)
    {
      Error("PndTpcNoiseTask::Init","RootManager not instantiated!");
      return kERROR;
    }
  // Get input collection
  fprimArray=(TClonesArray*) ioman->GetObject("PndTpcPrimaryCluster");
  
  if(fprimArray==0)
    {
      Error("PndTpcNoiseTask::Init","PrimaryElectron-array not found!");
      return kERROR;
    }

  //clear input array of any real "physical" hits
  fprimArray->Delete();

   fGas=fpar->getGas();

   return kSUCCESS;
}



void 
PndTpcNoiseTask::SetParContainers() {

  std::cout<<"PndTpcNoiseTask::SetParContainers"<<std::endl;
  std::cout.flush();

  // Get run and runtime database
  FairRun* run = FairRun::Instance();
  if ( ! run ) Fatal("SetParContainers", "No analysis run");

  FairRuntimeDb* db = run->GetRuntimeDb();
  if ( ! db ) Fatal("SetParContainers", "No runtime database");

  // Get PndTpc digitisation parameter container
  fpar = (PndTpcDigiPar*) db->getContainer("PndTpcDigiPar");
  if (! fpar ) Fatal("SetParContainers", "PndTpcDigiPar not found");
}



void
PndTpcNoiseTask::Exec(Option_t* opt)
{  
 
    Int_t nPrim = fprimArray->GetEntriesFast();

    unsigned int nNoise=(unsigned int)floor(nPrim/fstn);
       
    TRandom3 randm;
    randm.SetSeed(12345);
    TVector3 pos(1,0,0);

    // charge W is given in eV -> convert deposited Energy from keV to eV
    double b=1000.*fmeanEnergy;
    while(nNoise>0){
      // position:
      double z=randm.Uniform(fpar->getZGem(),fpar->getZMax());
      double r=randm.Uniform(fpar->getRMin(),fpar->getRMax());
      double phi=randm.Uniform(0,2*TMath::Pi());
      pos.SetZ(z);pos.SetPerp(r);pos.SetPhi(phi);
      // charge W is given in eV -> has been convert to keV above
      unsigned int q=(unsigned int)ceil(randm.Exp(b)/fGas->W());

      nPrim = fprimArray->GetEntriesFast();
      new((*fprimArray)[nPrim]) PndTpcPrimaryCluster(0,q,pos,9999,1);

      nNoise-=q;
    }

  
     
}
    
ClassImp(PndTpcNoiseTask)
