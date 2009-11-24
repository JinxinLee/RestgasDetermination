//--------------------------------------------------------------------------
// File and Version Information:
// 	$Id:$
//
// Description:
//      This module takes Clusters (Connected Regions) and slits them
//      up into Bumps. There are defined by local maxima
//
// Environment:
//	Software developed for the BaBar Detector at the SLAC B-Factory.
// Adapted for the PANDA experiment at GSI
//
// Author List:
//	Stephen J. Gowdy           University of Edinburgh
//      Phil Strother              Imperial College 
// 
// Dima Melnychuk, adaption for PANDA
// Modified:
// M. Babai
//------------------------------------------------------------------------

//-----------------------
// This Class's Header --
//-----------------------
#include "PndEmcMakeBump.h"

//-------------------------------
// Collaborating Class Headers --
//-------------------------------

#include "PndEmcStructure.h"
#include "PndEmcDataTypes.h"

#include "PndEmcMapper.h"
#include "PndEmcDigiPar.h"
#include "PndEmcRecoPar.h"
#include "PndEmcBump.h"
#include "PndEmcCluster.h"
#include "PndEmcDigi.h"
#include "PndEmcSharedDigi.h"
#include "PndEmc2DLocMaxFinder.h"
#include "PndEmcExpClusterSplitter.h"
#include "PndEmcTwoCoordIndex.h"

#include "FairRootManager.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"

#include "TClonesArray.h"
#include "TROOT.h"

//---------------
// C++ Headers --
//---------------
#include <iostream>

//OpenMP header file
#ifdef _OPENMP
//#undef _OPENMP
#include<omp.h>
#endif

using std::endl;
using std::cout;

Int_t PndEmcMakeBump::fEventCounter=1;

//----------------
// Constructors --
//----------------
PndEmcMakeBump::PndEmcMakeBump(Int_t verbose, Bool_t storebumps) 
{
  fVerbose=verbose; 
  fStoreBumps=storebumps;
}

//--------------
// Destructor --
//--------------
PndEmcMakeBump::~PndEmcMakeBump()
{
  delete fClusterArray;
  delete fBumpArray;
  delete fSharedDigiArray;
  delete theLocalMaxFinder;
  delete theClusterSplitter;
  delete fDigiPar;
  delete fRecoPar;
}

// -----   Public method Init   -------------------------------
InitStatus PndEmcMakeBump::Init() {
  
  // Get RootManager
  FairRootManager* ioman = FairRootManager::Instance();
  if ( ! ioman ){
    cout << "-E- PndEmcMakeBump::Init: "
	 << "RootManager not instantiated!" << endl;
    return kFATAL;
  }
	
  // Geometry loading
  fMapVersion=fDigiPar->GetMapperVersion();
  PndEmcMapper::Instance(fMapVersion);
  PndEmcStructure::Instance();
  
  // Get input array
  fClusterArray = (TClonesArray*) ioman->GetObject("EmcCluster");
  if ( ! fClusterArray ) {
    cout << "-W- PndEmcMakeBump::Init: "
	 << "No PndEmcCluster array!" << endl;
    return kERROR;
  }
  
  // Create and register output array
  fBumpArray = new TClonesArray("PndEmcBump");
  ioman->Register("EmcBump","Emc",fBumpArray,fStoreBumps);
  // Fill structure with parameters from RunTime DB for local maximum
  // finder
  PndEmc2DLocMaxFinderData locMaxData;
  locMaxData.MaxECut=fRecoPar->GetMaxECut();
  locMaxData.NeighbourECut=fRecoPar->GetNeighbourECut();
  locMaxData.CutSlope=fRecoPar->GetCutSlope();
  locMaxData.CutOffset=fRecoPar->GetCutOffset();
  locMaxData.ERatioCorr=fRecoPar->GetERatioCorr();
  locMaxData.TheNeighbourLevel=fRecoPar->GetTheNeighbourLevel();
  // Create PndEmc2DLocMaxFinder object with correspondent parameters
  theLocalMaxFinder = new PndEmc2DLocMaxFinder(locMaxData,fVerbose);
  
  // Fill structure with parameters from RunTime DB for bump splitter
  PndEmcExpClusterSplitterData  expClusterSplitterData;
  expClusterSplitterData.MoliereRadius=fRecoPar->GetMoliereRadius();
  expClusterSplitterData.ExponentialConstant=fRecoPar->GetExponentialConstant();
  expClusterSplitterData.MaxIterations=fRecoPar->GetMaxIterations();
  expClusterSplitterData.CentroidShift=fRecoPar->GetCentroidShift();
  expClusterSplitterData.MaxBumps=fRecoPar->GetMaxBumps();
  expClusterSplitterData.MinDigiEnergy=fRecoPar->GetMinDigiEnergy();
  
  // Create PndEmcExpClusterSplitter object with correspondent parameters
  theClusterSplitter = new PndEmcExpClusterSplitter(expClusterSplitterData,
						    fVerbose);
  cout << "-I- PndEmcMakeBump: Intialization successfull" << endl;
  
  return kSUCCESS;
}

void PndEmcMakeBump::Exec(Option_t* opt) 
{
  // Reset output array
  if ( ! fBumpArray ) Fatal("Exec", "No Bump Array");
  fBumpArray->Delete();
  std::cout<<"***************** PndEmcMakeBump, event: "
	   <<fEventCounter<<" **************"<<endl;
  
  int nClusters = fClusterArray->GetEntriesFast();
  PndEmcCoordIndexSet tmp_CoordSet_set;
#ifdef _OPENMP
  //std::cout << "With OpenMP" << std::endl;
  //omp_set_num_threads(1);
  int tid = 0;
  tid = 1;
#pragma omp parallel private(tid, tmp_CoordSet_set)
  {
    //static schedule(roundrobin)
#pragma omp for nowait private(tid, tmp_CoordSet_set) schedule(static)
#endif
  //loop over Clusters
  for (Int_t iCluster = 0; iCluster < nClusters; iCluster++){
    //tid = omp_get_thread_num();
    //std::cout << "TID = " << tid << std::endl;
    
    PndEmcCluster* theCluster = (PndEmcCluster*) fClusterArray->At(iCluster);
    // we need it since fMemberDigiMap is transient element
    theCluster->ValidateDigiMap();
    
    //Int_t NDigis = theCluster->NumberOfDigis();
    
    std::vector<PndEmcBump*> theBumps;

    theLocalMaxFinder->findMaxima(theCluster, tmp_CoordSet_set);
    PndEmcCoordIndexSet emcMaxDigis(tmp_CoordSet_set);

    theClusterSplitter->splitCluster(emcMaxDigis, theCluster, iCluster, theBumps);
    
    theCluster->SetNBumps(theBumps.size());
    
    if (fVerbose>=1){		
      std::cout<<"Energy of the cluster = "
	       << theCluster->energy()<<" Number of bumps = "
	       <<theBumps.size()<<endl;
    }

    for (unsigned int  i = 0; i < theBumps.size(); i++){
      Int_t size_ba = fBumpArray->GetEntriesFast();
      PndEmcBump* theNextBump = new((*fBumpArray)[size_ba]) PndEmcBump(*(theBumps[i]));
      
      if ((fVerbose>=1)&&(theBumps.size()>1)){
	std::cout<<"bump energy = "<<theBumps[i]->energy()<<endl;
	std::cout<<"bump position: theta = "<<theBumps[i]->where().Theta()*TMath::RadToDeg()
		 <<", phi = "<<theBumps[i]->where().Phi()*TMath::RadToDeg()<<endl;
      }
    }
  }
#ifdef _OPENMP
  }
#endif
  // At that moment internal state fEnergy and fWhere of Clusters are
  // not initialized, the following make it possible to see energy and
  // position from output root file
  Int_t nBump = fBumpArray->GetEntriesFast();
#ifdef _OPENMP
#pragma omp parallel for //schedule(dynamic)
#endif
  for (Int_t i=0; i<nBump; i++){
    PndEmcBump *tmpbump = (PndEmcBump*) fBumpArray->At(i);
    tmpbump->energy();
    tmpbump->where();
    
    // Double check of bump energy
    if (fVerbose>=2){
      std::cout<<"No "<<i<<", energy = "<<tmpbump->energy()
	       <<", theta = "<<tmpbump->where().Theta()*TMath::RadToDeg()
	       <<", phi = "<<tmpbump->where().Phi()*TMath::RadToDeg()<<std::endl;
    }
  }
  
  if (fVerbose>=1){
    std::cout<<"PndEmcMakeBump:: Number of clusters = "<<nClusters<<std::endl;
    std::cout<<"PndEmcMakeBump:: Number of bumps = "<<nBump<<std::endl;
  }
  fEventCounter++;
  tmp_CoordSet_set.clear();
}//Exec

void PndEmcMakeBump::SetParContainers() {

  // Get run and runtime database
  FairRun* run = FairRun::Instance();
  if ( ! run ) Fatal("SetParContainers", "No analysis run");

  FairRuntimeDb* db = run->GetRuntimeDb();
  if ( ! db ) Fatal("SetParContainers", "No runtime database");
  // Get Emc digitisation parameter container
  fDigiPar = (PndEmcDigiPar*) db->getContainer("PndEmcDigiPar");
  // Get Emc reconstruction parameter container
  fRecoPar = (PndEmcRecoPar*) db->getContainer("PndEmcRecoPar");
}

void PndEmcMakeBump::SetStorageOfData(Bool_t val)
{
  fStoreBumps=val;
  return;
}

ClassImp(PndEmcMakeBump)
