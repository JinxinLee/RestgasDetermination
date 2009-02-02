//-----------------------------------------------------------
// File and Version Information:
// $Id$
//
// Description:
//      Implementation of class PndTpcElectronicsTask
//      see PndTpcElectronicsTask.hh for details
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
#include "PndTpcElectronicsTask.h"

// C/C++ Headers ----------------------
#include <algorithm>
#include <iostream>
#include "assert.h"

#include "TGraph.h"
#include "TROOT.h"
#include "TSystem.h"
#include "TCanvas.h"
#include "TLine.h"
#include "TApplication.h"
#include "TH1D.h"

// Collaborating Class Headers --------
#include "CbmRootManager.h"
#include "CbmRunAna.h"
#include "CbmRuntimeDb.h"
#include "PndTpcDigiPar.h"
#include "TClonesArray.h"
#include "PndTpcSignal.h"
#include "PndTpcSample.h"
#include "PndTpcDigi.h"
#include "PndTpcDigiAge.h"
#include "PndTpcFrontend.h"
#include "PndTpcSimplePSAStrategy.h"
#include "PndTpcPSA_TOT1.h"
#include "PndTpcDigitizationPolicy.h"
#include "PndTpcCRRCPulseshape.h"
#include "PndTpcPSAplot.h"
#include "PndTpcPSA_TOT1.h"
#include "McId.h"
#include "QAPlotCollection.h"


// Class Member definitions -----------


PndTpcElectronicsTask::PndTpcElectronicsTask()
  : CbmTask("TPC Electronics response"), _persistence(kFALSE),_samplePersistence(kFALSE), _initialized(kFALSE), _qa(NULL)
 {
  _signalBranchName = "PndTpcSignal";
 }


PndTpcElectronicsTask::~PndTpcElectronicsTask()
{
  if(_psa!=0)delete _psa;
}


void
PndTpcElectronicsTask::SetParContainers() {

  std::cout<<"PndTpcElectronicsTask::SetParContainers"<<std::endl;
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
PndTpcElectronicsTask::Init()
{
	
	_initialized=kFALSE;
  //Get ROOT Manager
  CbmRootManager* ioman= CbmRootManager::Instance();

  if(ioman==0)
    {
      Error("PndTpcElectronicsTask::Init","RootManager not instantiated!");
      return kERROR;
    }
  
  // Get input collection
  _signalArray=(TClonesArray*) ioman->GetObject(_signalBranchName);
  
  if(_signalArray==0)
    {
      Error("PndTpcElectronicsTask::Init","Signal-array not found!");
      return kERROR;
    }
  
  // create and register output array
  _digiArray = new TClonesArray("PndTpcDigi"); 
  ioman->Register("PndTpcDigi","PndTpc",_digiArray,_persistence);
  
  _sampleArray = new TClonesArray("PndTpcSample");
  ioman->Register("PndTpcSample","PndTpc",_sampleArray,_samplePersistence);

  
  //TODO: Get this from Database!
  _frontend= _par->getFrontend();

  _pulseshape= new PndTpcCRRCPulseshape(_frontend->tdiff(),
				     _frontend->tint(),
				     _frontend->tsig());
					
   if( _par->getPSA() == 0)	{
  	_psa= new PndTpcSimplePSAStrategy(_frontend->psaThreshold());
	std::cout << "Using Simple PSA strategy!" << std::endl;
   }
   else if( _par->getPSA() == 1)	{
  	_psa= new PndTpcPSA_TOT1();
	std::cout << "Using PSA_TOT strategy!" << std::endl;
  }
  else	{
  	assert(0);
  }

  // check in QA plots
  if(_qa!=NULL){
	  double maxSamp=_frontend->adcmax()/_frontend->adcstep();
	  _sampleAmpH=_qa->getTH1D("SamplAmp","Sample amplitudes",1000,0,maxSamp);
	  _meanDigiTH=_qa->getTH1D("MeanDigiT","Mean Digi t - Mean Signal t",1000,-20,20);
  }
  
  _initialized=kTRUE;
  return kSUCCESS;
}


void
PndTpcElectronicsTask::Exec(Option_t* opt)
{
  std::cout<<"PndTpcElectronicsTask::Exec"<<std::endl;
  // Reset output Array
  if(_digiArray==0) Fatal("PndTpcElectronics::Exec)","No DigiArray");
  _digiArray->Delete();
  if(_samplePersistence)	{
	if(_sampleArray==0) Fatal("PndTpcElectronics::Exec)","No SampleArray");
		_sampleArray->Delete();
   }
	
  //partition data according to pads
  std::cout<<"Building up padmap ...";
  std::map<unsigned int,std::vector<PndTpcSignal*>* > padmap;
  Int_t ns=_signalArray->GetEntriesFast();
  for(Int_t is=0;is<ns;++is){
    PndTpcSignal* sig=(PndTpcSignal*)_signalArray->At(is);
    unsigned int id=sig->padId();
    if(padmap[id]==NULL)padmap[id]=new std::vector<PndTpcSignal*>;
    padmap[id]->push_back(sig);
  }
  std::cout<<"finished. "<<padmap.size()<<" pads hit"<<std::endl;
  // cleanup _signal_array;
  //_signalArray->Clear();

  // process each pad individually:
  std::map<unsigned int,std::vector<PndTpcSignal*>* >::iterator padIt=padmap.begin();
  int counter=0;
  while(padIt!=padmap.end()){
    if(counter++%5000==0){
      std::cout<<".";
      std::cout.flush();
    }
    // --- ADC ------------------------------------------
    std::vector<PndTpcSignal*>* sv=padIt->second;
    std::vector<PndTpcSample*> samplelist;
    PndTpcDigitizationPolicy().Digitize(sv,&samplelist,_frontend,_pulseshape);

    PresetNullSample(&samplelist);
    if(_samplePersistence)	{
    		StoreSamples(&samplelist);
    }

    
    // make some statistics of signals
    int nsig=sv->size();
    double meansigt=0;
    for(int i=0;i<nsig;++i){
    	meansigt+=(*sv)[i]->t();
    }
    if(nsig>0)meansigt/=nsig;
    
    //for(int isig=0; isig<nsig;++isig)delete(*sv)[isig];
	//    sv->clear();
   // std::cout<<samplelist.size()<<" samples created!"<<std::endl;

    // --- PSA ------------------------------------------
    std::vector<PndTpcDigi*> digis;
    _psa->Process(samplelist,digis,_frontend->psaThreshold());



    int nsamp=samplelist.size();
    int ndigi=digis.size();

    if(_qa!=NULL){
    	for(int i=0;i<nsamp;++i){
    		_sampleAmpH->Fill(samplelist[i]->amp());
    	}
    }
	/*
	for(int i=0;i<digis.size();i++) {
	  std::cout << digis[i]->amp() << " " << digis[i]->t() << " " << digis[i]->padId() << std::endl;
	}
	



	for(int iSig=0;iSig<nsig;iSig++) {
	  std::cout << ((*sv)[iSig])->t() << " " 
				<< _frontend->ClockFine( ((*sv)[iSig])->t() ) 
				<< std::endl;
	}


	
	if(nsamp>0) {
	  PndTpcPSAplot myplot(&samplelist,&digis,sv,_frontend);
	  myplot.Draw();
	}
	*/
	  
    //
    
    
    sv->clear();
    // cleanup samples

    for(int isamp=0;isamp<nsamp;++isamp)delete samplelist[isamp];
    samplelist.clear();

    //copy data into digi_array (TClonesvector)
	//    int ndigi=digis.size();
    double meandigit=0;
    int iarray=_digiArray->GetEntriesFast();
    for(int idigi=0;idigi<ndigi;++idigi) {
      PndTpcDigi* dig=new((*_digiArray)[iarray+idigi]) PndTpcDigi(*(digis[idigi]));
      delete digis[idigi]; // clean up temporay store
      meandigit=dig->t();
    }
    if(ndigi>0){
    	meandigit/=ndigi;
    	double dt=meandigit-meansigt;
    	if(_qa!=0)_meanDigiTH->Fill(dt);
    }
    
    ++padIt;
  } // end loop over hit pads
  
  std::cout<<std::endl<<_digiArray->GetEntriesFast()<<" Digis created"<<std::endl;
  return;
}

void  PndTpcElectronicsTask::PresetNullSample(std::vector<PndTpcSample*> *samplelist)
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
    }
}


void PndTpcElectronicsTask::StoreSamples(std::vector<PndTpcSample*> *samplelist)
{
	int iarray=_sampleArray->GetEntriesFast();
	for(int i=0;i<samplelist->size();i++) {
			new((*_sampleArray)[iarray+i]) PndTpcSample(*((*samplelist)[i]));
	}
}


void 
PndTpcElectronicsTask::WriteHistograms(){
	if(!_initialized || _qa==NULL)return;
	TFile* outfile=CbmRootManager::Instance()->GetOutFile();
	
	outfile->mkdir("PndTpcElectronicsTask");
	outfile->cd("PndTpcElectronicsTask");
	
	_sampleAmpH->Write();
	
}

ClassImp(PndTpcElectronicsTask)
