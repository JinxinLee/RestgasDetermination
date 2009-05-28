//-----------------------------------------------------------
// File and Version Information:
// $Id$
//
// Description:
//      Implementation of class PndTpcdEdxTask
//      see PndTpcdEdxTask.hh for details
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
#include "PndTpcdEdxTask.h"
#include <cmath>
// C/C++ Headers ----------------------
#include <iostream>
#include <assert.h>

// Collaborating Class Headers --------
#include "FairRootManager.h"
#include "TClonesArray.h"
#include "Track.h"
#include "PndTpcCluster.h"
#include "PndTpcSPHit.h"


#include "GeaneTrackRep.h"
#include "AbsTrackRep.h"
#include "RecoHitFactory.h"
#include "Kalman.h"
#include "FitterExceptions.h"
#include "TH1D.h"
#include "TFile.h"
#include "TGeoTrack.h"
#include "TGeoManager.h"
#include "TLorentzVector.h"
#include "DetPlane.h"

#include "AbsRecoHit.h"
#include "TVector3.h"

#include <signal.h>
#include <stdlib.h>



// Class Member definitions -----------


PndTpcdEdxTask::PndTpcdEdxTask()
  : FairTask("dE/dx Task"), _persistence(kFALSE)
{
  _trackBranchName = "Track";
}


PndTpcdEdxTask::~PndTpcdEdxTask()
{
}

InitStatus
PndTpcdEdxTask::Init()
{
  //Get ROOT Manager
  FairRootManager* ioman= FairRootManager::Instance();

  if(ioman==0)
    {
      Error("PndTpcdEdxTask::Init","RootManager not instantiated!");
      return kERROR;
    }
  
  // Get input collection
  _trackArray=(TClonesArray*) ioman->GetObject(_trackBranchName);
   
  if(_trackArray==0)
    {
      Error("PndTpcdEdxTask::Init","track-array not found!");
      return kERROR;
    }
   
  //TClonesArray* ar=(TClonesArray*) ioman->GetObject("PndTpcCluster");
  // if(ar==0){
  //   Error("PndTpcdEdxTask::Init","PndTpcCluster array not found");
  // }
   

  // setup histograms
  _distHist=new TH1D("distHist","Stepping lentghs",500,-3.,3.);
  _dirHist=new TH1D("dirHist","direction abundances",10,-2.,2.);
  
  return kSUCCESS;
}


void
PndTpcdEdxTask::Exec(Option_t* opt)
{
  std::cout<<"PndTpcdEdxTask::Exec"<<std::endl;
  
  Int_t ntracks=_trackArray->GetEntriesFast();
  
  if(ntracks>2000){
    std::cout<<"ntracks="<<ntracks<<" Evil Event! skipping"<<std::endl;
    return;
  }

  for(Int_t itr=0;itr<ntracks;++itr){
    std::cout<<"starting track"<<itr<<std::endl;
    Track* trk=(Track*)_trackArray->At(itr);
    std::cout<<"*** Number of clusters in track: "<<trk->getNumHits()<<" ***"<<std::endl;

    AbsTrackRep* absrep = trk->getCardinalRep();
    

    if(dynamic_cast<GeaneTrackRep*>(absrep) == NULL) {
      std::cerr<<"WRONG trackrep! Need GEANE to process ... skipping track"<<std::endl;
      return;
    }

    GeaneTrackRep* theRep = (GeaneTrackRep*) absrep->clone();

        
    std::vector<AbsRecoHit*> hits = trk->getHits();
    std::cout<<"\nstd::vector<AbsRecoHit*> hits has "<< hits.size()<<" entries"<<std::endl;
    
    std::vector<AbsRecoHit*>::iterator it;
    
    for(it = hits.begin(); it!=hits.end(); it++) {
      PndTpcSPHit* the_sphit = dynamic_cast<PndTpcSPHit*>(*it);
      //erase non-TPC hits
      if(the_sphit==NULL)
	hits.erase(it);
    }
    
    std::cout<<"\n0th hit position: "<<std::endl;
    (hits[0])->getRawHitCoord().Print();
    (((AbsTrackRep*)theRep)->getPos()).Print();
    
     
    return;
  }
}


void 
PndTpcdEdxTask::WriteHistograms(const TString& filename){
  TFile* file = new TFile(filename,"UPDATE");
  file->mkdir("DEDX");
  file->cd("DEDX");

  _distHist->Write();
  delete _distHist;

  _dirHist->Write();
  delete _dirHist;
   
  file->Close();
  delete file;
}



ClassImp(PndTpcdEdxTask)
