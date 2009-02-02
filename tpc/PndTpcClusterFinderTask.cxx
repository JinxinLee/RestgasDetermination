//-----------------------------------------------------------
// File and Version Information:
// $Id$
//
// Description:
//      Implementation of class PndTpcClusterFinderTask
//      see PndTpcClusterFinderTask.hh for details
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
#include "PndTpcClusterFinderTask.h"

// C/C++ Headers ----------------------
#include <algorithm>
#include <iostream>

// Collaborating Class Headers --------
#include "CbmRootManager.h"
#include "CbmRunAna.h"
#include "CbmRuntimeDb.h"
#include "PndTpcDigiPar.h"
#include "TClonesArray.h"
#include "PndTpcDigi.h"
#include "PndTpcDigiAge.h"
#include "PndTpcFrontend.h"
#include "PndTpcGas.h"
#include "PndTpcGem.h"
#include "PndTpcPadPlane.h"
#include "PndTpcDigiMapper.h"
#include "RecoHitFactory.h"
#include "PndTpcClusterFinder.h"

// Class Member definitions -----------


PndTpcClusterFinderTask::PndTpcClusterFinderTask()
  : CbmTask("TPC Cluster Finder"), _persistence(kFALSE),_trivial(kFALSE),
    _timeslice(2), _mode(0)
{
  _digiBranchName = "PndTpcDigi";
}


PndTpcClusterFinderTask::~PndTpcClusterFinderTask()
{}


void
PndTpcClusterFinderTask::SetParContainers() {

  std::cout<<"PndTpcClusterFinderTask::SetParContainers"<<std::endl;
  std::cout.flush();

  // Get run and runtime database
  CbmRun* run = CbmRun::Instance();
  if ( ! run ) Fatal("SetParContainers", "No analysis run");

  CbmRuntimeDb* db = run->GetRuntimeDb();
  if ( ! db ) Fatal("SetParContainers", "No runtime database");

  // Get PndTpc digitisation parameter container
  _par= (PndTpcDigiPar*) db->getContainer("PndTpcDigiPar");
  if (! _par ) Fatal("SetParContainers", "PndTpcDigiPar not found");
}


InitStatus
PndTpcClusterFinderTask::Init()
{
  //Get ROOT Manager
  CbmRootManager* ioman= CbmRootManager::Instance();

  if(ioman==0)
    {
      Error("PndTpcClusterFinderTask::Init","RootManager not instantiated!");
      return kERROR;
    }
  
  // Get input collection
  _digiArray=(TClonesArray*) ioman->GetObject(_digiBranchName);
  
  if(_digiArray==0)
    {
      Error("PndTpcClusterFinderTask::Init","Digi-array not found!");
      return kERROR;
    }
  
  // create and register output array
  _clusterArray = new TClonesArray("PndTpcCluster"); 
  ioman->Register("PndTpcCluster","PndTpc",_clusterArray,_persistence);

  
  //TODO: Get this from Database!
  _par->printParams();
  _frontend= _par->getFrontend();
  _padplane= _par->getPadPlane();
  _gas=      _par->getGas();
  _zGem=     _par->getZGem();
  double sf= _par->getFrontend()->samplingFrequency();
  double t0= _par->getFrontend()->t0();

  PndTpcDigiMapper::getInstance(false)->init(_padplane,_gem,_gas,_zGem,t0,sf);
 
  _cluster_buffer=new std::vector<PndTpcCluster*>;
  _finder=new PndTpcClusterFinder(PndTpcDigiMapper::getInstance()->getPadPlane(),
			       _cluster_buffer,
			       _timeslice, _mode);
  
  _finder->checkConsistency();
  _finder->setTrivialClustering(_trivial);

  return kSUCCESS;
}


void
PndTpcClusterFinderTask::Exec(Option_t* opt)
{
  std::cout<<"PndTpcClusterFinderTask::Exec"<<std::endl;
  // Reset output Array
  if(_clusterArray==0) Fatal("PndTpcClusterFinder::Exec)","No ClusterArray");
   _clusterArray->Delete();

   _finder->reset();

   //for sorting
   std::vector<PndTpcDigi*>digis;
   

  // For now: trivial clustering;
   Int_t ndigis=_digiArray->GetEntriesFast();
   for(Int_t i=0;i<ndigis;++i){
     PndTpcDigi* digi=(PndTpcDigi*)_digiArray->At(i);
     digis.push_back(digi);
   }

   _finder->process(digis);
   //sort(digis.begin(),digis.end(),PndTpcDigiAge());

   /*for(Int_t i=0;i<ndigis;++i){
     PndTpcDigi* digi=digis[i];
     TVector3 pos;
     PndTpcDigiMapper::getInstance()->map(digi,pos);
     PndTpcCluster* c=new((*_clusterArray)[i]) PndTpcCluster(pos,digi->amp());
     c->SetMcId(digi->mcId().DominantID());
     }*/

   // put clusters into array and clean up buffer
   unsigned int ncl=_cluster_buffer->size();
   unsigned int ndig=0;
   for(unsigned int icl=0;icl<ncl;++icl){
     PndTpcCluster* cl=new((*_clusterArray)[icl]) PndTpcCluster(*(*_cluster_buffer)[icl]);
     cl->SetIndex(icl);
     ndig+=(*_cluster_buffer)[icl]->size();
     delete (*_cluster_buffer)[icl];
   }


   std::cout<<_clusterArray->GetEntriesFast()<<" cluster created "
	    <<" containing "<<ndig<<" digis"
	    <<" from "<<ndigis<<std::endl;
   
   _cluster_buffer->clear();
   digis.clear();
   
  return;
}

ClassImp(PndTpcClusterFinderTask)
