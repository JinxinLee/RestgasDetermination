//-----------------------------------------------------------
// File and Version Information:
// $Id$
//
// Description:
//      Implementation of class PndTpcMCSkimmingTask
//      see PndTpcMCSkimmingTask.hh for details
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
#include "PndTpcMCSkimmingTask.h"

// C/C++ Headers ----------------------


// Collaborating Class Headers --------
#include "FairRootManager.h"
#include "TClonesArray.h"
#include "PndTpcPoint.h"
#include "PndMCTrack.h"
#include "PndGemMCPoint.h"
#include "PndSdsMCPoint.h"

#include "FairRunAna.h"


#include <iostream>
#include <cmath>
#include <map>

using std::cout;
using std::cerr;
using std::endl;
using std::map;
using std::floor;

// Class Member definitions -----------


PndTpcMCSkimmingTask::PndTpcMCSkimmingTask()
  : FairTask("TPC MCSkimming") 
{
  fpointBranchName = "PndTpcPoint";
  ftrackBranchName = "MCTrack";
  fmvdBranchName = "MVDPoint";
  fgemBranchName = "GEMPoint";
}


PndTpcMCSkimmingTask::~PndTpcMCSkimmingTask()
{}

void
PndTpcMCSkimmingTask::SetParContainers() {

  std::cout<<"PndTpcMCSkimmingTask::SetParContainers"<<std::endl;
  std::cout.flush();

 //  // Get run and runtime database
//   FairRun* run = FairRun::Instance();
//   if ( ! run ) Fatal("SetParContainers", "No analysis run");

//   FairRuntimeDb* db = run->GetRuntimeDb();
//   if ( ! db ) Fatal("SetParContainers", "No runtime database");

//   // Get PndTpc digitisation parameter container
//   fpar= (PndTpcDigiPar*) db->getContainer("PndTpcDigiPar");
//   if (! fpar ) Fatal("SetParContainers", "PndTpcDigiPar not found");
}



InitStatus
PndTpcMCSkimmingTask::Init()
{
  //Get ROOT Manager
  FairRootManager* ioman= FairRootManager::Instance();

  if(ioman==0)
    {
      Error("PndTpcMCSkimmingTask::Init","RootManager not instantiated!");
      return kERROR;
    }
  
  // Get input/output collections TPC, MVD, GEM, MCTrack
  fpointArray=(TClonesArray*) ioman->GetObject(fpointBranchName);  
  if(fpointArray==0)
    {
      Error("PndTpcMCSkimmingTask::Init","Point-array not found!");
      return kERROR;
    }
  fpointOutArray=new TClonesArray("PndTpcPoint");
  ioman->Register("PndTpcPoint","PndTpc",fpointOutArray,kTRUE);
 
  // Get input/output collections TPC, MVD, GEM, MCTrack
  fmvdArray=(TClonesArray*) ioman->GetObject(fmvdBranchName);  
  if(fmvdArray==0)
    {
      Error("PndTpcMCSkimmingTask::Init","MVD-array not found!");
      return kERROR;
    }
  fmvdOutArray=new TClonesArray("PndSdsMCPoint");
  ioman->Register("MVDPoint","PndMvd",fmvdOutArray,kTRUE);

// Get input/output collections TPC, MVD, GEM, MCTrack
  fgemArray=(TClonesArray*) ioman->GetObject(fgemBranchName);  
  if(fgemArray==0)
    {
      Error("PndTpcMCSkimmingTask::Init","GEM-array not found!");
      return kERROR;
    }
  fgemOutArray=new TClonesArray("PndGemMCPoint");
  ioman->Register("GEMPoint","PndGem",fgemOutArray,kTRUE);

  // Get input collection
  ftrackArray=(TClonesArray*) ioman->GetObject(ftrackBranchName); 
  if(ftrackArray==0)
    {
      Error("PndTpcMCSkimmingTask::Init","MCTrack-array not found!");
      return kERROR;
    }
  ftrackOutArray=new TClonesArray("PndMCTrack");
  ioman->Register("MCTrack","Stack",ftrackOutArray,kTRUE);

  



   return kSUCCESS;
}

void
PndTpcMCSkimmingTask::Exec(Option_t* opt)
{
 
  // cleanup output arrays
  fgemOutArray->Delete();
  fmvdOutArray->Delete();
  fpointOutArray->Delete();
  ftrackOutArray->Delete();
  

  //Get ROOT Manager
  FairRunAna* run= FairRunAna::Instance();

  Int_t nt=ftrackArray->GetEntries();
  Int_t np=fpointArray->GetEntries();

  // loop over MC tracks and find primaries

  // loop over points and check that we have 
  // enough points for each primary in the tpc
  // otherwise skim event

  // first: ID of primary track; second: number of points in tpc
  map<unsigned int,unsigned int> mcmap;

  for(unsigned int it=0;it<nt;++it){
    PndMCTrack* track=(PndMCTrack*)ftrackArray->At(it);
    //check if primary
    if(track->GetMotherID()!=-1)continue;
    mcmap[it]=0;
  }
  
  for(unsigned int ip=0;ip<np;++ip){
    PndTpcPoint* point=(PndTpcPoint*)fpointArray->At(ip);
    if(point->GetSecID()!=0)continue;
    mcmap[point->GetTrackID()]++;
  } // end loop over hits
  

  // loop over mcmap to check if all primary tracks have enough hits
  bool keep=true;
  map<unsigned int, unsigned int>::iterator mit=mcmap.begin();
  while(mit!=mcmap.end()){
    if(mit->second<20){
      cerr << "Track " << mit->first << " has " << mit->second << " < 20 points in TPC" << endl;
      run->SkimThisEvent();keep=false;
    }
    ++mit;
  }


  if(keep){
    // copy data to output
    unsigned int n=fgemArray->GetEntries();
    for(unsigned int i=0;i<n;++i)new((*fgemOutArray)[i]) PndGemMCPoint(*(PndGemMCPoint*)fgemArray->At(i));
   n=fmvdArray->GetEntries();
    for(unsigned int i=0;i<n;++i)new((*fmvdOutArray)[i]) PndSdsMCPoint(*(PndSdsMCPoint*)fmvdArray->At(i));
    n=fpointArray->GetEntries();
    for(unsigned int i=0;i<n;++i)new((*fpointOutArray)[i]) PndTpcPoint(*(PndTpcPoint*)fpointArray->At(i));
    n=ftrackArray->GetEntries();
    for(unsigned int i=0;i<n;++i)new((*ftrackOutArray)[i]) PndMCTrack(*(PndMCTrack*)ftrackArray->At(i));

  }

  return;
}


ClassImp(PndTpcMCSkimmingTask)
