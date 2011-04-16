//-----------------------------------------------------------
// File and Version Information:
// $Id$
//
// Description:
//      Implementation of class PndTpcPSATask
//      see PndTpcPSATask.hh for details
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
#include "PndTpcPSATask.h"

// C/C++ Headers ----------------------
#include <algorithm>
#include <iostream>

// Collaborating Class Headers --------
#include "FairRootManager.h"
#include "TClonesArray.h"
#include "PndTpcSample.h"
#include "PndTpcDigi.h"
#include "PndTpcDigiAge.h"
#include "PndTpcSampleAge.h"
#include "PndTpcFrontend.h"
#include "PndTpcSimplePSAStrategy.h"
#include "PndTpcPSA_TOT1.h"
//#include "PndTpcPSA_AD1.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"
#include "PndTpcDigiPar.h"
#include "PndTpcT2KPulseshape.h"
#include "PndTpcCRRCPulseshape.h"
#include "PndTpcPadPlane.h"


// Class Member definitions -----------
void
PndTpcPSATask::SetParContainers() {

  std::cout<<"PndTpcPSATask::SetParContainers"<<std::endl;
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


PndTpcPSATask::PndTpcPSATask()
  : FairTask("TPC Pulse shape analyser"), fpersistence(kFALSE), fpeak(100), fopt(0),
    fPads(0)
{
  fsampleBranchName = "PndTpcSample";
  fDigiOutName = "PndTpcDigi";
}


PndTpcPSATask::~PndTpcPSATask()
{  
  if(ffrontend!=NULL)delete ffrontend;
  if(fpsa!=NULL)delete fpsa;
    
  clearSampleMap();
}

InitStatus
PndTpcPSATask::Init()
{
  //Get ROOT Manager
  FairRootManager* ioman= FairRootManager::Instance();
  fPadPlane = fpar->getPadPlane();
  fPads = fPadPlane->GetNPads();
  
  if(ioman==0)
    {
      Error("PndTpcPSATask::Init","RootManager not instantiated!");
      return kERROR;
    }
  
  // Get input collection
  fsampleArray=(TClonesArray*) ioman->GetObject(fsampleBranchName);
  
  if(fsampleArray==0)
    {
      Error("PndTpcPSATask::Init","Sample-array not found!");
      return kERROR;
    }
  
  // create and register output array
  fdigiArray = new TClonesArray("PndTpcDigi"); 
  ioman->Register(fDigiOutName,"PndTpc",fdigiArray,fpersistence);

  
  //TODO: Get this from Database
  //  ffrontend= new PndTpcFrontend(10,    // AdcThreshold
  //		     2048, // adcmax
  //		     11,     // adcbits
  //		     20,     // SamplingFreq_Mhz
  //		     0,      // t0
  //			     16,     // timebits
  //		     10);     // PSAthreshold

  ffrontend = (PndTpcFrontend*) fpar->getFrontend();
  //fpulseshape= new PndTpcT2KPulseshape(fpeak);
  fpulseshape= new PndTpcCRRCPulseshape(ffrontend->tdiff(),
                                        ffrontend->tint(),
                                        ffrontend->tsig());

   if( fpar->getPSA() == 0) {
  	fpsa= new PndTpcSimplePSAStrategy(ffrontend->psaThreshold());
	std::cout << "Using Simple PSA strategy!" << std::endl;
   }
   else if( fpar->getPSA() == 1){
     fpsa= new PndTpcPSA_TOT1();
     std::cout << "Using PSA_TOT strategy!";
     if (fopt) {fpsa->setOpt(fopt); std::cout << " with " << fopt << "nb of empty samples allowed.";}
     std::cout << std::endl;
   }
   /*
   else if( fpar->getPSA() == 2){
     fpsa= new PndTpcPSA_AD1();//fpulseshape);
     fpsa->setPs(fpulseshape);
     fpsa->TailCancellation(fTail);
     fpsa->setOpt((unsigned int)ffrontend->samplingFrequency());
     std::cout << "Using PSA_AD strategy!" << std::endl;
   } 
   */
   else return kERROR;
   return kSUCCESS;
}


void
PndTpcPSATask::Exec(Option_t* opt)
{
  clearSampleMap();
  std::cout<<"PndTpcPSATask::Exec"<<std::endl;
  // Reset output Array
  if(fdigiArray==0) Fatal("PndTpcPSA::Exec)","No DigiArray");
  fdigiArray->Delete();
  // delete Samples created by PresetNullSample
  while(!nullSamples.empty()) delete nullSamples.back(), nullSamples.pop_back();


  // Sort samples according to padid (create several vectors of samples)
  Int_t ns=fsampleArray->GetEntriesFast();
  for(Int_t is=0;is<ns;++is){
    PndTpcSample* mysample=(PndTpcSample*)fsampleArray->At(is);
    unsigned int id=mysample->padId();
    if(id<0 || id>fPads)
      continue;
    if(fSampleMap[id]==NULL)fSampleMap[id]=new std::vector<PndTpcSample*>;
    fSampleMap[id]->push_back(mysample);
  }
  std::cout<< "Found " << fSampleMap.size() << " hit pads" << std::endl;

  // output vector
  std::vector <PndTpcDigi*> digis;
  // loop over pads
  std::map<unsigned int,std::vector<PndTpcSample*>* >::iterator padit=fSampleMap.begin();
  while(padit!=fSampleMap.end()){
    
    // get vector of samples in first pad
    std::vector<PndTpcSample*>* vecSa=padit->second;
    // sort samples in time
    sort(vecSa->begin(),vecSa->end(),PndTpcSampleAge());
    // pass samplelist to psa
    unsigned int ndigibefore=digis.size();
    if(vecSa->size()!=0)
      {
        PresetNullSample(vecSa);
        //std::cout << "Pad " << padit->first
        //	  << ": Processing " << vecSa->size() << " samples with PSA (th";
        fpsa->Process(*vecSa,digis,ffrontend->psaThreshold());
        //unsigned int ndigiafter=digis.size();
        //std::cout <<ffrontend->psaThreshold() << ") -- " 
        //	  << ndigiafter-ndigibefore << " Digis created."<<std::endl;
      }
    
    ++padit; // increase iterator 
  } // end loop over pads
  
  std::cout << "Total number of digis created: " << digis.size() << std::endl;
  
  //	    digi->fct(((PndTpcSample*)fsampleArray->At(is))->ct());
  
  //sort digis in time;
  std::cout<<"sort Digis in time ... ";
  sort(digis.begin(),digis.end(),PndTpcDigiAge());
  std::cout<<"finished"<<std::endl;
  
  //copy data into digi_array (TClonesvector)
  int ndigi=digis.size();
  for(int idigi=0;idigi<ndigi;++idigi) {
    new((*fdigiArray)[idigi]) PndTpcDigi(*(digis[idigi]));
    delete digis[idigi]; // clean up temporay store
  }

  std::cout<<fdigiArray->GetEntriesFast()<<" Digis created"<<std::endl;
  return;
}


void  PndTpcPSATask::PresetNullSample(std::vector<PndTpcSample*> *samplelist)
{
	//set a zero sample before the first sample in the vector, so that the alternative psa will always 	
    	//find the first maximum 
	if(samplelist->size()>0)	{	
		PndTpcSample *pFirstSample=*(samplelist->begin());
		int t=pFirstSample->t();
		unsigned int PadID=pFirstSample->padId();
		McIdCollection m=pFirstSample->mcId();
		PndTpcSample *pS=new PndTpcSample(t-1,0,PadID,m);
		samplelist->insert(samplelist->begin(),pS);
    nullSamples.push_back(pS);
  }
}


void PndTpcPSATask::clearSampleMap(){
  //delete vectors (but not PndTpcSamples) and clear sampleMaps
  std::map<unsigned int,std::vector<PndTpcSample*>* >::iterator secIt=fSampleMap.begin();
  while(secIt!=fSampleMap.end()){
    (secIt->second)->clear();
    delete secIt->second;
    ++secIt;
  }
  fSampleMap.clear();
}



ClassImp(PndTpcPSATask)
