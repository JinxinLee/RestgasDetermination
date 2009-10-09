//-----------------------------------------------------------
// File and Version Information:
// $Id$
//
// Description:
//      Implementation of class DemoToolsTask
//      see DemoToolsTask.hh for details
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
#include "DemoToolsTask.h"

// C/C++ Headers ----------------------
#include <algorithm>
#include <iostream>
#include <assert.h>

// Collaborating Class Headers --------
#include "FairRootManager.h"
#include "TClonesArray.h"
#include "GFTrack.h"
//#include "PndTpcPoint.h"
#include "DemoRecoHit.h"
#include "FairMCPoint.h"
#include "LSLTrackRep.h"
#include "GeaneTrackRep.h"
#include "GFRecoHitFactory.h"
#include "GFKalman.h"
#include "GFException.h"
#include "TH1D.h"
#include "TFile.h"
#include "TGeoTrack.h"
#include "TGeoManager.h"

// Class Member definitions -----------


DemoToolsTask::DemoToolsTask()
  : FairTask("Tools Filter"), _persistence(kFALSE), _evt(0)
{
  _trackBranchName = "Track";
}


DemoToolsTask::~DemoToolsTask()
{
  if(_lengthH!=NULL)delete _lengthH;
}

InitStatus
DemoToolsTask::Init()
{
  //Get ROOT Manager
  FairRootManager* ioman= FairRootManager::Instance();

  if(ioman==0)
    {
      Error("DemoToolsTask::Init","RootManager not instantiated!");
      return kERROR;
    }
  
  // Get input collection
  _trackArray=(TClonesArray*) ioman->GetObject(_trackBranchName);
  
  if(_trackArray==0)
    {
      Error("DemoToolsTask::Init","track-array not found!");
      return kERROR;
    }
  
  // setup histograms
  _lengthH=new TH1D("lengthH","Track length",200,0,100);

  return kSUCCESS;
}


void
DemoToolsTask::Exec(Option_t* opt)
{
  std::cout<<"DemoToolsTask::Exec Event "<<_evt++<<std::endl;
  Int_t ntracks=_trackArray->GetEntriesFast();
  
  // Get the length along the track between the first and the last hit
  for(Int_t itr=0;itr<ntracks;++itr){
    GFTrack* trk=(GFTrack*)_trackArray->At(itr);
    // check that track has been fitted
    if(trk->getCardinalRep()->getStatusFlag()!=0){
      std::cout<< "Discarding track. Status flag !=0" <<std::endl;
      continue;
    }

    // get first and lst hit from track
    GFAbsRecoHit* firstHit=trk->getHit(0);
    GFAbsRecoHit* prelastHit=trk->getHit(trk->getNumHits()-2);
    GFAbsRecoHit* lastHit=trk->getHit(trk->getNumHits()-1);
    
    // Let's make a copy of the trackrep in oder not to disturb the track.
    // For this you have to use the clone function!!!
    GFAbsTrackRep* rep=trk->getCardinalRep()->clone();

    rep->getReferencePlane().Print();

    // now we extrapolate this rep to the first hit:
    rep->extrapolate(firstHit->getDetPlane(rep));
    rep->setReferencePlane(firstHit->getDetPlane(rep));
    rep->Print();

    // now we do the extrapolation to the last hit
    // we use the version of extrapolate which gives the track length
    double length=rep->extrapolate(prelastHit->getDetPlane(rep));

   

    // fill a histo
    _lengthH->Fill(length);

    GFDetPlane p=lastHit->getDetPlane(rep);
    double z=p.getO().Z();
    double x=lastHit->getHitCoord(p)[0][0];
    double y=lastHit->getHitCoord(p)[1][0];

    TVector3 pos(x,y,z);
    

    pos.Print();
    GeaneTrackRep* grep=dynamic_cast<GeaneTrackRep*>(rep);

    if(grep!=0){
      TVector3 poca,dirInPoca;
      grep->extrapolateToPoint(pos,poca,dirInPoca);
      poca.Print();
      grep->Print();
    }


  }
  return;
}

void 
DemoToolsTask::WriteHistograms(){
  TFile* file = FairRootManager::Instance()->GetOutFile();
  file->cd();
  file->mkdir("DemoTools");
  file->cd("DemoTools");

  if(_lengthH!=NULL){
    _lengthH->Write();
    delete _lengthH;
    _lengthH=NULL;
  }

}


ClassImp(DemoToolsTask)
