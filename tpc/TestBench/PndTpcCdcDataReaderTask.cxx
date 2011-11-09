//-----------------------------------------------------------
// File and Version Information:
// $Id$
//
// Description:
//     eader for T2K and FOPI data for the gemTPCdecoding program
//
//
// Environment:
//      Software developed for the PANDA Detector at FAIR.
//
// Author List:
//      Maxence Vandenbroucke    TUM            (original author)
//
//
//-----------------------------------------------------------


// Panda Headers ----------------------

// This Class' Header ------------------
#include "PndTpcCdcDataReaderTask.h"

// C/C++ Headers ----------------------
#include <vector>
#include <map>
#include <algorithm>

// Collaborating Class Headers --------
#include "FairRootManager.h"
#include "TClonesArray.h"
#include "TVector3.h"
#include "PndTpcDigiMapper.h"

#include "McIdCollection.h"

#include <cmath>
#include "FairRunAna.h"
#include "FairRuntimeDb.h"

#include "FopiEvent.h" 
using std::fabs;

// Class Member definitions -----------

ClassImp(PndTpcCdcDataReaderTask)


PndTpcCdcDataReaderTask::PndTpcCdcDataReaderTask()
: _persistence(kFALSE), fCutoff(0),
  fEventNb(0), fSpillNb(0), fEventId(NULL)
{
  fSampleBranchName = "PndTpcSample";
  fCdcEvBranchName = "PndTpcCdcEvent";
  fCdcHitBranchName = "PndTpcCdcHit";
  fCdcTrackBranchName = "PndTpcCdcTrack";
  fRpcTrackBranchName = "PndTpcRpcTrack";
  fFile = "TBtest/run.root";
  nEvents=0;
  fEv=NULL; //not setting this to 0 gives a segfault. ROOT is fully awesome
}

PndTpcCdcDataReaderTask::~PndTpcCdcDataReaderTask()
{;}

InitStatus
PndTpcCdcDataReaderTask::Init()
{
  //Get ROOT Manager
  FairRootManager* ioman= FairRootManager::Instance();
  if(ioman==0) {
      Error("PndTpcCdcDataReaderTask::Init","RootManager not instantiated!");
      return kERROR;
    }
  
  // Get input data

  fIntree = new TChain("tpcEvent");
  fIntree->Add(fFile.Data());//before AddFile
  fIntree->SetBranchAddress("tpcEvent", &fEv);
  
  fCdcEv=      new TClonesArray("CdcEvent");
  fCdcTrack =  new TClonesArray("CdcTrack");
  fRpcTrack =  new TClonesArray("RpcTrack");
  fCdcHit=     new TClonesArray("CdcHit");  

  fCdcIntree = new TChain("cdcTree");
  fCdcIntree->Add(fCdcFile.Data());//before AddFile

  fCdcIntree->SetBranchAddress("CdcEventBr",&fCdcEv);
  fCdcIntree->SetBranchAddress("CdcTrackBr",&fCdcTrack);
  fCdcIntree->SetBranchAddress("RpcTrackBr",&fRpcTrack);
  fCdcIntree->SetBranchAddress("CdcHitBr",&fCdcHit);   
  
  fLoop=0;
  fCdcLoop=1;
       
  if(fIntree->IsZombie()) {
    std::cout << "PndTpcCdcDataReaderTask::Init data file/tree "<< fFile 
	      << " couldn't be loaded -> abort" <<std::endl;
    Error("PndTpcCdcDataReaderTask::Init","TPC data file not found");
    return kERROR;
  }else if(fCdcIntree->IsZombie()){
    std::cout << "PndTpcCdcDataReaderTask::Init data file/tree "<< fCdcFile 
	      << " couldn't be loaded -> abort" <<std::endl;
    Error("PndTpcCdcDataReaderTask::Init","FOPI data file not found");
    return kERROR;
  }
  std::cout<<"Number of Entries in TpcChain: "<<fIntree->GetEntries()<< std::endl;  
  std::cout<<"Number of Entries in CdcChain: "<<fCdcIntree->GetEntries()<< std::endl;
  if(nEvents==0)
    nEvents=fCdcIntree->GetEntries();
  // create and register output array
  fSampleOutArray = new TClonesArray("PndTpcSample");
  fEventIdOutArray = new TClonesArray("PndTpcEventIdentifier");
  fCdcEventOutArray = new TClonesArray("CdcEvent");
  fCdcHitOutArray   = new TClonesArray("CdcHit");
  fCdcTrackOutArray = new TClonesArray("CdcTrack");
  fRpcTrackOutArray = new TClonesArray("RpcTrack");

  ioman->Register("PndTpcSample","PndTpc",fSampleOutArray,_persistence);
  ioman->Register("PndTpcEventIdentifier","PndTpc", fEventIdOutArray, _persistence);
  ioman->Register("PndTpcCdcEvent","PndTpc", fCdcEventOutArray, _persistence);
  ioman->Register("PndTpcCdcHit","PndTpc", fCdcHitOutArray, _persistence);
  ioman->Register("PndTpcCdcTrack","PndTpc", fCdcTrackOutArray, _persistence);
  ioman->Register("PndTpcRpcTrack","PndTpc", fRpcTrackOutArray, _persistence);
  return kSUCCESS;
}

//EXEC
void PndTpcCdcDataReaderTask::Exec(Option_t* opt){
  std::cerr << "PndTpcCdcDataReaderTask::Exec" << std::endl;
  fIntree->AddFile(fFile.Data());
  fCdcIntree->AddFile(fCdcFile.Data());
  
  // Reset output Arrays
  if(fSampleOutArray==0) 
    Fatal("PndTpcCdcDataReaderTask::Exec()","No SampleOutArray");
  else if(fCdcEventOutArray==0)
    Fatal("PndTpcCdcDataReaderTask::Exec()","No CdcEventOutArray");
  else if(fCdcTrackOutArray==0)
    Fatal("PndTpcCdcDataReaderTask::Exec()","No CdcTrackOutArray");
  else if(fCdcHitOutArray==0)
    Fatal("PndTpcCdcDataReaderTask::Exec()","No CdcHitOutArray");
  else if(fRpcTrackOutArray==0)
    Fatal("PndTpcCdcDataReaderTask::Exec()","No RpcTrackOutArray");
  fSampleOutArray->Delete();
  fEventIdOutArray->Delete();
  fCdcEventOutArray->Delete();
  fCdcTrackOutArray->Delete();
  fRpcTrackOutArray->Delete();
  fCdcHitOutArray->Delete();

  
  const std::vector<PndTpcSample>* samples;
  
  while(fCdcLoop<fCdcIntree->GetEntries()&&fCdcLoop<nEvents) {
    fCdcIntree->GetEvent(fCdcLoop);
    fCdcLoop++;
    int cdcEvNb=((CdcEvent*)fCdcEv->At(0))->GetEvtNum();
    int cdcSpillNb=((CdcEvent*)fCdcEv->At(0))->GetSpillNum();
    int cdcRunNb=((CdcEvent*)fCdcEv->At(0))->GetRunNum();
    if(cdcRunNb==runNr){
      //std::cout << "RUN NUMBER DO MATCH! GO AHED" << std::end;
    }
    else{
      std::cerr << "RUN NUMBER DOES NOT MATCH!!! Bailing out!"<< std::endl;
      throw;
    }
    if(cdcSpillNb>2048){
      continue;
    }
    int loopSearch=0;
    std::cout<<"searching tpc for ev: "<<cdcEvNb<<" ,spill: "<<cdcSpillNb<<std::endl;
    while(loopSearch+fLoop<fIntree->GetEntries()&&loopSearch<20){
      
      //get the PndTpcEvent;    
      fIntree->GetEvent(loopSearch+fLoop);
      loopSearch++;
    
      fEventNb = fEv->getEventNb();
      fSpillNb = fEv->getSpillNb();
      std::cout<<"tpc ev: "<<fEventNb<<" ,spill: "<<fSpillNb<<std::endl;
      if(fEventNb==cdcEvNb&&fSpillNb==cdcSpillNb){
	fLoop=fLoop+loopSearch;
	samples = fEv->getEventVector();
	std :: cout << "Copying "<< samples->size()<<" samples." <<std::endl; 
	if(samples->size()<fCutoff)
	  continue;
	PndTpcEventIdentifier* id = new((*fEventIdOutArray)[0]) PndTpcEventIdentifier(fEventNb,
										      fSpillNb);
	CdcEvent* cdcev=(CdcEvent*)fCdcEv->At(0);
	CdcEvent* theEv= new ((*fCdcEventOutArray)[0]) CdcEvent(*cdcev);
	std :: cout << "Saving "<< samples->size()<<" samples" <<std::endl;  
	for(unsigned int i=0; i<samples->size(); i++){
	  PndTpcSample* theSample = new((*fSampleOutArray)[i]) PndTpcSample((*samples)[i]);
	}
	for(unsigned int i=0;i<fCdcTrack->GetEntries();++i){
	  CdcTrack* track=(CdcTrack*)fCdcTrack->At(i);
	  CdcTrack* theTrack= new ((*fCdcTrackOutArray)[i]) CdcTrack(*track);
	  RpcTrack* rpctr=(RpcTrack*)fRpcTrack->At(i);
	  RpcTrack* theRpcTr= new ((*fRpcTrackOutArray)[i]) RpcTrack(*rpctr);
	}
	for(unsigned int i=0;i<fCdcHit->GetEntries();++i){
	  CdcHit* hit=(CdcHit*)fCdcHit->At(i);
	  CdcHit* theHit= new ((*fCdcHitOutArray)[i]) CdcHit(*hit);
	}
      return;
      }
    }
    return;
  }
  return;
}
	


void
PndTpcCdcDataReaderTask::SetParContainers() {
  
  std::cout<<"PndTpcCdcDataReaderTask::SetParContainers"<<std::endl;
  std::cout.flush();
  
  // Get run and runtime database
  FairRun* run = FairRun::Instance();
  if ( ! run ) Fatal("SetParContainers", "No analysis run");
  
  FairRuntimeDb* db = run->GetRuntimeDb();
  if ( ! db ) Fatal("SetParContainers", "No runtime database");
  
  // Get PndTpc digitisation parameter container
  fPar= (PndTpcDigiPar*) db->getContainer("PndTpcDigiPar");
  if (! fPar ) Fatal("SetParContainers", "PndTpcDigiPar not found");
}

