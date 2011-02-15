//-----------------------------------------------------------
// File and Version Information:
// $Id$
//
// Description:
//      Implementation of class PndFtsKalmanFit
//      see PndFtsKalmanFit.h for details
//
// Environment:
//      Software developed for the PANDA Detector at FAIR.
//
// Author List:
//      Stefano Spataro, UNI Torino
//      Ralf Kliemt,     UNI Bonn
//
//-----------------------------------------------------------

// Panda Headers ----------------------

// This Class' Header ------------------
#include "PndFtsKalmanFit.h"

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


//#include "PndSdsRecoHit.h"
//#include "PndGemRecoHit.h"
#include "PndHitRecoHit.h"
#include "PndGenfitAdapters.h"
#include "PndTrack.h"
#include "PndTrackCand.h"
#include "PndDetectorList.h"
#include "GFRecoHitFactory.h"
#include "GFKalman.h"
#include "GFException.h"
#include "TLorentzVector.h"

#include "FairTrackParH.h"
#include "FairHit.h"

#include "GeaneTrackRep.h"
#include "FairGeanePro.h"

// Class Member definitions -----------


PndFtsKalmanFit::PndFtsKalmanFit(): TNamed("GenfitFwdFit", "Fit Tracks"),
				      fUseGeane(kTRUE), fNumIt(1), fVerbose(0)
{
 // PndGeoHandling::Instance();
}
Bool_t PndFtsKalmanFit::Init()
{
  //Get ROOT Manager
  FairRootManager* ioman= FairRootManager::Instance();
  if(ioman==0)
    {
      Error("PndFtsKalmanFit::Init","RootManager not instantiated!");
      return kFALSE;
    }

  // STT map loading
  FairRuntimeDb* rtdb = FairRunAna::Instance()->GetRuntimeDb();

  // Build hit factory -----------------------------
  fTheRecoHitFactory = new GFRecoHitFactory();
  if (fVerbose<2) GFException::quiet(true);
  
  TClonesArray* stripar=(TClonesArray*) ioman->GetObject("MVDHitsStrip");
  if(stripar!=0)
    {
      fTheRecoHitFactory->addProducer(FairRootManager::Instance()->GetBranchId("MVDHitsStrip"),new GFRecoHitProducer<FairHit,PndHitRecoHit>(stripar));
      std::cout << "*** PndFtsKalmanFit::Init" << "\t" << "MVDHitsStrip array  found" << std::endl;
    }
  
  TClonesArray* pixelar=(TClonesArray*) ioman->GetObject("MVDHitsPixel");
  if(pixelar!=0)
    { 
      fTheRecoHitFactory->addProducer(FairRootManager::Instance()->GetBranchId("MVDHitsPixel"),new GFRecoHitProducer<FairHit,PndHitRecoHit>(pixelar));
      std::cout << "*** PndFtsKalmanFit::Init" << "\t" << "MVDHitsPixel array  found" << std::endl;
    }
    
  TClonesArray* gemar=(TClonesArray*) ioman->GetObject("GEMHit");
  if(gemar!=0)
    {
      fTheRecoHitFactory->addProducer(FairRootManager::Instance()->GetBranchId("GEMHit"),new GFRecoHitProducer<FairHit,PndHitRecoHit>(gemar));
      std::cout << "*** PndFtsKalmanFit::Init" << "\t" << "GEMHit array  found" << std::endl;
    }
  
  TClonesArray* ftsar=(TClonesArray*) ioman->GetObject("FTSHit");
  if(ftsar!=0)
    {
      fTheRecoHitFactory->addProducer(FairRootManager::Instance()->GetBranchId("FTSHit"),new GFRecoHitProducer<FairHit,PndHitRecoHit>(ftsar));
      std::cout << "*** PndFtsKalmanFit::Init" << "\t" << "FtsHit array  found" << std::endl;
    }
  
  if (fUseGeane)
    {
      fPro = new FairGeanePro();
    }
  else
    {
      Error("PndFtsKalmanFit::Init","Only GEANE Propagatio available!!!");
      return kFALSE;
    }
  
  fGenFitter.setNumIterations(fNumIt);
  
  std::cout << "===PndFtsKalmanFit::Init() finished ===================================================" << std::endl;

  return kTRUE;
}


PndFtsKalmanFit::~PndFtsKalmanFit() { }

PndTrack* PndFtsKalmanFit::Fit(PndTrack *tBefore, Int_t PDG)
{
  PndTrack* tAfter = NULL;
  if (fVerbose>0) std::cout<<"PndFtsKalmanFit::Fit"<<std::endl;
  if (fabs(tBefore->GetParamFirst().GetPz())<1e-9) 
    {
      if(fVerbose>2) Info("Fit","not enough z momentum, skip.");
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

  if(fVerbose>2) Info("Fit","Getting helix parameters");
  GFAbsTrackRep* rep = 0;
 
  // Calculating params at PCA to Origin
  FairTrackParP par = tBefore->GetParamFirst();
  Int_t ierr = 0;
  FairTrackParH *helix = new FairTrackParH(&par, ierr);
  FairGeanePro *fPro0 = new FairGeanePro();
  FairTrackParH *fRes= new FairTrackParH();
  fPro0->SetPoint(TVector3(0,0,0));
  fPro0->PropagateToPCA(1, -1);
  if(fVerbose>2) Info("Fit","Propagate to (0,0,0)");
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
  
  if(fVerbose>2) Info("Fit","Prepare output track");
  GFTrack* trk= new GFTrack(rep);
  PndTrackCand trackCand = tBefore->GetTrackCand();
  trk->setCandidate(*PndTrackCand2GenfitTrackCand(&trackCand));
  
  // Load RecoHits
  if(fVerbose>2) Info("Fit","Load reco Hits");
  try 
    {
      trk->addHitVector(fTheRecoHitFactory->createMany(trk->getCand()));
      if (fVerbose>0) std::cout<<trk->getNumHits()<<" hits in track " << std::endl;
    }
  catch(GFException& e)
    {
      Error("Fit","\tGenfit Exception: trk->addHitVector %s",e.what());
      //throw e;
    }
  // Start Fitter
  if(fVerbose>2) Info("Fit","Start fitter");
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

ClassImp(PndFtsKalmanFit);
