//-----------------------------------------------------------
// File and Version Information:
// $Id$
//
// Description:
//      Implementation of class PndMVDCorrelatorTask
//      see PndMVDCorrelatorTask.hh for details
//
// Environment:
//      Software developed for the PANDA Detector at FAIR.
//
// Author List:
//      Felix Boehmer
//
//-----------------------------------------------------------

// Panda Headers ----------------------

// This Class' Header ------------------
#include "PndTpcMVDCorrelatorTask.h"

// C/C++ Headers ----------------------
#include <iostream>
#include <assert.h>

// Collaborating Class Headers --------
#include "FairRootManager.h"
#include "TClonesArray.h"
#include "GFTrack.h"

#include "GFRecoHitFactory.h"
#include "GFException.h"

#include "GFDetPlane.h"

#include "GeaneTrackRep.h"
#include "RKTrackRep.h"
//#include "FairGeanePro.h"

#include "PndDetectorList.h"
#include "PndSdsHit.h"
#include "PndSdsRecoHit.h"

#include "TH1D.h"

// Class Member definitions -----------


bool sortByR(const std::map<PndSdsHit*, std::map<unsigned int, int> >& h1, 
	     const std::map<PndSdsHit*, std::map<unsigned int, int> >& h2) {
  double r1 = h1.begin()->first->GetPosition().Perp();
  double r2 = h2.begin()->first->GetPosition().Perp();
  return (r1 < r2);
}


PndTpcMVDCorrelatorTask::PndTpcMVDCorrelatorTask()
  : FairTask("TPC-MVD Correlator"), fPersistence(kFALSE), fMatchDistance(0.15),
    fMinMVDHits(3), fRequireMatch(false)
{
  fOutTrackBranchName = "TrackPreFitComplete";
  fTrackBranchName = "TrackPostFit";
  fPixelBranchName = "MVDHitsPixel";
  fStripBranchName = "MVDHitsStrip";
  fVerbose = 0;
}


PndTpcMVDCorrelatorTask::~PndTpcMVDCorrelatorTask()
{
  delete fResHistX;
  delete fResHistY;
  delete fResHistZ;
}

InitStatus
PndTpcMVDCorrelatorTask::Init()
{
  
  
  //Get ROOT Manager
  FairRootManager* ioman= FairRootManager::Instance();
  
  if(ioman==0)
    {
      Error("PndTpcMVDCorrelatorTask::Init","RootManager not instantiated!");
      return kERROR;
    }
  
  // Get input collection
  fTrackArray=(TClonesArray*) ioman->GetObject(fTrackBranchName);

  if(fTrackArray==0)
    {
      Error("PndTpcMVDCorrelatorTask::Init","track-array not found!");
      return kERROR;
    }
  
  // create and register output array
  fOutTrackArray = new TClonesArray("GFTrack"); 
  ioman->Register(fOutTrackBranchName,"PndTpc",fOutTrackArray,fPersistence);
  
  
  // Build hit factory -----------------------------
  fTheRecoHitFactory = new GFRecoHitFactory();
  
  fStripArray=(TClonesArray*) ioman->GetObject(fStripBranchName);
  if(fStripArray==0){ 
    TString error = fStripBranchName;
    error.Append(" array not found");
    Error("PndTpcMVDCorrelatorTask::Init", error);
  } 
  else {
    fTheRecoHitFactory->addProducer
      (ioman->GetBranchId(fStripBranchName),new GFRecoHitProducer<PndSdsHit,PndSdsRecoHit>(fStripArray));
    ;
  }
  
  fPixelArray=(TClonesArray*) ioman->GetObject(fPixelBranchName);
  if(fPixelArray==0){ 
    TString error = fPixelBranchName;
    error.Append(" array not found");
    Error("PndTpcMVDCorrelatorTask::Init", error);
  } 
  else {
    fTheRecoHitFactory->addProducer
      (ioman->GetBranchId(fPixelBranchName),new GFRecoHitProducer<PndSdsHit,PndSdsRecoHit>(fPixelArray));
    ;
  }

  fResHistX = new TH1D("resHistX", "Extrapolation residual distribution",  200, -2, 2);
  fResHistY = new TH1D("resHistY", "Extrapolation residual distribution",  200, -2, 2);
  fResHistZ = new TH1D("resHistZ", "Extrapolation residual distribution",  200, -2, 2);
  
  return kSUCCESS;
}


void
PndTpcMVDCorrelatorTask::Exec(Option_t* opt)
{
  std::cout<<"PndTpcMVDCorrelatorTask::Exec"<<std::endl;
  fOutTrackArray->Delete();
  
  unsigned int ntracks = fTrackArray->GetEntriesFast();
  
  //loop over tracks
  for(unsigned int itr=0;  itr<ntracks; itr++) {
    if(fVerbose) std::cout<<"  ... processing TPC track no. "<<itr<<std::endl;
    GFTrack* track = (GFTrack*) (*fTrackArray)[itr];
    GFAbsTrackRep* rep = track->getCardinalRep();
    
    unsigned int nPix = fPixelArray->GetEntriesFast();
    unsigned int nStr = fStripArray->GetEntriesFast();
    
    TVector3 destination, error;
    TVector3 poca, dirInPoca, res;
    
    std::vector<std::map<PndSdsHit*, std::map<unsigned int, int> > > tempCand; //<SdsHit, <array index, detID enum> >
    
    //loop over MVD pixel hits 
    for(unsigned int ipx=0; ipx<nPix; ipx++) {
      PndSdsHit* hit = (PndSdsHit*) (*fPixelArray)[ipx];
      hit->Position(destination);
      hit->PositionError(error);
      if(fVerbose) {
        std::cout<<"       processing hit at ";
        destination.Print();
        std::cout<<"       position error: ";
        error.Print();
      }
      try {
        rep->extrapolateToPoint(destination, poca, dirInPoca);
      }
      catch(GFException& ex) {
        if(fVerbose)std::cout<<ex.what()<<std::endl;
        continue;
      }
      res.SetXYZ(poca.X()-destination.X(),
           poca.Y()-destination.Y(),
           poca.Z()-destination.Z());
      //Fill Histo
      fResHistX->Fill(res.X());
      fResHistY->Fill(res.Y());
      fResHistZ->Fill(res.Z());

      //check if hit is close enough
      if(res.Mag() > fMatchDistance) {
        if(fVerbose) {
          std::cout<<"       rep:    not close enough: RES was ";
          res.Print();
        }
        continue; //hit wasn't close enough to the track
      }
      else {
        if(fVerbose) {
          std::cout<<"       rep:    added hit - RES was ";
          res.Print();
        }
        std::map<PndSdsHit*, std::map<unsigned int, int> > tmp;
        std::map<unsigned int, int> id;
        id[ipx] = FairRootManager::Instance()->GetBranchId("MVDHitsPixel");
        tmp[hit] = id;
        tempCand.push_back(tmp);
      }
    }//end loop over pixels
    
    //loop over MVD strip hits
    for(unsigned int istr=0; istr<nStr; istr++) {
      PndSdsHit* hit = (PndSdsHit*) (*fStripArray)[istr];
      hit->Position(destination);
      hit->PositionError(error);
      if(fVerbose) {
        std::cout<<"       processing hit at ";
        destination.Print();
        std::cout<<"       position error: ";
        error.Print();
      }

      try {
        rep->extrapolateToPoint(destination, poca, dirInPoca);
      }
      catch(GFException& ex) {
         if(fVerbose) std::cout<<ex.what()<<std::endl;
        continue;
      }
      res.SetXYZ(poca.X()-destination.X(),
           poca.Y()-destination.Y(),
           poca.Z()-destination.Z());
      //Fill Histo
      fResHistX->Fill(res.X());
      fResHistY->Fill(res.Y());
      fResHistZ->Fill(res.Z());

      //check if hit is close enough
      if(res.Mag() > fMatchDistance) {
        if(fVerbose) {
          std::cout<<"       rep:    not close enough: RES was ";
          res.Print();
        }
        continue; //hit wasn't close enough to the track
      }
      else {
        if(fVerbose) {
          std::cout<<"       rep:    added hit - RES was ";
          res.Print();
        }
        std::map<PndSdsHit*, std::map<unsigned int, int> > tmp;
        std::map<unsigned int, int> id;
        id[istr] = FairRootManager::Instance()->GetBranchId("MVDHitsStrip");
        tmp[hit] = id;
        tempCand.push_back(tmp);
      }
    }//end loop over strips
  
    std::cout<<"======= TempCand from found MVD pixel/strip hits has size "<<
          tempCand.size()<<" =======\n"<<std::endl;
    

    if(tempCand.size() < fMinMVDHits) {
      if(!fRequireMatch) {
        GFTrack* outTrack = new GFTrack(*track);
        outTrack->clearBookkeeping();
        (*fOutTrackArray)[fOutTrackArray->GetEntriesFast()] = outTrack;
      }
      continue;
    }
    
    std::sort(tempCand.begin(), tempCand.end(), sortByR);
    //create MVD hit candidate
    GFTrackCand* mvdCand = new GFTrackCand();
    for(unsigned int im=0; im<tempCand.size(); im++) {
      mvdCand->addHit(tempCand[im].begin()->second.begin()->second, //detId enum
		      tempCand[im].begin()->second.begin()->first); //hit in array
    }
    
    mvdCand->setMcTrackId(track->getCand().getMcTrackId());
    // copy track and set new candidate
    GFTrack* outTrack = new GFTrack(*track);
    outTrack->clearBookkeeping();
    outTrack->setCandidate(*mvdCand);
        
    // merge with MVD hits
    GFTrack* tmpTrack = new GFTrack(*track);
    tmpTrack->clearBookkeeping();
    outTrack->mergeHits(tmpTrack);
    outTrack->blowUpCovs(500.);
    (*fOutTrackArray)[fOutTrackArray->GetEntriesFast()] = outTrack;

  } //end loop over tracks

  std::cout <<"### Found "<< fOutTrackArray->GetEntries() << " tracks with MVD correlations." << std::endl; 
  
  return;
}

void
PndTpcMVDCorrelatorTask::WriteHistograms(const TString& fname) const {
  TFile* rOut = new TFile(fname, "recreate");
  rOut->cd();
  fResHistX->Write();
  fResHistY->Write();
  fResHistZ->Write();
  rOut->Close();
}
  
  
  



ClassImp(PndTpcMVDCorrelatorTask);
