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
#include "CbmRootManager.h"
#include "TClonesArray.h"
#include "TRandom.h"
#include "PndTpcDriftedElectron.h"
#include "PndTpcAvalanche.h"
#include "PndTpcGem.h"
#include "CbmRunAna.h"
#include "CbmRuntimeDb.h"
#include "PndTpcDigiPar.h"

#include <iostream>
#include <cmath>

using std::log;

// Class Member definitions -----------


PndTpcGemTask::PndTpcGemTask()
  : CbmTask("TPC Gem"), _persistence(kFALSE), _gainFluctuations(kFALSE),
    _initialized(kFALSE)
{
  _driftedBranchName = "PndTpcDriftedElectron";
}


PndTpcGemTask::~PndTpcGemTask()
{}


void
PndTpcGemTask::SetParContainers() {

  std::cout<<"PndTpcGemTask::SetParContainers"<<std::endl;
  std::cout.flush();

  // Get run and runtime database
  CbmRunAna* run = CbmRunAna::Instance();
  if ( ! run ) Fatal("SetParContainers", "No analysis run");

  CbmRuntimeDb* db = run->GetRuntimeDb();
  if ( ! db ) Fatal("SetParContainers", "No runtime database");

  // Get PndTpc digitisation parameter container
  _par= (PndTpcDigiPar*) db->getContainer("PndTpcDigiPar");
  if (! _par ) Fatal("SetParContainers", "PndTpcDigiPar not found");
}


InitStatus
PndTpcGemTask::Init()
{

  _initialized=false;
  //Get ROOT Manager
  CbmRootManager* ioman= CbmRootManager::Instance();

  if(ioman==0)
    {
      Error("PndTpcGemTask::Init","RootManager not instantiated!");
      return kERROR;
    }
  
  // Get input collection
  _driftedArray=(TClonesArray*) ioman->GetObject(_driftedBranchName);
  
  if(_driftedArray==0)
    {
      Error("PndTpcGemTask::Init","DriftedElectron-array not found!");
      return kERROR;
    }
  
  // create and register output array
  _avalancheArray = new TClonesArray("PndTpcAvalanche"); 
  ioman->Register("PndTpcAvalanche","PndTpc",_avalancheArray,_persistence);

  _gain=_par->getGem()->gain();

  _initialized=true;
  return kSUCCESS;
}


void
PndTpcGemTask::Exec(Option_t* opt)
{
  // Reset output Array
  if(_avalancheArray==0) Fatal("PndTpcPrimCluster::Exec)","No AvalancheArray");
  _avalancheArray->Delete();
  Int_t counter=0;
  Int_t nd=_driftedArray->GetEntriesFast();
  std::cout<<"Aggregating drifted electrons into avalanches ";
  // loop over drifted electrons
  for(Int_t id=0;id<nd;++id){
    PndTpcDriftedElectron* e=(PndTpcDriftedElectron*)_driftedArray->At(id);
    int na=_avalancheArray->GetEntriesFast();
    // look if there is already an avalanche where we can put this electron
    bool found=false;
    /*if(na%1000==0)std::cout<<".";std::cout.flush();
    for(int ia=0; ia<na; ++ia){
      PndTpcAvalanche* a=(PndTpcAvalanche*)_avalancheArray->At(ia);
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
      a->addAmp(_gain);
      found=true;
      ++counter;
      //std::cout<<"Combined an electron into an existing avalanche"<<std::endl;
      break;
    }
    */
    if(!found){
	  double gainFactor = 1.;
	  if(_gainFluctuations) {
		gainFactor = log(1/(1-gRandom->Uniform()));
	  }
      new ((*_avalancheArray)[na]) PndTpcAvalanche(e->x(), e->y(), e->t(),
						_gain*gainFactor, e);
    }
  } // end loop over drifted electrons
  std::cout<<" finished."<<std::endl;
  std::cout<<_avalancheArray->GetEntriesFast()
	   <<" Avalanches created"<<std::endl;
  std::cout<<counter<<" aggregations done."<<std::endl;
  return;
}

ClassImp(PndTpcGemTask)
