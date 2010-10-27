//-----------------------------------------------------------
// File and Version Information:
// $Id$
//
// Description:
//      Implementation of class PndRecoKalmanFit
//      see PndRecoKalmanFit.h for details
//
// Environment:
//      Software developed for the PANDA Detector at FAIR.
//
// Author List:
//      Stefano Spataro, UNI Torino
//
//-----------------------------------------------------------

// Panda Headers ----------------------

// This Class' Header ------------------
#include "PndRecoKalmanFit.h"

// C/C++ Headers ----------------------
#include <algorithm>
#include <iostream>
#include <assert.h>
#include <cmath>

// Collaborating Class Headers --------
#include "FairRootManager.h"
#include "FairRuntimeDb.h"
#include "FairRunAna.h"
#include "TClonesArray.h"

#include "GFTrack.h"
//#include "TDatabasePDG.h"


#include "PndSdsRecoHit.h"
#include "PndGemRecoHit.h"
#include "PndTpcSPHit.h"
#include "PndSttRecoHit.h"
#include "PndDchRecoHit.h"
#include "PndDchRecoHit2.h"
#include "PndMdtRecoHit.h"
#include "PndSttRecoHitProducer.h"
#include "PndGeoSttPar.h"
#include "PndSttMapCreator.h"
#include "PndGenfitAdapters.h"
#include "PndTrack.h"
#include "PndTrackCand.h"
#include "PndDetectorList.h"
#include "PndGeoHandling.h"
#include "GFRecoHitFactory.h"
#include "GFKalman.h"
#include "GFException.h"
#include "TLorentzVector.h"

#include "FairTrackParH.h"

#include "GeaneTrackRep.h"
#include "FairGeanePro.h"

// Class Member definitions -----------


PndRecoKalmanFit::PndRecoKalmanFit(): TNamed("Genfit", "Fit Tracks"),
				      fUseGeane(kTRUE), fNumIt(1), fVerbose(0)
{
  PndGeoHandling::Instance();
}
Bool_t PndRecoKalmanFit::Init()
{
  //Get ROOT Manager
  FairRootManager* ioman= FairRootManager::Instance();
  if(ioman==0)
    {
      Error("PndRecoKalmanFit::Init","RootManager not instantiated!");
      return kFALSE;
    }

  // STT map loading
  FairRuntimeDb* rtdb = FairRunAna::Instance()->GetRuntimeDb();
  PndGeoSttPar *sttParameters = (PndGeoSttPar*) rtdb->getContainer("PndGeoSttPar");
  PndSttMapCreator *mapper = new PndSttMapCreator(sttParameters);
  TClonesArray *tubeArray = mapper->FillTubeArray();

  // Build hit factory -----------------------------
  fTheRecoHitFactory = new GFRecoHitFactory();
  if (fVerbose<2) GFException::quiet(true);
  
  TClonesArray* stripar=(TClonesArray*) ioman->GetObject("MVDHitsStrip");
  if(stripar!=0)
    {
      fTheRecoHitFactory->addProducer(kMVDHitsStrip,new GFRecoHitProducer<PndSdsHit,PndSdsRecoHit>(stripar));
      std::cout << "*** PndRecoKalmanFit::Init" << "\t" << "MVDHitsStrip array  found" << std::endl;
    }
  
  TClonesArray* pixelar=(TClonesArray*) ioman->GetObject("MVDHitsPixel");
  if(pixelar!=0)
    { 
      fTheRecoHitFactory->addProducer(kMVDHitsPixel,new GFRecoHitProducer<PndSdsHit,PndSdsRecoHit>(pixelar));
      std::cout << "*** PndRecoKalmanFit::Init" << "\t" << "MVDHitsPixel array  found" << std::endl;
    }
  
  TClonesArray* ar=(TClonesArray*) ioman->GetObject("PndTpcCluster");
  if(ar!=0)
    {
      fTheRecoHitFactory->addProducer(kTpcCluster,new GFRecoHitProducer<PndTpcCluster,PndTpcSPHit>(ar));
      std::cout << "*** PndRecoKalmanFit::Init" << "\t" << "PndTpcCluster array  found" << std::endl;
    }
  
  TClonesArray* sttr=(TClonesArray*) ioman->GetObject("SttHelixHit");
  if(sttr!=0)
    {
      fTheRecoHitFactory->addProducer(kSttHelixHit,new PndSttRecoHitProducer<PndSttHelixHit,PndSttRecoHit>(sttr, tubeArray));
      std::cout << "*** PndRecoKalmanFit::Init" << "\t" << "SttHelixHit array  found" << std::endl;
    }
  
  TClonesArray* sthit=(TClonesArray*) ioman->GetObject("STTHit");
  if(sthit!=0)
    {
      fTheRecoHitFactory->addProducer(kSttHit,new PndSttRecoHitProducer<PndSttHit,PndSttRecoHit>(sthit, tubeArray)); 
      std::cout << "*** PndRecoKalmanFit::Init" << "\t" << "SttHit array  found" << std::endl;
    }
  
  TClonesArray* gemar=(TClonesArray*) ioman->GetObject("GEMHit");
  if(gemar!=0)
    {
      fTheRecoHitFactory->addProducer(kGemHit,new GFRecoHitProducer<PndGemHit,PndGemRecoHit>(gemar)); 
      std::cout << "*** PndRecoKalmanFit::Init" << "\t" << "GEMHit array  found" << std::endl;
    }
  
  TClonesArray* dchar=(TClonesArray*) ioman->GetObject("PndDchCylinderHit");
  if(dchar!=0)
    {
      fTheRecoHitFactory->addProducer(kDchHit,new GFRecoHitProducer<PndDchCylinderHit,PndDchRecoHit2>(dchar));
      std::cout << "*** PndRecoKalmanFit::Init" << "\t" << "PndDchCylinderHit array  found" << std::endl;
    }
  
  TClonesArray* mdtar=(TClonesArray*) ioman->GetObject("MdtHit");
  if(mdtar!=0)
    {
      fTheRecoHitFactory->addProducer(kMdtHit,new GFRecoHitProducer<PndMdtHit,PndMdtRecoHit>(mdtar));
      std::cout << "*** PndRecoKalmanFit::Init" << "\t" << "MdtHit array  found" << std::endl;
    }
  
  if (fUseGeane)
    {
      fPro = new FairGeanePro();
    }
  else
    {
      Error("PndRecoKalmanFit::Init","Only GEANE Propagatio available!!!");
      return kFALSE;
    }
  
  fGenFitter.setNumIterations(fNumIt);
  
  std::cout << "===PndRecoKalmanFit::Init() finished ===================================================" << std::endl;

  return kTRUE;
}


PndRecoKalmanFit::~PndRecoKalmanFit() { }

PndTrack* PndRecoKalmanFit::Fit(PndTrack *tBefore, Int_t PDG)
{
  PndTrack* tAfter = NULL;
  if (fVerbose>0) std::cout<<"PndRecoKalmanFit::Fit"<<std::endl;
  if (fabs(tBefore->GetParamFirst().GetPz())<1e-9) 
    {
      tAfter = tBefore;
      tAfter->SetFlag(-10);
      return tAfter; // flag -10 : pz==0
    }
  
  Int_t  fCharge= tBefore->GetParamFirst().GetQ();
  Int_t PDGCode= PDG;
  TVector3 StartPos(tBefore->GetParamFirst().GetX(),tBefore->GetParamFirst().GetY(),tBefore->GetParamFirst().GetZ()); 
  TVector3 StartMom(tBefore->GetParamFirst().GetPx(),tBefore->GetParamFirst().GetPy(),tBefore->GetParamFirst().GetPz());
  TVector3 StartPosErr(tBefore->GetParamFirst().GetDX(),tBefore->GetParamFirst().GetDY(),tBefore->GetParamFirst().GetDZ()); 
  TVector3 StartMomErr(tBefore->GetParamFirst().GetDPx(),tBefore->GetParamFirst().GetDPy(),tBefore->GetParamFirst().GetDPz());
  
  GFAbsTrackRep* rep = 0;
 
  // Calculating params at PCA to Origin
  FairTrackParP par = tBefore->GetParamFirst();
  Int_t ierr = 0;
  FairTrackParH *helix = new FairTrackParH(&par, ierr);
  FairGeanePro *fPro0 = new FairGeanePro();
  FairTrackParH *fRes= new FairTrackParH();
  fPro0->SetPoint(TVector3(0,0,0));
  fPro0->PropagateToPCA(1, -1);
  Bool_t rc =  fPro0->Propagate(helix, fRes, PDGCode);
  if (rc)
    {
      StartPos.SetXYZ(fRes->GetX(), fRes->GetY(), fRes->GetZ());
      StartMom.SetXYZ(fRes->GetPx(), fRes->GetPy(), fRes->GetPz());
      StartPosErr.SetXYZ(fRes->GetDX(), fRes->GetDY(), fRes->GetDZ());
      StartMomErr.SetXYZ(fRes->GetDPx(), fRes->GetDPy(), fRes->GetDPz());
    }
  
  GFDetPlane start_pl(StartPos, TVector3(1.,0.,0.), TVector3(0.,1.,0.));
  GeaneTrackRep *grep = new GeaneTrackRep(fPro,
					  start_pl,StartMom,
					  StartPosErr,StartMomErr,
					  fCharge,PDGCode);
  grep->setPropDir(1);
  rep = grep;
  
  GFTrack* trk= new GFTrack(rep);
  PndTrackCand trackCand = tBefore->GetTrackCand();
  trk->setCandidate(*PndTrackCand2GenfitTrackCand(&trackCand));
  
  // Load RecoHits
  try 
    {
      trk->addHitVector(fTheRecoHitFactory->createMany(trk->getCand()));
      if (fVerbose>0) std::cout<<trk->getNumHits()<<" hits in track " << std::endl;
    }
  catch(GFException& e)
    {
      std::cout << "*** PndRecoKalmanFit::Exec" << "\t" << "Genfit Exception: trk->addHitVector " << e.what() << std::endl;
      //throw e;
    }
  // Start Fitter
  try
    {
      fGenFitter.processTrack(trk);
    }
  catch (GFException e)
    {
      std::cout<<"*** FITTER EXCEPTION ***"<<std::endl;
      std::cout<<e.what()<<std::endl;
    }
  if (fVerbose>0) std::cout<<"SUCCESSFULL FIT!"<<std::endl;
  
  try
    { 
      tAfter = (PndTrack*)GenfitTrack2PndTrack(trk);
    }
  catch (GFException e)
    {
      std::cout<<"*** PndGenfitAdapters EXCEPTION ***"<<std::endl;
      std::cout<<e.what()<<std::endl;
      tAfter = tBefore;
      tAfter->SetFlag(-2); // flag -2: conversion failed
    } 

  if (fVerbose>0) std::cout<<"Fitting done"<<std::endl;

  return tAfter;
}

ClassImp(PndRecoKalmanFit);
