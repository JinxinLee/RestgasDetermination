 //-----------------------------------------------------------
// File and Version Information:
// $Id$
//
// Description:
//      Implementation of class TrackVisTask
//      see TrackVisTask.h for details
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
#include "TrackVisTask.h"

// C/C++ Headers ----------------------
#include <map>
#include <algorithm>
#include <string>

// Collaborating Class Headers --------
#include "FairRootManager.h"
#include "TClonesArray.h"
#include "Track.h"

#include "AbsTrackRep.h"
#include "RecoHitFactory.h"

#include "TApplication.h"
#include "TGeoManager.h"
#include "TROOT.h"
#include "TSystem.h"
#include "TCanvas.h"
#include "TList.h"
#include "TGeoMaterial.h"


TrackVisTask::TrackVisTask()
  : FairTask("TrackVis"), _persistence(kFALSE)//, app("app",0,NULL)
{
  _trackBranchName = "TrackPostFit";

}


TrackVisTask::~TrackVisTask()
{
}


InitStatus
TrackVisTask::Init()
{
  //Get ROOT Manager
  FairRootManager* ioman= FairRootManager::Instance();

  if(ioman==0)
    {
      Error("TrackReadTask::Init","RootManager not instantiated!");
      return kERROR;
    }
  
  // Get input collection
  _trackArray=(TClonesArray*) ioman->GetObject(_trackBranchName);
  
  if(_trackArray==0)
    {
      Error("TrackReadTask::Init","Track-array not found!");
      return kERROR;
    }

  _geom=new TGeoManager("world","world");
  TGeoMaterial *matVacuum = new TGeoMaterial("Vacuum", 0,0,0);
  TGeoMedium *Vacuum = new TGeoMedium("Vacuum",1, matVacuum);
  TGeoVolume *top = _geom->MakeBox("TOP", Vacuum, 100., 100., 800.);
  top->SetVisibility(kFALSE);
  top->SetTransparency(20);
  _geom->SetTopVolume(top);

  return kSUCCESS;

}


void
TrackVisTask::Exec(Option_t* opt)
{
  std::cout << "TrackVisTask::Exec" << std::endl;
    std::vector<Track*> trackvec;

  Int_t nTracks=_trackArray->GetEntriesFast();
    for(int i=0;i<nTracks;i++) {
	trackvec.push_back( (Track*)  _trackArray->At(i));
  }
  _geom->ClearTracks();

  if(trackvec.size()==0)return;
  TCanvas *c1 = new TCanvas("c1");
  

  for(unsigned int i=0;i<nTracks; ++i){
    if(trackvec[i]->getCardinalRep()->getStatusFlag()!=0)continue;
    TVirtualGeoTrack* geotrk=_geom->GetTrack(_geom->AddTrack(i,13));
    trackvec[i]->fillGeoTrack(geotrk);
  }
  
  _geom->SetVisLevel(4);
  _geom->GetTopVolume()->Draw();
  _geom->DrawTracks();


  std::cout << std::endl << "================================" << std::endl;
  std::cout << "To continue with next event, do:" << std::endl;
  std::cout << "MENU File -> Quit ROOT" << std::endl;
  std::cout << "================================" << std::endl;
  gApplication->SetReturnFromRun(kTRUE);
  gSystem->Run();

  delete c1;

  return;


}














ClassImp(TrackVisTask)
