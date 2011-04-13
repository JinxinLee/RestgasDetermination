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
#include "FairRootManager.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"
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
#include "PndDetectorList.h"
#include "McId.h"
#include "QAPlotCollection.h"


// Class Member definitions -----------


PndTpcElectronicsTask::PndTpcElectronicsTask()
  : FairTask("TPC Electronics response"), fpersistence(kFALSE),fsamplePersistence(kFALSE), finitialized(kFALSE), fqa(NULL)
 {
  fsignalBranchName = "PndTpcSignal";
 }


PndTpcElectronicsTask::~PndTpcElectronicsTask()
{
  if(fpsa!=0)delete fpsa;
}


void
PndTpcElectronicsTask::SetParContainers() {

  std::cout<<"PndTpcElectronicsTask::SetParContainers"<<std::endl;
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
PndTpcElectronicsTask::Init()
{
	
	finitialized=kFALSE;
  //Get ROOT Manager
  FairRootManager* ioman= FairRootManager::Instance();

  if(ioman==0)
    {
      Error("PndTpcElectronicsTask::Init","RootManager not instantiated!");
      return kERROR;
    }
  
  // Get input collection
  fsignalArray=(TClonesArray*) ioman->GetObject(fsignalBranchName);
  
  if(fsignalArray==0)
    {
      Error("PndTpcElectronicsTask::Init","Signal-array not found!");
      return kERROR;
    }
  
  // create and register output array
  fdigiArray = new TClonesArray("PndTpcDigi"); 
  ioman->Register("PndTpcDigi","PndTpc",fdigiArray,fpersistence);
  
  fsampleArray = new TClonesArray("PndTpcSample");
  ioman->Register("PndTpcSample","PndTpc",fsampleArray,fsamplePersistence);

  
  //TODO: Get this from Database!
  ffrontend= fpar->getFrontend();

  fpulseshape= new PndTpcCRRCPulseshape(ffrontend->tdiff(),
				     ffrontend->tint(),
				     ffrontend->tsig());
					
   if( fpar->getPSA() == 0)	{
  	fpsa= new PndTpcSimplePSAStrategy(ffrontend->psaThreshold());
	std::cout << "Using Simple PSA strategy!" << std::endl;
   }
   else if( fpar->getPSA() == 1)	{
  	fpsa= new PndTpcPSA_TOT1();
	std::cout << "Using PSA_TOT strategy!" << std::endl;
  }
  else	{
  	assert(0);
  }

  // check in QA plots
  if(fqa!=NULL){
	  double maxSamp=ffrontend->adcmax()/ffrontend->adcstep();
	  fsampleAmpH=fqa->getTH1D("SamplAmp","Sample amplitudes",1000,0,maxSamp);
	  fmeanDigiTH=fqa->getTH1D("MeanDigiT","Mean Digi t - Mean Signal t",1000,-20,20);
  }
  
  finitialized=kTRUE;
  return kSUCCESS;
}


void
PndTpcElectronicsTask::Exec(Option_t* opt)
{
  std::cout<<"PndTpcElectronicsTask::Exec"<<std::endl;
  // Reset output Array
  if(fdigiArray==0) Fatal("PndTpcElectronics::Exec)","No DigiArray");
  fdigiArray->Delete();
  if(fsamplePersistence)	{
	if(fsampleArray==0) Fatal("PndTpcElectronics::Exec)","No SampleArray");
	fsampleArray->Delete();
   }
	
  //partition data according to pads
  std::cout<<"Building up padmap ...";
  std::map<unsigned int,std::vector<PndTpcSignal*> > padmap;
  std::map<unsigned int, std::vector<int> > sigIdMap;

  Int_t ns=fsignalArray->GetEntriesFast();
  for(Int_t is=0;is<ns;++is){
    PndTpcSignal* sig=(PndTpcSignal*)fsignalArray->At(is);
    unsigned int id=sig->padId();
    //if(padmap[id]==NULL)padmap[id]=new std::vector<PndTpcSignal*>;
    padmap[id].push_back(sig);
    sigIdMap[id].push_back(is);
  }
  std::cout<<"finished. "<<padmap.size()<<" pads hit"<<std::endl;
  int tenpercent=padmap.size()/10;
  if(tenpercent==0)tenpercent=1;
  // cleanup fsignal_array;
  //fsignalArray->Clear();

  // process each pad individually:
  std::map<unsigned int,std::vector<PndTpcSignal*> >::iterator padIt=padmap.begin();
  int counter=0;
  while(padIt!=padmap.end()){
    if(counter++%tenpercent==0){
      std::cout<<".";
      std::cout.flush();
    }
    // --- ADC ------------------------------------------
    std::vector<PndTpcSignal*>* sv=&padIt->second;
    std::vector<PndTpcSample*> samplelist;
    PndTpcDigitizationPolicy().Digitize(sv,&samplelist,ffrontend,fpulseshape);

    PresetNullSample(&samplelist);
    if(fsamplePersistence)	{
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
    fpsa->Process(samplelist,digis,ffrontend->psaThreshold());



    int nsamp=samplelist.size();
    int ndigi=digis.size();

    if(fqa!=NULL){
    	for(int i=0;i<nsamp;++i){
    		fsampleAmpH->Fill(samplelist[i]->amp());
    	}
    }
	/*
	for(int i=0;i<digis.size();i++) {
	  std::cout << digis[i]->amp() << " " << digis[i]->t() << " " << digis[i]->padId() << std::endl;
	}
	



	for(int iSig=0;iSig<nsig;iSig++) {
	  std::cout << ((*sv)[iSig])->t() << " " 
				<< ffrontend->ClockFine( ((*sv)[iSig])->t() ) 
				<< std::endl;
	}


	
	if(nsamp>0) {
	  PndTpcPSAplot myplot(&samplelist,&digis,sv,ffrontend);
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
    int iarray=fdigiArray->GetEntriesFast();
    for(int idigi=0;idigi<ndigi;++idigi) {
      PndTpcDigi* dig=new((*fdigiArray)[iarray+idigi]) PndTpcDigi(*(digis[idigi]));
      dig->SetLinks(FairMultiLinkedData(fsignalBranchName, sigIdMap[padIt->first]));
      dig->index(iarray+idigi);
      delete digis[idigi]; // clean up temporay store
      meandigit=dig->t();
    }
    if(ndigi>0){
    	meandigit/=ndigi;
    	double dt=meandigit-meansigt;
    	if(fqa!=0)fmeanDigiTH->Fill(dt);
    }
    
    ++padIt;
  } // end loop over hit pads
  
  std::cout<<std::endl<<fdigiArray->GetEntriesFast()<<" Digis created"<<std::endl;
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
	int iarray=fsampleArray->GetEntriesFast();
	for(int i=0;i<samplelist->size();i++) {
			new((*fsampleArray)[iarray+i]) PndTpcSample(*((*samplelist)[i]));
	}
}


void 
PndTpcElectronicsTask::WriteHistograms(){
	if(!finitialized || fqa==NULL)return;
	TFile* outfile=FairRootManager::Instance()->GetOutFile();
	
	outfile->mkdir("PndTpcElectronicsTask");
	outfile->cd("PndTpcElectronicsTask");
	
	fsampleAmpH->Write();
	
}

ClassImp(PndTpcElectronicsTask)
