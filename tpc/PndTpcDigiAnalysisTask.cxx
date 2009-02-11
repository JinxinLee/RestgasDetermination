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
#include "TClonesArray.h"
#include "PndTpcDigi.h"
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
  if(_frontend!=0)delete _frontend;
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
  
  _hTimesOverThres=new TH1D("TimesOverThres","TimesOverThres",200,0,20000);
  _hTimesXY=new TH2D("TimesXY","TimesOverThres on Pads",420,-42.01,39.99,420,-42.01,39.99);
  _hTimesR=new TH2D("TimesR","TimesOverThres vs R",42,0,42,200,0,20000);

  _mapper=PndTpcDigiMapper::getInstance();

  return kSUCCESS;
}


void
PndTpcDigiAnalysisTask::Exec(Option_t* opt)
{
  std::cout<<"PndTpcDigiAnalysisTask::Exec"<<std::endl;

  std::map<unsigned int, double> times;
  
  int ndigi=_digiArray->GetEntriesFast();
  std::cout<<ndigi<<" Digis in event."<<std::endl;

  if(ndigi==0)return;

  for(int i=0;i<ndigi;++i){
    PndTpcDigi* digi=(PndTpcDigi*)_digiArray->At(i);
    unsigned int padId=digi->padId();
    times[padId]+=digi->tlength()*25.0; // TODO: replace this by sampling freq!
  }

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
  
  return;
}


void
PndTpcDigiAnalysisTask::WriteHistos()
{
  TFile* outfile=FairRootManager::Instance()->GetOutFile();
  outfile->cd();
  _hTimesOverThres->Write();
  _hTimesXY->Write();
  _hTimesR->Write();
}

ClassImp(PndTpcDigiAnalysisTask)
