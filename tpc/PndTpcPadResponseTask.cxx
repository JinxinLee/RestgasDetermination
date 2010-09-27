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
#include "FairRootManager.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"
#include "PndTpcDigiPar.h"
#include "TClonesArray.h"
#include "TRandom.h"
#include "PndTpcSignal.h"
#include "PndTpcAvalanche.h"
#include "PndTpcPad.h"
#include "PndTpcPadPlane.h"
#include "PndTpcGem.h"
#include "PndDetectorList.h"
#include "QAPlotCollection.h"

#include <iostream>

// Class Member definitions -----------


PndTpcPadResponseTask::PndTpcPadResponseTask()
  : FairTask("TPC PadResponse"), fpersistence(kFALSE), fminSignalAmp(0),
    frmin(15.5), frmax(41.5), fselected(false), finitialized(kFALSE), fqa(NULL)
    //TODO: parameter management
{
  favalancheBranchName = "PndTpcAvalanche";
}


PndTpcPadResponseTask::~PndTpcPadResponseTask()
{}

void
PndTpcPadResponseTask::SetParContainers() {

  std::cout<<"PndTpcPadResponseTask::SetParContainers"<<std::endl;
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
PndTpcPadResponseTask::Init()
{
  finitialized=false;

  //Get ROOT Manager
  FairRootManager* ioman= FairRootManager::Instance();

  if(ioman==0)
    {
      Error("PndTpcPadResponseTask::Init","RootManager not instantiated!");
      return kERROR;
    }
  
  // Get input collection
  favalancheArray=(TClonesArray*) ioman->GetObject(favalancheBranchName);
  
  if(favalancheArray==0)
    {
      Error("PndTpcPadResponseTask::Init","Avalanche-array not found!");
      return kERROR;
    }
  
  // create and register output array
  fsignalArray = new TClonesArray("PndTpcSignal"); 
  ioman->Register("PndTpcSignal","PndTpc",fsignalArray,fpersistence);

  
  fpadPlane= fpar->getPadPlane();
  std::cout<<*fpadPlane<<std::endl;
  
  fgem=fpar->getGem();

  frmin=fpar->getRMin();
  frmax=fpar->getRMax();
  fminSignalAmp=fpar->getMinSigAmp();

  feventcounter=0;

  //Create Histograms for recording spread of Avalanche-centre to hit Pads
  if(fqa!=NULL){
	  fxVariation = fqa->getTH1D("xSpread","Hit Pad x-coordinates around Avalanche centre", 
			 200, -0.3, 0.3);
	  fyVariation = fqa->getTH1D("ySpread", 
			 "Hit Pad y-coordinates around Avalanche centre", 
			 200, -0.3, 0.3);
	  f2DHisto = fqa->getTH2D("xySpread","xSpread vs ySpread",
		      200,-0.3,0.3,200,-0.3,0.3);
  }
  fselected=fsecids.size()>0;

  finitialized=true;
  return kSUCCESS;
}


void
PndTpcPadResponseTask::Exec(Option_t* opt)
{
  // Reset output Array
  if(fsignalArray==0) Fatal("PndTpcPadResponse::Exec)","No SignalArray");
  fsignalArray->Delete(); // Note: signals will be cleand up in the adc!
  int nValidHits=0;
  Int_t na=favalancheArray->GetEntriesFast();
  for(Int_t ia=0;ia<na;++ia){
    PndTpcAvalanche* Aval=(PndTpcAvalanche*)favalancheArray->At(ia);

    std::vector<PndTpcPad*> hitPads;
    double xAv=Aval->x();
    double yAv=Aval->y();
    double r=sqrt(xAv*xAv+yAv*yAv);
    if(r<frmin || r>frmax) {
      //Warning("Exec","Avalanche outside of allowed region! r=%f",r);
      continue;
    }
    fpadPlane->GetPadList(xAv,yAv,
                         fgem->spread()*3,
                         hitPads);
    // Build Signals
    int nHits=hitPads.size();
    //std::cout<<nHits<<" Pads hit by avalanche"<<std::endl;

    for(int iHit=0; iHit<nHits; ++iHit){
      PndTpcPad* pad=hitPads[iHit];
      if(fselected){
	// find sectorid
	std::map<unsigned int,bool>::iterator it=fsecids.find(pad->sectorId());
	if(it==fsecids.end())continue;
      }
      double Amp=Aval->amp()*pad->GetValue(xAv,yAv);
      if(fpar->getGaussianNoise()){
	Amp+=gRandom->Gaus(0.,fpar->getGaussianNoiseAmp());
      }
      //Fill Histograms with these Values

      FillHistograms(xAv, yAv, pad->x(), pad->y() );      

      // cut on amplitude
      if(Amp>=fminSignalAmp){
        PndTpcSignal* sig=new((*fsignalArray)[nValidHits]) PndTpcSignal(Aval->t(),Amp,
                                     pad->id(),0,
                                     0);
        sig->setmcTrackId(Aval->mcTrackId());
        sig->setmcHitId(Aval->mcHitId());
        //sig->SetLink(FairLink(favalancheBranchName, ia));
        ++nValidHits;
      }
      else {
	//std::cout<<"PadAmp too small! Amp="<<Amp<<"<"<<fminSignalAmp<<std::endl;
      }
    } // end loop over hit pads

  } // end loop over avalanches

  ++feventcounter;
  std::cout<<fsignalArray->GetEntriesFast()<<" Signals created"<<std::endl;

  return;
}

void PndTpcPadResponseTask::FillHistograms(double xAv, double yAv,
					double xPad, double yPad) {
  if(fqa!=NULL){
	  fxVariation->Fill(xAv - xPad);
	  fyVariation->Fill(yAv - yPad);
	  f2DHisto->Fill(xAv - xPad, yAv - yPad);
  }
}

//!Method WriteHistograms() has to be called once in the runDigi.C macro!
//! This method is deprecated - replaced by the QAPlotcollection!
void PndTpcPadResponseTask::WriteHistograms() {

  if(!finitialized || fqa==NULL)return;
  TFile* file = FairRootManager::Instance()->GetOutFile();
  file->mkdir("PndTpcPadResponse");
  file->cd("PndTpcPadResponse");

  fxVariation->Write();
  fyVariation->Write();
  f2DHisto->Write();

}


ClassImp(PndTpcPadResponseTask)
