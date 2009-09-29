//-----------------------------------------------------------
// File and Version Information:
// $Id$
//
// Description:
//      Implementation of class PndTpcGemTask
//      see PndTpcGemTask.hh for details
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
#include "PndTpcGemTask.h"

// C/C++ Headers ----------------------


// Collaborating Class Headers --------
#include "FairRootManager.h"
#include "TClonesArray.h"
#include "TRandom.h"
#include "PndTpcDriftedElectron.h"
#include "PndTpcAvalanche.h"
#include "PndTpcGem.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"
#include "PndTpcDigiPar.h"

#include <iostream>
#include <cmath>

using std::log;

// Class Member definitions -----------


PndTpcGemTask::PndTpcGemTask()
  : FairTask("TPC Gem"), fpersistence(kFALSE), fgainFluctuations(kFALSE),
    finitialized(kFALSE)
{
  fdriftedBranchName = "PndTpcDriftedElectron";
}


PndTpcGemTask::~PndTpcGemTask()
{}


void
PndTpcGemTask::SetParContainers() {

  std::cout<<"PndTpcGemTask::SetParContainers"<<std::endl;
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
PndTpcGemTask::Init()
{

  finitialized=false;
  //Get ROOT Manager
  FairRootManager* ioman= FairRootManager::Instance();

  if(ioman==0)
    {
      Error("PndTpcGemTask::Init","RootManager not instantiated!");
      return kERROR;
    }
  
  // Get input collection
  fdriftedArray=(TClonesArray*) ioman->GetObject(fdriftedBranchName);
  
  if(fdriftedArray==0)
    {
      Error("PndTpcGemTask::Init","DriftedElectron-array not found!");
      return kERROR;
    }
  
  // create and register output array
  favalancheArray = new TClonesArray("PndTpcAvalanche"); 
  ioman->Register("PndTpcAvalanche","PndTpc",favalancheArray,fpersistence);

  fgain=fpar->getGem()->gain();

  finitialized=true;
  return kSUCCESS;
}


void
PndTpcGemTask::Exec(Option_t* opt)
{
  // Reset output Array
  if(favalancheArray==0) Fatal("PndTpcPrimCluster::Exec)","No AvalancheArray");
  favalancheArray->Delete();
  Int_t counter=0;
  Int_t nd=fdriftedArray->GetEntriesFast();
  std::cout<<"Aggregating drifted electrons into avalanches ";
  // loop over drifted electrons
  for(Int_t id=0;id<nd;++id){
    PndTpcDriftedElectron* e=(PndTpcDriftedElectron*)fdriftedArray->At(id);
    int na=favalancheArray->GetEntriesFast();
    // look if there is already an avalanche where we can put this electron
    bool found=false;
    /*if(na%1000==0)std::cout<<".";std::cout.flush();
    for(int ia=0; ia<na; ++ia){
      PndTpcAvalanche* a=(PndTpcAvalanche*)favalancheArray->At(ia);
      // calculate distance in t and cut
      double dt=e->t()-a->t(); 
      if(fabs(dt)>20) {
      	//std::cout<<"t cut"<<dt<<std::endl;
      	continue; // cut on dt>ns
      }
      // calculate distance in xy and cut
      double dx=a->x()-e->x();
      double dy=a->y()-e->y();
      double dr=sqrt(dx*dx+dy*dy);
      if(dr>0.1) {
      	//std::cout<<"dr cut"<<std::endl;  
      	continue; // cut on dr>1mm
      }
      // if electron survived enlarge amplitude of avalanche
      a->addAmp(fgain);
      found=true;
      ++counter;
      //std::cout<<"Combined an electron into an existing avalanche"<<std::endl;
      break;
    }
    */
    if(!found){
	  double gainFactor = 1.;
	  if(fgainFluctuations) 
	    gainFactor = -log(gRandom->Uniform());
	  //TODO: implement full set: exp. distr. for first stage,
	  //representing rest
	  
      new ((*favalancheArray)[na]) PndTpcAvalanche(e->x(), e->y(), e->t(),
						fgain*gainFactor, e);
    }
  } // end loop over drifted electrons
  std::cout<<" finished."<<std::endl;
  std::cout<<favalancheArray->GetEntriesFast()
	   <<" Avalanches created"<<std::endl;
  std::cout<<counter<<" aggregations done."<<std::endl;
  return;
}

ClassImp(PndTpcGemTask)
