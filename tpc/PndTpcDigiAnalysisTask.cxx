//-----------------------------------------------------------
// File and Version Information:
// $Id$
//
// Description:
//      Implementation of class PndTpcDigiAnalysisTask
//      see PndTpcDigiAnalysisTask.hh for details
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
#include "PndTpcDigiAnalysisTask.h"

// C/C++ Headers ----------------------
#include <algorithm>
#include <iostream>

#include "TH1D.h"
#include "TH2D.h"

// Collaborating Class Headers --------
#include "FairRootManager.h"
#include "FairRun.h"
#include "FairRuntimeDb.h"
#include "TClonesArray.h"
#include "PndTpcDigi.h"
#include "PndTpcDigiPar.h"
#include "PndTpcDigiAge.h"
#include "PndTpcFrontend.h"
#include "PndTpcDigiMapper.h"


// Class Member definitions -----------


PndTpcDigiAnalysisTask::PndTpcDigiAnalysisTask()
  : FairTask("TPC DigiAnalysis"), _persistence(kFALSE)
 {
  _digiBranchName = "PndTpcDigi";
 }


PndTpcDigiAnalysisTask::~PndTpcDigiAnalysisTask()
{
}

InitStatus
PndTpcDigiAnalysisTask::Init()
{
  //Get ROOT Manager
  FairRootManager* ioman= FairRootManager::Instance();

  if(ioman==0)
    {
      Error("PndTpcDigiAnalysisTask::Init","RootManager not instantiated!");
      return kERROR;
    }
  
  // Get input collection
  _digiArray=(TClonesArray*) ioman->GetObject(_digiBranchName);
  
  if(_digiArray==0)
    {
      Error("PndTpcDigiAnalysisTask::Init","Digi-array not found!");
      return kERROR;
    }
  
  _hTimesOverThres=new TH1D("TimesOverThres","Total TimesOverThres",800,0,20000);
 _hContOT=new TH1D("ContOT","Continuous TimesOverThres",200,0,5000);
  _hTimesXY=new TH2D("TimesXY","TimesOverThres on Pads",420,-42.01,39.99,420,-42.01,39.99);

  _hTimesR=new TH2D("TimesR","TimesOverThres vs R",42,0,42,800,0,20000);
  _hContOTR=new TH2D("ContTimesR","Continuous TimesOverThres vs R",42,0,42,200,0,5000);
  _hDigiLengthR=new TH2D("DigiLengthR","DigiLength vs R",42,0,42,80,0,2000);
  _hDigiLengthXY=new TH2D("DigiLengthXY","DigiLength on Pads",420,-42.01,39.99,420,-42.01,39.99);


  _frontend= _par->getFrontend();

  _mapper=PndTpcDigiMapper::getInstance(0);

   _mapper->init(_par->getPadPlane(),
            _par->getGem(),
            _par->getGas(),
            _par->getZGem(),
            _frontend->t0(),
            _frontend->samplingFrequency());

  return kSUCCESS;
}


void
PndTpcDigiAnalysisTask::Exec(Option_t* opt)
{
  std::cout<<"PndTpcDigiAnalysisTask::Exec"<<std::endl;

  std::map<unsigned int, double> times;
  std::map<unsigned int, double> conttimes;
  std::map<unsigned int, int> lastdigi;
  
  int ndigi=_digiArray->GetEntriesFast();
  std::cout<<ndigi<<" Digis in event."<<std::endl;

  if(ndigi==0)return;


  double meanl=0;
  for(int i=0;i<ndigi;++i){
    PndTpcDigi* digi=(PndTpcDigi*)_digiArray->At(i);
    unsigned int padId=digi->padId();
    
    double dt=_frontend->dt();
    double l=digi->tlength()*dt;
    meanl+=l;
    times[padId]+=l;
    double x=_mapper->getPad(padId)->x();
    double y=_mapper->getPad(padId)->y();
    double r=sqrt(x*x+y*y);
    _hDigiLengthR->Fill(r,l);
    _hDigiLengthXY->Fill(x,y,l);

    // check if last contiuous pulse is over
    if(lastdigi[padId]!=0){
      PndTpcDigi* ldigi=(PndTpcDigi*)_digiArray->At(lastdigi[padId]-1);
      if(digi->t()*dt-0.3*l>ldigi->t()*dt+(0.7*ldigi->tlength()+1.0)*dt){
	_hContOT->Fill(conttimes[padId]);
	_hContOTR->Fill(r,conttimes[padId]);
	conttimes[padId]=0;
      }
    }
    
    lastdigi[padId]=i+1; // +1 to avoid putting 0
    conttimes[padId]+=l;
      
  } // end loop over digis
  
  std::cout<<times.size()<<" Pads with a digit."<<std::endl;
  


  double meantimeoverthres=0;
  std::map<unsigned int,double>::iterator it=times.begin();
  while(it!=times.end()){
    meantimeoverthres+=it->second;
    _hTimesOverThres->Fill(it->second);
    double x=_mapper->getPad(it->first)->x();
    double y=_mapper->getPad(it->first)->y();
    _hTimesXY->Fill(x,y,it->second);
    double r=sqrt(x*x+y*y);
    _hTimesR->Fill(r,it->second);
    ++it;
  }
  meantimeoverthres/=(double)times.size();

  std::cout<<"Mean time over threshold: "<<meantimeoverthres
	   <<" ns."<<std::endl;
  
  // fill remaining stuff
  double meanctimeot=0;
  std::map<unsigned int,double>::iterator itc=conttimes.begin();
  while(itc!=conttimes.end()){
    meanctimeot+=itc->second;
    _hContOT->Fill(itc->second);
    double x=_mapper->getPad(itc->first)->x();
    double y=_mapper->getPad(itc->first)->y();
    double r=sqrt(x*x+y*y);
    _hContOTR->Fill(r,itc->second);
    ++itc;
  }
  meanctimeot/=(double)conttimes.size();

  std::cout<<"Mean continuous time over threshold: "<<meanctimeot
	   <<" ns."<<std::endl;

  meanl/=(double)ndigi;
  std::cout<<"Mean time per digit: "<<meanl
	   <<" ns."<<std::endl;
  return;
}

void 
PndTpcDigiAnalysisTask::SetParContainers() {

  std::cout<<"PndTpcDigiAnalysisTask::SetParContainers"<<std::endl;
  std::cout.flush();

  // Get run and runtime database
  FairRun* run = FairRun::Instance();
  if ( ! run ) Fatal("SetParContainers", "No analysis run");

  FairRuntimeDb* db = run->GetRuntimeDb();
  if ( ! db ) Fatal("SetParContainers", "No runtime database");

  // Get PndTpc digitisation parameter container
  _par= (PndTpcDigiPar*) db->getContainer("PndTpcDigiPar");
  if (! _par ) Fatal("SetParContainers", "PndTpcDigiPar not found");
}


void
PndTpcDigiAnalysisTask::WriteHistos()
{
  TFile* outfile=FairRootManager::Instance()->GetOutFile();
  outfile->cd();
  _hTimesOverThres->Write();
  _hTimesXY->Write();
  _hTimesR->Write();
  _hDigiLengthR->Write();
  _hDigiLengthXY->Write();
  _hContOT->Write();
  _hContOTR->Write();
}

ClassImp(PndTpcDigiAnalysisTask)
