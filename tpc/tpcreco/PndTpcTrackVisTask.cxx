//-----------------------------------------------------------
// File and Version Information:
// $Id$
//
// Description:
//      Implementation of class PndTpcTrackVisTask
//      see PndTpcTrackVisTask.h for details
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
#include "PndTpcTrackVisTask.h"

// C/C++ Headers ----------------------
#include <map>
#include <algorithm>
#include <string>

// Collaborating Class Headers --------
#include "FairRootManager.h"
#include "TClonesArray.h"
#include "GFTrack.h"
#include "GFTrackCand.h"
#include "PndTpcCluster.h"

#include "GFAbsTrackRep.h"
#include "GFRecoHitFactory.h"

#include "TApplication.h"
#include "TGeoManager.h"
#include "TROOT.h"
#include "TSystem.h"
#include "TCanvas.h"
#include "TList.h"
#include "TGeoMaterial.h"
#include "TVirtualGeoTrack.h"
#include "TPolyMarker3D.h"

PndTpcTrackVisTask::PndTpcTrackVisTask()
  : FairTask("PndTpcTrackVis"), _persistence(kFALSE),_fitFlag(kFALSE)
{
  _trackBranchName = "TrackPostFit";

}


PndTpcTrackVisTask::~PndTpcTrackVisTask()
{
}


InitStatus
PndTpcTrackVisTask::Init()
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

  _clusterArray=(TClonesArray*) ioman->GetObject("PndTpcCluster");
  if(_clusterArray==0)
    {
      Error("TrackReadTask::Init","Cluster-array not found!");
      return kERROR;
    }



  _geom=new TGeoManager("world","world");
  TGeoMaterial *matVacuum = new TGeoMaterial("Vacuum", 0,0,0);
  TGeoMedium *Vacuum = new TGeoMedium("Vacuum",1, matVacuum);
  TGeoVolume *top = _geom->MakeBox("TOP", Vacuum, 50., 50., 110.);
  top->SetVisibility(kFALSE);
  top->SetTransparency(0);
  _geom->SetTopVolume(top);

  return kSUCCESS;

}


void
PndTpcTrackVisTask::Exec(Option_t* opt)
{
  std::cout << "PndTpcTrackVisTask::Exec" << std::endl;
    std::vector<GFTrack*> trackvec;

  Int_t nTracks=_trackArray->GetEntriesFast();
    for(int i=0;i<nTracks;i++) {
	trackvec.push_back( (GFTrack*)  _trackArray->At(i));
  }
  _geom->ClearTracks();

  if(trackvec.size()==0)return;
  TCanvas *c1 = new TCanvas("c1");
  
  std::vector<TPolyMarker3D*> hitTrks;
    
  // draw all hits
  int ncl=_clusterArray->GetEntriesFast();
  TPolyMarker3D* hits=new TPolyMarker3D(ncl);
  for(int icl=0;icl<ncl;++icl){
    unsigned int detid,hitid;
    PndTpcCluster* cl=(PndTpcCluster*)_clusterArray->At(icl);
    //cl->pos().Print();
    hits->SetPoint(icl,cl->pos().X(),cl->pos().Y(),cl->pos().Z());
    hits->SetMarkerColor(1);
    hits->SetMarkerStyle(21);
    hits->SetMarkerSize(0.5);
  }

 
  for(unsigned int i=0;i<nTracks; ++i){
    if(_fitFlag){
      if(trackvec[i]->getCardinalRep()->getStatusFlag()==0){
	std::cout<<"Filling track info"<<std::endl;
	TVirtualGeoTrack* geotrk=_geom->GetTrack(_geom->AddTrack(i,13));
	trackvec[i]->fillGeoTrack(geotrk);
	geotrk->SetLineColor(kBlue);
      }
    }


    // draw hits per track
    GFTrackCand cand=trackvec[i]->getCand();
    TPolyMarker3D* hittrk=new TPolyMarker3D(cand.getNHits());
    hitTrks.push_back(hittrk);
    std::cout<<"Filling hits"<<std::endl;
    for(int icl=0;icl<cand.getNHits();++icl){
      unsigned int detid,hitid;
      cand.getHit(icl,detid,hitid);
      if(detid!=2) continue;
      PndTpcCluster* cl=(PndTpcCluster*)_clusterArray->At(hitid);
      //cl->pos().Print();
      hittrk->SetPoint(icl,cl->pos().X(),cl->pos().Y(),cl->pos().Z());
      hittrk->SetMarkerColor(i%15+1);
      hittrk->SetMarkerStyle(24);
      hittrk->SetMarkerSize(0.3);
    }

  }
  
  _geom->SetVisLevel(4);
  _geom->GetTopVolume()->Draw();
  //_geom->Top();
 
  hits->Draw("same");

  for(unsigned int i=0;i<nTracks; ++i){
    hitTrks[i]->Draw("same");
  } 
  _geom->DrawTracks();

  std::cout << std::endl << "================================" << std::endl;
  std::cout << "To continue with next event, do:" << std::endl;
  std::cout << "MENU File -> Quit ROOT" << std::endl;
  std::cout << "================================" << std::endl;
  gApplication->SetReturnFromRun(kTRUE);
  gSystem->Run();

  delete hits;
  for(unsigned int i=0;i<nTracks; ++i){
    delete hitTrks[i];
  }
  delete c1;

  return;


}














ClassImp(PndTpcTrackVisTask)
