//-----------------------------------------------------------
// File and Version Information:
// $Id$
//
// Description:
//      Implementation of class PndTpcPadResponseTask
//      see PndTpcPadResponseTask.hh for details
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
#include "PndTpcPadResponseTask.h"

// C/C++ Headers ----------------------


// Collaborating Class Headers --------
#include "CbmRootManager.h"
#include "CbmRunAna.h"
#include "CbmRuntimeDb.h"
#include "PndTpcDigiPar.h"
#include "TClonesArray.h"
#include "TRandom.h"
#include "PndTpcSignal.h"
#include "PndTpcAvalanche.h"
#include "PndTpcPad.h"
#include "PndTpcPadPlane.h"
#include "PndTpcGem.h"
#include "QAPlotCollection.h"

#include <iostream>

// Class Member definitions -----------


PndTpcPadResponseTask::PndTpcPadResponseTask()
  : CbmTask("TPC PadResponse"), _persistence(kFALSE), _minSignalAmp(0),
    _rmin(15.), _rmax(42.), _selected(false), _initialized(kFALSE), _qa(NULL)
{
  _avalancheBranchName = "PndTpcAvalanche";
}


PndTpcPadResponseTask::~PndTpcPadResponseTask()
{}

void
PndTpcPadResponseTask::SetParContainers() {

  std::cout<<"PndTpcPadResponseTask::SetParContainers"<<std::endl;
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
PndTpcPadResponseTask::Init()
{
  _initialized=false;

  //Get ROOT Manager
  CbmRootManager* ioman= CbmRootManager::Instance();

  if(ioman==0)
    {
      Error("PndTpcPadResponseTask::Init","RootManager not instantiated!");
      return kERROR;
    }
  
  // Get input collection
  _avalancheArray=(TClonesArray*) ioman->GetObject(_avalancheBranchName);
  
  if(_avalancheArray==0)
    {
      Error("PndTpcPadResponseTask::Init","Avalanche-array not found!");
      return kERROR;
    }
  
  // create and register output array
  _signalArray = new TClonesArray("PndTpcSignal"); 
  ioman->Register("PndTpcSignal","PndTpc",_signalArray,_persistence);

  
  _padPlane= _par->getPadPlane();
  std::cout<<*_padPlane<<std::endl;
  
  _gem=_par->getGem();

  _rmin=_par->getRMin();
  _rmax=_par->getRMax();
  _minSignalAmp=_par->getMinSigAmp();

  _eventcounter=0;

  //Create Histograms for recording spread of Avalanche-centre to hit Pads
  if(_qa!=NULL){
	  _xVariation = _qa->getTH1D("xSpread","Hit Pad x-coordinates around Avalanche centre", 
			 200, -0.3, 0.3);
	  _yVariation = _qa->getTH1D("ySpread", 
			 "Hit Pad y-coordinates around Avalanche centre", 
			 200, -0.3, 0.3);
	  _2DHisto = _qa->getTH2D("xySpread","xSpread vs ySpread",
		      200,-0.3,0.3,200,-0.3,0.3);
  }
  _selected=_secids.size()>0;

  _initialized=true;
  return kSUCCESS;
}


void
PndTpcPadResponseTask::Exec(Option_t* opt)
{
  // Reset output Array
  if(_signalArray==0) Fatal("PndTpcPadResponse::Exec)","No SignalArray");
  _signalArray->Delete(); // Note: signals will be cleand up in the adc!
  int nValidHits=0;
  Int_t na=_avalancheArray->GetEntriesFast();
  for(Int_t ia=0;ia<na;++ia){
    PndTpcAvalanche* Aval=(PndTpcAvalanche*)_avalancheArray->At(ia);

    std::vector<PndTpcPad*> hitPads;
    double xAv=Aval->x();
    double yAv=Aval->y();
    double r=sqrt(xAv*xAv+yAv*yAv);
    if(r<_rmin || r>_rmax) {
      Warning("Exec","Avalanche outside of allowed region! r=%f",r);
      continue;
    }
    _padPlane->GetPadList(xAv,yAv,
                         _gem->spread()*3,
                         hitPads);
    // Build Signals
    int nHits=hitPads.size();
    //std::cout<<nHits<<" Pads hit by avalanche"<<std::endl;

    for(int iHit=0; iHit<nHits; ++iHit){
      PndTpcPad* pad=hitPads[iHit];
      if(_selected){
		// find sectorid
		std::map<unsigned int,bool>::iterator it=_secids.find(pad->sectorId());
		if(it==_secids.end())continue;
      }
      double Amp=Aval->amp()*pad->GetValue(xAv,yAv);
      if(_par->getGaussianNoise()){
	Amp+=gRandom->Gaus(0.,_par->getGaussianNoiseAmp());
      }
      //Fill Histograms with these Values

      FillHistograms(xAv, yAv, pad->x(), pad->y() );      

      // cut on amplitude
      if(Amp>=_minSignalAmp){
        PndTpcSignal* sig=new((*_signalArray)[nValidHits]) PndTpcSignal(Aval->t(),Amp,
                                     pad->id(),0,
                                     0);
        sig->setmcTrackId(Aval->mcTrackId());
        sig->setmcHitId(Aval->mcHitId());
        ++nValidHits;
      }
      else {
	//std::cout<<"PadAmp too small! Amp="<<Amp<<"<"<<_minSignalAmp<<std::endl;
      }
    } // end loop over hit pads

  } // end loop over avalanches

  ++_eventcounter;
  std::cout<<_signalArray->GetEntriesFast()<<" Signals created"<<std::endl;

  return;
}

void PndTpcPadResponseTask::FillHistograms(double xAv, double yAv,
					double xPad, double yPad) {
  if(_qa!=NULL){
	  _xVariation->Fill(xAv - xPad);
	  _yVariation->Fill(yAv - yPad);
	  _2DHisto->Fill(xAv - xPad, yAv - yPad);
  }
}

//!Method WriteHistograms() has to be called once in the runDigi.C macro!
//! This method is deprecated - replaced by the QAPlotcollection!
void PndTpcPadResponseTask::WriteHistograms() {

  if(!_initialized || _qa==NULL)return;
  TFile* file = CbmRootManager::Instance()->GetOutFile();
  file->mkdir("PndTpcPadResponse");
  file->cd("PndTpcPadResponse");

  _xVariation->Write();
  _yVariation->Write();
  _2DHisto->Write();

}


ClassImp(PndTpcPadResponseTask)
