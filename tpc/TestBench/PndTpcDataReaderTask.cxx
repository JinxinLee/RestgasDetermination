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
: _persistence(kFALSE),_smallpad(kFALSE),
  fMinDigis(0)
{
  _digiBranchName = "PndTpcSample";
  _file = "TBtest/run.root";
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

  fpadplane= fpar->getPadPlane();
 
  fIntree = new TChain("tpcEvent");
  fIntree->AddFile(_file.Data());
  fIntree->SetBranchAddress("tpcEvent", &fEv);
       
  if(fIntree->IsZombie()) {
    std::cout << "PndTpcDataReaderTask::Init data file/tree "<< _file 
	      << " couldn't be loaded -> abort" <<std::endl;
    Error("PndTpcDataReaderTask::Init","Data file not found");
    return kERROR;
  }

  std::cout<<"Number of Entries in Chain: "<<fIntree->GetEntries()<< std::endl;


  // create and register output array
  _sampleOutArray = new TClonesArray("PndTpcSample");
  ioman->Register("PndTpcSample","PndTpc",_sampleOutArray,_persistence);
  loop=0;
  _di=new std::vector<PndTpcSample*>;
  return kSUCCESS;
}

//EXEC
void PndTpcDataReaderTask::Exec(Option_t* opt)
{
  std::cout << "PndTpcDataReaderTask::Exec" << std::endl;

  fIntree->AddFile(_file.Data());
  // Reset output Arrays
  if(_sampleOutArray==0) 
    Fatal("PndTpcDataReaderTask::Exec()","No SampleOutArray");
  
  _sampleOutArray->Delete();
  
  std::vector<PndTpcSample> samples;

  McIdCollection * mcid = new McIdCollection();
  
  while(true) {
  
    //get the PndTpcEvent;    
    fIntree->GetEvent(loop);//Loop();
    loop++;
    samples = fEv->getEventVector();
  
    std :: cout << "Copying "<< samples.size()<<" samples." <<std::endl; 
  
    
    for (int i = 0;i<samples.size();i++)
      if (samples[i].padId()>0)
	try{
	  //cut smallpad:
	  if(_smallpad && fpadplane->GetPad(samples[i].padId())->y()<0.6) 
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
  
  std :: cout << "Saving "<< _di->size()<<" samples" <<std::endl;  
  
  for (int i = 0; i < _di->size();i++)
    { 
      PndTpcSample* dididi=new((*_sampleOutArray)[i]) PndTpcSample(*(_di->at(i)));
      //saved  
      delete _di->at(i);
    }
  
  _di->clear();
  
  delete mcid;
  
  return;
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
  fpar= (PndTpcDigiPar*) db->getContainer("PndTpcDigiPar");
  if (! fpar ) Fatal("SetParContainers", "PndTpcDigiPar not found");
}

