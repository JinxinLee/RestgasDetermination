//-----------------------------------------------------------
// File and Version Information:
// $Id$
//
// Description:
//     Task for the test chamber crosstalk suppression 
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
#include "PndTpcTCcrossTalkTask.h"

// C/C++ Headers ----------------------
#include <vector>
#include <map>
#include <algorithm>
#include <cmath>

// Collaborating Class Headers --------
#include "FairRootManager.h"
#include "TClonesArray.h"
#include "PndTpcSample.h"

#include "PndTpcTCcrossTalk.h"

#include "FairRunAna.h"
#include "FairRuntimeDb.h"
 
using std::fabs;

// Class Member definitions -----------

ClassImp(PndTpcTCcrossTalkTask) 


  PndTpcTCcrossTalkTask::PndTpcTCcrossTalkTask(): _persistence(kFALSE), fqa(NULL), _calc(kFALSE)
{
  _sampleBranchName = "PndTpcSample";
  _mode = 1;//voting is default
}

PndTpcTCcrossTalkTask::~PndTpcTCcrossTalkTask()
{}

InitStatus
PndTpcTCcrossTalkTask::Init()
{
  //Get ROOT Manager
  FairRootManager* ioman= FairRootManager::Instance();
   
  if(ioman==0)
    {
      Error("PndTpcTCcrossTalkTask::Init","RootManager not instantiated!");
      return kERROR;
    }
  
  //init crosstalk algo
  TCcross = new PndTpcTCcrossTalk();
  TCcross->SetQAPlotCol(fqa);
  TCcross->Init();
  
  // Get input collection
  _sampleInArray=(TClonesArray*) ioman->GetObject(_sampleBranchName);
  
  if(_sampleInArray==0)
    {
      Error("PndTpcTCcrossTalkTask::Init","Sample-array not found!");
      return kERROR;
    }
  
  // create and register output array
  _sampleOutArray = new TClonesArray("PndTpcSample");
  ioman->Register("PndTpcSampleCorr","PndTpc",_sampleOutArray,_persistence);
  
  _di=new std::vector<PndTpcSample*>;
  return kSUCCESS;
}

//EXEC
void PndTpcTCcrossTalkTask::Exec(Option_t* opt)
{
  std::cout << "PndTpcTCcrossTalkTask::Exec" << std::endl;
  
  
  _di->clear();
  
  // Reset output Arrays
  if(_sampleOutArray==0) Fatal("PndTpcTCcrossTalkTask::Exec)","No SampleOutArray");
  _sampleOutArray->Delete();
  
  // copy into vector
  unsigned int n=_sampleInArray->GetEntriesFast();
  for(unsigned int i=0;i<n;++i){
    PndTpcSample* cl=((PndTpcSample*) _sampleInArray->At(i));
    _di->push_back(cl);
  }
  
  if (_di->size()>1)
    {
      std::cout << "TCcrosstalk called with " <<_di->size()<<" samples"<< std::endl;
      if(_calc)
	{
	  TCcross->CalcAmp(_di);
	  TCcross->Fill();
	}

      switch(_mode)
	{
	case 0:   TCcross->ProcessCut(_di); break;
	case 1:   TCcross->Process(_di); break;
	case 2:;break;
	}//end swtich
    }
  
  for (int i = 0; i < _di->size();i++)
    { 
      PndTpcSample* dididi=new((*_sampleOutArray)[i]) PndTpcSample(*(*_di)[i]);//saved  
      // delete (*_di)[i];
    }
  

}
