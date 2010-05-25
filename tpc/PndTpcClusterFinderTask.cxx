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
#include "FairRootManager.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"
#include "PndTpcDigiPar.h"
#include "TClonesArray.h"
#include "PndTpcDigi.h"
#include "PndTpcDigiAge.h"
#include "PndTpcFrontend.h"
#include "PndTpcGas.h"
#include "PndTpcGem.h"
#include "PndTpcPadPlane.h"
#include "PndTpcDigiMapper.h"
#include "GFRecoHitFactory.h"
#include "PndTpcClusterFinder.h"

// Class Member definitions -----------


PndTpcClusterFinderTask::PndTpcClusterFinderTask()
  : FairTask("TPC Cluster Finder"), fpersistence(kFALSE),ftrivial(kFALSE),
    ftimeslice(2), fmode(0),fthres(1)
{
  fdigiBranchName = "PndTpcDigi";
}
 

PndTpcClusterFinderTask::~PndTpcClusterFinderTask()
{}


void
PndTpcClusterFinderTask::SetParContainers() {

  std::cout<<"PndTpcClusterFinderTask::SetParContainers"<<std::endl;
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
PndTpcClusterFinderTask::Init()
{
  //Get ROOT Manager
  FairRootManager* ioman= FairRootManager::Instance();

  if(ioman==0)
    {
      Error("PndTpcClusterFinderTask::Init","RootManager not instantiated!");
      return kERROR;
    }
  
  // Get input collection
  fdigiArray=(TClonesArray*) ioman->GetObject(fdigiBranchName);
  
  if(fdigiArray==0)
    {
      Error("PndTpcClusterFinderTask::Init","Digi-array not found!");
      return kERROR;
    }
  
  // create and register output array
  fclusterArray = new TClonesArray("PndTpcCluster"); 
  ioman->Register("PndTpcCluster","PndTpc",fclusterArray,fpersistence);

  
  fpar->printParams();
  ffrontend= fpar->getFrontend();
  fpadplane= fpar->getPadPlane();
  fgas=      fpar->getGas();
  fzGem=     fpar->getZGem();
  double sf= fpar->getFrontend()->samplingFrequency();
  double t0= fpar->getFrontend()->t0();

  std::cout << "T0 " << t0 << "sF " << sf << std::endl;

  PndTpcDigiMapper::getInstance(false)->init(fpadplane,fgem,fgas,fzGem,t0,sf);
 
  fcluster_buffer=new std::vector<PndTpcCluster*>;
  ffinder=new PndTpcClusterFinder(PndTpcDigiMapper::getInstance()->getPadPlane(),
				  fcluster_buffer,
				  ftimeslice, fmode);
  
  ffinder->checkConsistency();
  ffinder->setTrivialClustering(ftrivial);
  ffinder->saveRaw(fpersistence);

  return kSUCCESS;
}


void
PndTpcClusterFinderTask::Exec(Option_t* opt)
{
  std::cout<<"PndTpcClusterFinderTask::Exec"<<std::endl;
  // Reset output Array
  if(fclusterArray==0) Fatal("PndTpcClusterFinder::Exec)","No ClusterArray");
   fclusterArray->Delete();

   ffinder->reset();

   //for sorting
   std::vector<PndTpcDigi*> digis;
   

  // For now: trivial clustering;
   Int_t ndigis=fdigiArray->GetEntries();
   //   std::cout << "FINDER"<< ndigis << std::endl;
   for(Int_t i=0;i<ndigis;++i){
     PndTpcDigi* digi=(PndTpcDigi*)fdigiArray->At(i);
     //     digi->Print();
     digis.push_back(digi);
   }

   try{
     ffinder->process(digis);
   } catch (std::exception& e) {
     std::cout << e.what() << std::endl;
   } catch (...) {
     std::cout << "unknown exception..." << std::endl;
   }
   //sort(digis.begin(),digis.end(),PndTpcDigiAge());

   /*for(Int_t i=0;i<ndigis;++i){
     PndTpcDigi* digi=digis[i];
     TVector3 pos;
     PndTpcDigiMapper::getInstance()->map(digi,pos);
     PndTpcCluster* c=new((*fclusterArray)[i]) PndTpcCluster(pos,digi->amp());
     c->SetMcId(digi->mcId().DominantID());
     }*/

   // put clusters into array and clean up buffer
   unsigned int ncl=fcluster_buffer->size();
   unsigned int ndig=0;
   unsigned int ncl_rec=0;
   for(unsigned int icl=0;icl<ncl;++icl)
     {
       if((*fcluster_buffer)[icl]->amp()>fthres)
	 {
	   PndTpcCluster* cl=new((*fclusterArray)[ncl_rec]) PndTpcCluster(*(*fcluster_buffer)[icl]);
	   cl->SetIndex(ncl_rec);
	   ndig+=(*fcluster_buffer)[icl]->size();
	   ncl_rec++;
	 }
       delete (*fcluster_buffer)[icl];
     }
   
   std::cout<<fclusterArray->GetEntriesFast()<<" cluster created "
	    <<" containing "<<ndig<<" digis"
	    <<" from "<<ndigis<<std::endl;
   
   fcluster_buffer->clear();
   digis.clear();
   
  return;
}

ClassImp(PndTpcClusterFinderTask)
