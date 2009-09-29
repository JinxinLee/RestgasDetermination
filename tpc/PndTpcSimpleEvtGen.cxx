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
#include "PndTpcSimpleEvtGen.h"

// Collaborating Class Headers --------
#include "FairRootManager.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"
#include "TClonesArray.h"
#include "PndTpcPrimaryCluster.h"
#include "TVector3.h"
#include "TError.h"
#include "PndTpcDigiPar.h"
#include <iostream>
#include <string>
#include "TRandom3.h"


PndTpcSimpleEvtGen::PndTpcSimpleEvtGen()
  : FairTask("TPC Simple Event Gen")
{
  fmode = "iron55";
  fpos = TVector3(0.,0.,0.);
}

PndTpcSimpleEvtGen::~PndTpcSimpleEvtGen()
{
  ;  
}

InitStatus
PndTpcSimpleEvtGen::Init()
{
  //Get ROOT Manager
  FairRootManager* ioman= FairRootManager::Instance();

  if(ioman==0)
    {
      Error("PndTpcSimpleEvtGen::Init","RootManager not instantiated!");
      return kERROR;
    }
  // Get input collection
  fprimArray=(TClonesArray*) ioman->GetObject("PndTpcPrimaryCluster");
  
  if(fprimArray==0)
    {
      Error("PndTpcSimpleEvtGen::Init","PrimaryElectron-array not found!");
      return kERROR;
    }

  //clear input array of any real "physical" hits
  fprimArray->Delete();

  std::cerr<<"\n\nPndTpcSimpleEvtGen: running in mode *** "<<fmode.c_str()
	   <<" ***\n"<<std::endl;

   return kSUCCESS;
}



void 
PndTpcSimpleEvtGen::SetParContainers() {

  std::cout<<"PndTpcSimpleEvtGen::SetParContainers"<<std::endl;
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
PndTpcSimpleEvtGen::Exec(Option_t* opt)
{  
  if(fmode.compare("iron55") == 0) {
    Int_t nPrim = fprimArray->GetEntriesFast();
    if(nPrim!=0) 
      Fatal("PndTpcSimpleEvtGen::Exec","Input-Array not empty!");
      
    std::cerr<<"\n\nPndTpcSimpleEvtGen: running in mode *** "<<fmode.c_str()
	     <<" ***\n"<<std::endl;
    std::cerr<<"PndTpcSimpleEvtGen: Position: ("<<fpos.X()<<", "
	     <<fpos.Y()<<", "<<fpos.Z()<<")"<<std::endl;
    
    TRandom3 randm;
    randm.SetSeed(1);
    
    for(unsigned int n=0; n<207; ++n) {
      TVector3 postemp = fpos+TVector3(randm.Uniform(-0.01,0.01),
				       randm.Uniform(-0.01,0.01),
				       randm.Uniform(-0.01,0.01));
				      
      nPrim = fprimArray->GetEntriesFast();
      new((*fprimArray)[nPrim]) PndTpcPrimaryCluster(n,1,postemp,1,1);
      //new((*fprimArray)[nPrim]) PndTpcPrimaryCluster(1,293,postemp,1,1);
    }
  }
}   
 
    
ClassImp(PndTpcSimpleEvtGen)
