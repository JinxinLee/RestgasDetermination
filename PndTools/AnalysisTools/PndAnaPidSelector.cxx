//
//  PndAnaPidSelector.cxx
//  PANDAROOT
//
//  Created by Ralf Kliemt on 12/16/11.
//
//  This class does charged PID selection on TCandList objects.
//  The cut parameters are taken from the RTDB
//
//
//

#include <math.h>
#include "TDatabasePDG.h"

#include "PndAnaPidSelector.h"

#include "RhoBase/TRho.h"
#include "RhoBase/TCandidate.h"
#include "RhoBase/VAbsMicroCandidate.h"

#include "FairRun.h"
#include "FairRuntimeDb.h"

#include "PndAnaContFact.h"
#include "PndAnaSelectorPar.h"
//#include "PndAnaFluxPar.h"

#include "TString.h"
#include "TList.h"

ClassImp(PndAnaPidSelector)

TBuffer &operator>>(TBuffer &buf,PndAnaPidSelector  *&obj)
{
  obj = (PndAnaPidSelector *) buf.ReadObject(PndAnaPidSelector::Class());
  return buf;
}

PndAnaPidSelector::PndAnaPidSelector(const char *name, const char *type) : 
VAbsPidSelector(name,type) 
{
  fSelectorParameterList=new TList();
  fChargeCrit=0;
  
  FairRun* ana = FairRun::Instance();
  FairRuntimeDb* rtdb=ana->GetRuntimeDb();
  Int_t runID = ana->GetRunId();
  PndAnaContFact* theContfact = (PndAnaContFact*)rtdb->getContFactory("PndAnaContFact");
  TList* theContNames = theContfact->GetParNames();
  Info("PndAnaPidCombiner()","The container names list contains %i entries",theContNames->GetEntries());
  TIter cfIter(theContNames);
  while (TObjString* contname = (TObjString*)cfIter()) {
    TString parsetname = contname->String();
    Info("PndAnaPidCombiner()","Parset Name is: %s",parsetname.Data());
    if(parsetname.BeginsWith("ANAPidSelections")){
      PndAnaSelectorPar* selpar = (PndAnaSelectorPar*)(rtdb->getContainer(parsetname.Data()));
      if ( ! selpar ) Fatal("SetParContainers","No ANAPidSelections* parameter found: %s",parsetname.Data());
      fSelectorParameterList->Add(selpar);
    }
    //    if(parsetname.BeginsWith("ANAPidFlux")){
    //      PndAnaFluxPar* fluxpar = (PndAnaFluxPar*)(rtdb->getContainer(parsetname.Data()));
    //      if ( ! fluxpar ) Fatal("SetParContainers","No ANAPidFlux parameter found: %s",parsetname.Data());
    //      fFluxParameterList->Add(fluxpar);
    //    }
  }//while
   // Caution: The Parameter Set is not filled from the DB IO, yet. 
  rtdb->initContainers(runID); // actually fill the containers. We might want to do that at another point, because multiple instnces will multiply call the RTDB init. That's bad in a loop!
  
  // Now we access the RTDB once and perform the selection on local variables.
  fVeryLoose=0.0;
  fLoose=0.2;
  fTight=0.5;
  fVeryTight=0.9;
  
}

Bool_t PndAnaPidSelector::SetSelection(TString &crit)
{
  // parse criterion string to criteria flags... 
  if(crit.Contains("VeryLoose")) VAbsPidSelector::SetCriterion(veryLoose); 
  else if(crit.Contains("Loose")) VAbsPidSelector::SetCriterion(loose);     
  else if(crit.Contains("Tight")) VAbsPidSelector::SetCriterion(tight);     
  else if(crit.Contains("VeryTight")) VAbsPidSelector::SetCriterion(veryTight); 
  
  TDatabasePDG *pdg = TRho::Instance()->GetPDG();
  // Name convention for TDatabsePDG found at $ROOTSYS/etc/pdg_table.txt
  if(crit.Contains("Proton")) fTypePlus=pdg->GetParticle("proton");
  else if(crit.Contains("Kaon")) fTypePlus=pdg->GetParticle("K+");
  else if(crit.Contains("Pion")) fTypePlus=pdg->GetParticle("pi+");
  else if(crit.Contains("Muon")) fTypePlus=pdg->GetParticle("mu+");
  else if(crit.Contains("Electron")) fTypePlus=pdg->GetParticle("e+");
  fTypeMinus = CPConjugate(fTypePlus);

  if (crit.Contains("Plus")) fChargeCrit=1.;
  else if (crit.Contains("Minus")) fChargeCrit=-1.;
  else fChargeCrit=0;
           
  return kTRUE;
}


Bool_t PndAnaPidSelector::Accept(TCandidate& b) 
{ 
  
  if (&b == 0) return kFALSE;
  
  // too stringent on charge with +-1. ??
  if(fChargeCrit!=0 && fChargeCrit!=b.GetCharge()) return kFALSE;
  
  SetTypeAndMass(b);
  
  double Le  = b.GetPidInfo(0);
  double Lmu = b.GetPidInfo(1);
  double Lpi = b.GetPidInfo(2);
  double Lk  = b.GetPidInfo(3);
  double Lp  = b.GetPidInfo(4);
  double Lcheck = b.GetPidInfo(fPidSelect);
  
  
  if (fCriterion == veryLoose) {
    if (Lcheck<fVeryLoose) return kFALSE;
  }
  else if (fCriterion == loose) {
    if (Lcheck<fLoose) return kFALSE;
  }
  else if (fCriterion == tight) {
    if (Lcheck<fTight) return kFALSE; 
  } 
  else if (fCriterion == veryTight) {
    if (Lcheck<fVeryTight) return kFALSE;
  }
  else if (fCriterion == variable || fCriterion == all  ) {
    return kTRUE;
  }
  else if (fCriterion == best) {
    if (Lcheck<Le || Lcheck<Lmu || Lcheck<Lpi || Lcheck<Lk || Lcheck<Lp) return kFALSE;
  }
  
  return kTRUE;
}

Bool_t PndAnaPidSelector::Accept(VAbsMicroCandidate& b) 
{ 
  return kFALSE;
}




