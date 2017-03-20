/********************************************************************************
 *    Copyright (C) 2014 GSI Helmholtzzentrum fuer Schwerionenforschung GmbH    *
 *                                                                              *
 *              This software is distributed under the terms of the             * 
 *         GNU Lesser General Public Licence version 3 (LGPL) version 3,        *  
 *                  copied verbatim in the file "LICENSE"                       *
 ********************************************************************************/
// -------------------------------------------------------------------------
// -----                        PndCaloDraw source file                  -----
// -----                  Created 03/01/08  by M. Al-Turany            -----
// -------------------------------------------------------------------------
#include "PndCaloDraw.h"

#include "FairBoxSet.h"                 // for FairBoxSet
#include "FairEventManager.h"           // for FairEventManager
#include "FairRootManager.h"            // for FairRootManager
#include "FairRunAna.h"                 // for FairRunAna
#include "FairTSBufferFunctional.h"     // for StopTime
#include "FairTimeStamp.h"              // for FairTimeStamp

#include "Riosfwd.h"                    // for ostream
#include "TClonesArray.h"               // for TClonesArray
#include "TEveBoxSet.h"
#include "TEveManager.h"                // for TEveManager, gEve
#include "TVector3.h"                   // for TVector3

#include <stddef.h>                     // for NULL
#include <iostream>                     // for operator<<, basic_ostream, etc

using std::cout;
using std::endl;

FairBoxSet* fq;    //!
Double_t fX, fY, fZ;

// -----   Default constructor   -------------------------------------------
PndCaloDraw::PndCaloDraw()
  : FairTask("PndCaloDraw",0),
    fVerbose(0),
    fList(NULL),
    fEventManager(NULL),
    fManager(NULL),
    fTimeWindowPlus(0.),
    fTimeWindowMinus(0.),
    fStartTime(0.),
    fUseEventTime(kTRUE),
    fStartFunctor(),
    fStopFunctor()
{
}
// -------------------------------------------------------------------------



// -----   Standard constructor   ------------------------------------------
PndCaloDraw::PndCaloDraw(const char* name, Int_t iVerbose)
  : FairTask(name, iVerbose),
    fVerbose(iVerbose),
    fList(NULL),
    fEventManager(NULL),
    fManager(NULL),
    fTimeWindowPlus(0.),
    fTimeWindowMinus(0.),
    fStartTime(0.),
    fUseEventTime(kTRUE),
    fStartFunctor(),
    fStopFunctor()
{
}
// -------------------------------------------------------------------------
InitStatus PndCaloDraw::Init()
{
  if(fVerbose>1) {
    cout<<  "PndCaloDraw::Init()" << endl;
  }
  fManager = FairRootManager::Instance();

  fList = (TClonesArray*)FairRootManager::Instance()->GetObject(GetName());
  //std::cout << fList << std::endl;
  if (fList==0) {
    cout << "PndCaloDraw::Init()  branch " << GetName() << " Not found! Task will be deactivated "<< endl;
    SetActive(kFALSE);
    return kERROR;
  }
  if(fVerbose>2) {
    cout<<  "PndCaloDraw::Init() get track list" <<  fList<< endl;
  }
  fEventManager =FairEventManager::Instance();
  if(fVerbose>2) {
    cout<<  "PndCaloDraw::Init() get instance of FairEventManager " << endl;
  }

  fCaloHisto = new TH2F("hBarrelCal","BarrelCal", 350, -5.191, 5.191, 200, -TMath::Pi(), TMath::Pi());
  fEveHist = new TEveCaloDataHist();
  fEveHist->AddHistogram(fCaloHisto);
  fEveHist->RefSliceInfo(0).Setup("BarrelCalorimeter",0.01, kRed);
  fEveHist->IncDenyDestroy();
//  gEve->AddToListTree(fEveHist, kFALSE);

  fCalo3D = new TEveCalo3D(fEveHist);
  fCalo3D->SetBarrelRadius(64.00);
  fCalo3D->SetForwardEndCapPos(212);
  fCalo3D->SetBackwardEndCapPos(-64);
  fCalo3D->SetMaxTowerH(200.0);
  fCalo3D->SetScaleAbs(kTRUE);
  gEve->AddElement(fCalo3D, fEventManager );


  fStartFunctor = new StopTime();
  fStopFunctor = new StopTime();

  return kSUCCESS;
}
// -------------------------------------------------------------------------
void PndCaloDraw::Exec(Option_t*ion)
{
  if(IsActive()) {
    TObject* p;
    Reset();
    //  cout<<  "PndCaloDraw::Init() Exec! " << fList->GetEntriesFast() << endl;
    if (FairRunAna::Instance()->IsTimeStamp()) {
      fList->Clear();
      Double_t eventTime = FairRootManager::Instance()->GetEventTime();
      if (fUseEventTime) { fStartTime = eventTime - fTimeWindowMinus; }
      cout << "EventTime: " << eventTime << " TimeWindow: " << fStartTime << " - " << eventTime + fTimeWindowPlus << std::endl;

      fList = FairRootManager::Instance()->GetData(GetName(), fStartFunctor, fStartTime, fStopFunctor, eventTime + fTimeWindowPlus); //FairRootManager::Instance()->GetEventTime() +

    }

    //fList = (TClonesArray *)fManager->GetObject(GetName());
    if (fVerbose > 1) {
      std::cout << GetName() << " fList: " << fList->GetEntries() << std::endl;
    }
    fCaloHisto->Reset();
    for (Int_t i=0; i<fList->GetEntriesFast(); ++i) {
      p=fList->At(i);
      FillHisto(p);
    }
    fCalo3D->SetMaxValAbs(fCaloHisto->GetBinContent(fCaloHisto->GetMaximumBin()));
    std::cout << "CaloHisto MaxValue: " << fEveHist->GetMaxVal(kTRUE) << " " << fCaloHisto->GetBinContent(fCaloHisto->GetMaximumBin()) << std::endl;
    fCalo3D->DataChanged();
    gEve->Redraw3D(kFALSE);
  }
}


void PndCaloDraw::FillHisto(TObject* obj)
{
	TVector3 position = GetVector(obj);
	Double_t energy = GetEnergy(obj);

	fCaloHisto->Fill(position.PseudoRapidity(), position.Phi(), energy);
}


void PndCaloDraw::SetTimeWindowMinus(Double_t val)
{
  fTimeWindowMinus = val;
}

void PndCaloDraw::SetTimeWindowPlus(Double_t val)
{
  fTimeWindowPlus = val;
}

// -----   Destructor   ----------------------------------------------------
PndCaloDraw::~PndCaloDraw()
{
}
// -------------------------------------------------------------------------
void PndCaloDraw::SetParContainers()
{

}
// -------------------------------------------------------------------------
/** Action after each event**/
void PndCaloDraw::Finish()
{
}
// -------------------------------------------------------------------------
void PndCaloDraw::Reset()
{
  if(fq!=0) {
    fq->Reset();
    gEve->RemoveElement(fq, fEventManager );
  }
}


ClassImp(PndCaloDraw)


