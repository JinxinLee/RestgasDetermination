//-----------------------------------------------------------
// File and Version Information:
// $Id$
//
// Description:
//     Reader for t2k data for the gemTPCdecoding program
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
#include "PndTpcDataReaderTask.h"

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
 
using std::fabs;

// Class Member definitions -----------

ClassImp(PndTpcDataReaderTask)


PndTpcDataReaderTask::PndTpcDataReaderTask()
: _persistence(kFALSE),_cutsmallpad(kFALSE),_cutbigpad(kFALSE), fCutoff(0),
  fEventNb(0), fSpillNb(0), fEventId(NULL)
{
  _digiBranchName = "PndTpcSample";
  fFile = "TBtest/run.root";
  fEv=NULL; //not setting this to 0 gives a segfault. ROOT is fully awesome
}

PndTpcDataReaderTask::~PndTpcDataReaderTask()
{;}

InitStatus
PndTpcDataReaderTask::Init()
{
  //Get ROOT Manager
  FairRootManager* ioman= FairRootManager::Instance();
  
  if(ioman==0)
    {
      Error("PndTpcDataReaderTask::Init","RootManager not instantiated!");
      return kERROR;
    }
  
  // Get input data

  //fPadplane= fPar->getPadPlane();
 
  fIntree = new TChain("tpcEvent");
  fIntree->Add(fFile.Data());//before AddFile
  fIntree->SetBranchAddress("tpcEvent", &fEv);

  fLoop=0;
       
  if(fIntree->IsZombie()) {
    std::cout << "PndTpcDataReaderTask::Init data file/tree "<< fFile 
	      << " couldn't be loaded -> abort" <<std::endl;
    Error("PndTpcDataReaderTask::Init","Data file not found");
    return kERROR;
  }

  std::cout<<"Number of Entries in Chain: "<<fIntree->GetEntries()<< std::endl;
  
  // create and register output array
  _sampleOutArray = new TClonesArray("PndTpcSample");
  fEventIdOutArray = new TClonesArray("PndTpcEventIdentifier");
  ioman->Register("PndTpcSample","PndTpc",_sampleOutArray,_persistence);
  ioman->Register("PndTpcEventIdentifier","PndTpc", fEventIdOutArray, _persistence);
  return kSUCCESS;
}

//EXEC
void PndTpcDataReaderTask::Exec(Option_t* opt)
{
  std::cout << "PndTpcDataReaderTask::Exec" << std::endl;
  fIntree->AddFile(fFile.Data());
  
  // Reset output Arrays
  if(_sampleOutArray==0) 
    Fatal("PndTpcDataReaderTask::Exec()","No SampleOutArray");
  
  _sampleOutArray->Delete();
  fEventIdOutArray->Delete();
  
  const std::vector<PndTpcSample>* samples;
  
  //McIdCollection mcid = new McIdCollection();
  
  while(fLoop<fIntree->GetEntries()) {
  
    //get the PndTpcEvent;    
    fIntree->GetEvent(fLoop);
    fLoop++;
    samples = fEv->getEventVector();
    std :: cout << "Copying "<< samples->size()<<" samples." <<std::endl; 
    
    if(samples->size()<fCutoff)
      continue;
    
    fEventNb = fEv->getEventNb();
    fSpillNb = fEv->getSpillNb();
    PndTpcEventIdentifier* id = new((*fEventIdOutArray)[0]) PndTpcEventIdentifier(fEventNb,
										 fSpillNb);
    
    /*
      unsigned int badsample[fNbChip][fMaxSample];
      for (int i = 0;i<fCutOcc;i++)
      for (int j = 0;j<fMaxSample;j++)     
      badsample[i][j]=0;
      
      //FPN Noise Corr
      for (int i = 0;i<samples.size();i++)
      {
      badsample[samples[i].chipId()][samples[i].t()]++;
      }
      
      
      for (int i = 0;i<samples.size();i++)
      if (samples[i].t()>1 && samples[i].amp()>1 && badsample[samples[i].chipId()][samples[i].t()]<fCutOcc &&(samples[i].sourceId()==800||!fOnly800))
      if (samples[i].padId()>0)
      try{
      //cut smallpad:
      if(_cutsmallpad && fpadplane->GetPad(samples[i].padId())->y()>0.6) 
      continue;
      else if(_cutbigpad && fpadplane->GetPad(samples[i].padId())->y()<0.6) 
      continue;
      PndTpcSample * didi = new PndTpcSample(samples[i]);
      _di->push_back(didi);
      }
      catch(...){
      std::cout << "PndTpcDataReaderTask::Exec GetPad exception caught: pad"
      <<samples[i].padId()<< " used."<<std::endl;
      }
      if(_di->size()>=fMinDigis)
      break;
      samples.clear();
      _di->clear();
      }
    */
    
    std :: cout << "Saving "<< samples->size()<<" samples" <<std::endl;  
    
    for(unsigned int i=0; i<samples->size(); i++)
      PndTpcSample* theSample = new((*_sampleOutArray)[i]) PndTpcSample((*samples)[i]);
        
    return;
  }
}
	


void
PndTpcDataReaderTask::SetParContainers() {
  
  std::cout<<"PndTpcDataReaderTask::SetParContainers"<<std::endl;
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

