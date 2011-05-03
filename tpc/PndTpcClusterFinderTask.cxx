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
#include "PndTpcClusterFinderSimple.h"

// Class Member definitions -----------

PndTpcClusterFinderTask::PndTpcClusterFinderTask()
  : FairTask("TPC Cluster Finder"), fpersistence(kFALSE), fDigiPersistence(kFALSE),ftrivial(kFALSE),fsimple(kFALSE),
    ftimeslice(2), fmode(0),fthres(1), fSDiClAmpCut(0), fClAmpCut(0), fDataMode(kFALSE), fDiffFactor(1.), fClusterTimeCut(5),
    fAdcSens(600.), fC(300.)
{
  fdigiBranchName = "PndTpcDigi";
  fClusterOutName = "PndTpcCluster";
}
 

PndTpcClusterFinderTask::~PndTpcClusterFinderTask() {
}


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

void PndTpcClusterFinderTask::SetSimpleClustering(Bool_t opt){
  fsimple=opt;
  if(fsimple)
    std::cerr<<"\n PndTpcClusterFinderTask::SetSimpleClustering  \n \
    You want to use SimpleClustering. \n \
    This modifies the Digis (amplitude sharing between clusters). If you want to access the digis, set : \n \
    tpcCF->SetDigiPersistence(); \n \
    Then, references to digis and their contribution to the cluster amplitude are saved in the cluster"<<std::endl;
}

InitStatus
PndTpcClusterFinderTask::Init()
{
  //Get ROOT Manager
  FairRootManager* ioman= FairRootManager::Instance();

  if(ioman==0) {
    Error("PndTpcClusterFinderTask::Init","RootManager not instantiated!");
    return kERROR;
  }
  
  // Get input collection
  fdigiArray=(TClonesArray*) ioman->GetObject(fdigiBranchName);
  
  if(fdigiArray==0){
    Error("PndTpcClusterFinderTask::Init","Digi-array not found!");
    return kERROR;
  }
  
  // create and register output array
  fclusterArray = new TClonesArray("PndTpcCluster"); 
  ioman->Register(fClusterOutName,"PndTpc",fclusterArray,fpersistence);


  // init the DigiMapper
  fpar->printParams();
  ffrontend= fpar->getFrontend();
  fpadplane= fpar->getPadPlane();
  fgem=      fpar->getGem();
  fgas=      fpar->getGas();
  fzGem=     fpar->getZGem();
  double sf= fpar->getFrontend()->samplingFrequency();
  double t0= fpar->getFrontend()->t0();
  double gain=fpar->getGain();

  std::cout << "T0 " << t0 << "sF " << sf << std::endl;

  PndTpcDigiMapper::getInstance(false)->init(fpadplane,fgem,fgas,fpar->getPadShapes(),fzGem,t0,sf);
 
  fcluster_buffer = new std::vector<PndTpcCluster*>;

  // init the ClusterFinder
  if(!fsimple){
    ffinder=new PndTpcClusterFinder(PndTpcDigiMapper::getInstance()->getPadPlane(),
            fcluster_buffer,
            ftimeslice, fmode, -1,fDataMode,fDiffFactor,fClusterTimeCut,gain/fAdcSens,fC);
  }
  else{
    ffinder=new PndTpcClusterFinderSimple(PndTpcDigiMapper::getInstance()->getPadPlane(),
            fcluster_buffer,
            ftimeslice, gain/fAdcSens, fC);
    ((PndTpcClusterFinderSimple*)(ffinder))->setNoXclust(false);
  }
  
  ffinder->checkConsistency();
  ffinder->setTrivialClustering(ftrivial);
  ffinder->saveRaw(fDigiPersistence);
  

  return kSUCCESS;
}


void
PndTpcClusterFinderTask::Exec(Option_t* opt)
{
  std::cerr<<"PndTpcClusterFinderTask::Exec"<<std::endl;

  // Reset output Array
  if(fclusterArray==0) Fatal("PndTpcClusterFinder::Exec)","No ClusterArray");
  fclusterArray->Delete();

  ffinder->reset();

  // put digis into vector
  Int_t ndigis=fdigiArray->GetEntriesFast();
  std::vector<PndTpcDigi*> digis(ndigis);
  for(unsigned int idigi=0; idigi<ndigis; ++idigi)
    digis[idigi] = (PndTpcDigi*)fdigiArray->At(idigi);
  

  std::cerr<<"process digis ";
  try{
    ffinder->process(digis);   
  } catch (std::exception& e) {
    std::cout << e.what() << std::endl;
  } catch (...) {
    std::cout << "unknown exception..." << std::endl;
  }
  std::cerr<<"... done"<<std::endl;
  

  /*for(Int_t i=0;i<ndigis;++i){
    PndTpcDigi* digi=digis[i];
    TVector3 pos;
    PndTpcDigiMapper::getInstance()->map(digi,pos);
    PndTpcCluster* c=new((*fclusterArray)[i]) PndTpcCluster(pos,digi->amp());
    c->SetMcId(digi->mcId().DominantID());
  }*/
   

  // put clusters into array and clean up buffer
  unsigned int ncl=fcluster_buffer->size();
  unsigned int ncl_rec=0;
  unsigned int ndig_rec=0;
  
  std::cerr<<"copying clusters to output array ";
  double amp;
  int size;
  for(unsigned int icl=0;icl<ncl;++icl){ // loop over clusters
    amp  = (*fcluster_buffer)[icl]->amp();
    size = (*fcluster_buffer)[icl]->size();
    if(amp>fthres &&  (size>1 && amp>fClAmpCut*(double)size || size==1 && amp>fSDiClAmpCut)){
      // copy cluster
      PndTpcCluster* cl = new((*fclusterArray)[ncl_rec]) PndTpcCluster(*((*fcluster_buffer)[icl]));
      cl->SetIndex(ncl_rec); 
      ncl_rec++;
      ndig_rec+=cl->size();
    }
    // delete cluster
    delete  (*fcluster_buffer)[icl];
    
  } // end loop over clusters
  
  std::cerr<<"... done"<<std::endl;
  
  
  int splitDigis=0;
  if(fsimple){
    splitDigis = ((PndTpcClusterFinderSimple*)(ffinder))->NsplitDigis();
    ndig_rec -= splitDigis;
  } 
  std::cout<<fclusterArray->GetEntriesFast()<<" cluster created "
	<<" containing "<<ndig_rec<<" digis"
  <<" from "<<fdigiArray->GetEntriesFast()<<std::endl;
  if(fsimple){
    std::cout<<" SimpleClustering assigned "<< splitDigis <<" split digis to clusters"<<std::endl;
  }   
   
  fcluster_buffer->clear();

   
 /*  
  for(unsigned int i=0; i<fclusterArray->GetEntriesFast(); ++i){
    PndTpcCluster* cl = (PndTpcCluster*)(*fclusterArray)[i];
    std::cout<<"Testing cluster "<<i<<" of "<<fclusterArray->GetEntriesFast()<<"  ";
    cl->pos().Print();
  }

  for(unsigned int i=0; i<fdigiArray->GetEntriesFast(); ++i){
    PndTpcDigi* cl = (PndTpcDigi*)(*fdigiArray)[i];
    std::cout<<"Testing Raw Digi "<<i<<" of "<<fdigiArray->GetEntriesFast()<<"  "<<cl->amp()<<std::endl;
  }*/
   
   
  std::cerr<<" End ClusterFinderTask " <<std::endl;
  return;
}


bool PndTpcDigiIndex(PndTpcDigi* digi1, PndTpcDigi* digi2){
  return(digi1->index() < digi2->index());
}


ClassImp(PndTpcClusterFinderTask)

