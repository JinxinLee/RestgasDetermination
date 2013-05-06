//
//  PndAnaPidSelector.cxx
//  PANDAROOT
//
//  Created by Ralf Kliemt on 12/16/11.
//
//  This class does charged PID selection on RhoCandList objects.
//  The cut parameters are taken from the RTDB
//
//
//

#include <math.h>
#include "TDatabasePDG.h"

#include "PndAnaPidSelector.h"

//#include "RhoBase/TRho.h"
#include "RhoBase/RhoCandidate.h"
#include "FairRecoCandidate.h"

#include "FairRun.h"
#include "FairRuntimeDb.h"

#include "PndAnaContFact.h"
#include "PndAnaSelectorPar.h"
//#include "PndAnaFluxPar.h"

#include "TString.h"
#include "TList.h"


ClassImp ( PndAnaPidSelector )

TBuffer& operator>> ( TBuffer& buf,PndAnaPidSelector  *&obj )
{
  obj = ( PndAnaPidSelector* ) buf.ReadObject ( PndAnaPidSelector::Class() );
  return buf;
}

PndAnaPidSelector::PndAnaPidSelector ( const char* name, const char* type, const char* paramid ) :
    RhoParticleSelectorBase ( name,type ) ,
    fChargeCrit ( 0.0 ),
    fPidSelect ( 99 )   // some silly default number here

{
  // reading parameters from RTDB
  FairRun* ana = FairRun::Instance();
  FairRuntimeDb* rtdb=ana->GetRuntimeDb();
  fSelectPar = ( PndAnaSelectorPar* ) ( rtdb->getContainer ( "ANAPidSelections" ) );

  if ( ! fSelectPar ) {
    Fatal ( "PndAnaPidSelector","No ANAPidSelections* parameter found: %s","ANAPidSelections" );
  }

  //      PndAnaFluxPar* fFluxPar = (PndAnaFluxPar*)(rtdb->getContainer("ANAPidSelectionFlux"));
  //      if ( ! fFluxPar ) Fatal("SetParContainers","No ANAPidFlux parameter found: %s",parsetname.Data());
  //    }

  // Caution: The Parameter Set is not filled from the DB IO, yet.
  Int_t runID = ana->GetRunId();

  // actually fill the containers.
  //We might want to do that at another point, because multiple
  //instnces will multiply call the RTDB init. That's bad in a loop!
  rtdb->initContainers ( runID );
}

Bool_t PndAnaPidSelector::SetSelection ( TString& crit )
{
  //Default setting for criterion
  RhoParticleSelectorBase::SetCriterion ( all );

  // parse criterion string to criteria flags...

  if ( crit.Contains ( "VeryLoose" ) ) {
    RhoParticleSelectorBase::SetCriterion ( veryLoose );
  } else if ( crit.Contains ( "Loose" ) ) {
    RhoParticleSelectorBase::SetCriterion ( loose );
  } else if ( crit.Contains ( "VeryTight" ) ) {
    RhoParticleSelectorBase::SetCriterion ( veryTight );
  } else if ( crit.Contains ( "Tight" ) ) {
    RhoParticleSelectorBase::SetCriterion ( tight );
  } else if ( crit.Contains ( "Variable" ) ) {
    RhoParticleSelectorBase::SetCriterion ( variable );
  } else if ( crit.Contains ( "Best" ) ) {
    RhoParticleSelectorBase::SetCriterion ( best );
  } else if ( crit.Contains ( "All" ) ) {
    RhoParticleSelectorBase::SetCriterion ( all );
  } // well, that's default anyway

  //TODO: make a 2d array of the criterion numbers for faster access

  fTypePlus=0;

  fTypeMinus=0;

  TDatabasePDG* pdg = TDatabasePDG::Instance();

  // Name convention for TDatabsePDG found at $ROOTSYS/etc/pdg_table.txt
  fPidSelect=99;// some silly number here

  if ( crit.Contains ( "Proton" ) ) {
    fTypePlus=pdg->GetParticle ( "p+" );

    if ( 0==fTypePlus ) {
      fTypePlus=pdg->GetParticle ( "proton" );
    }

    fPidSelect=4;
  } else if ( crit.Contains ( "Kaon" ) ) {
    fTypePlus=pdg->GetParticle ( "K+" );
    fPidSelect=3;
  } else if ( crit.Contains ( "Pion" ) ) {
    fTypePlus=pdg->GetParticle ( "pi+" );
    fPidSelect=2;
  } else if ( crit.Contains ( "Muon" ) ) {
    fTypePlus=pdg->GetParticle ( "mu+" );
    fPidSelect=1;
  } else if ( crit.Contains ( "Electron" ) ) {
    fTypePlus=pdg->GetParticle ( "e+" );
    fPidSelect=0;
  }

  if ( fTypePlus!=0 ) {
    fTypeMinus = CPConjugate ( fTypePlus );
  }

  if ( crit.Contains ( "Plus" ) ) {
    fChargeCrit=1.;
  } else if ( crit.Contains ( "Minus" ) ) {
    fChargeCrit=-1.;
  } else {
    fChargeCrit=0;
  }

  return kTRUE;
}


Bool_t PndAnaPidSelector::Accept ( RhoCandidate& b )
{
  // Accept or reject one candidate based on it's PID hypothesis, the pid
  // pdf values and the selection criterion.
  // This might be not performant, since each time we access an object to
  // read the criterion value. Is a field of values faster?

  if ( &b == 0 ) {
    Error ( "PndAnaPidSelector::Accept","Candiate missing." ); return kFALSE;
  }
  //Info("PndAnaPidSelector::Accept","Request: Q=%g pid=%i crit=%i",fChargeCrit,fPidSelect,fCriterion);
  // too stringent on charge with +-1. ??
  if ( fChargeCrit!=0 && fChargeCrit!=b.GetCharge() ) {
    //std::cout<<"PndAnaPidSelector::Accept: charge reject. fChargeCrit="<<fChargeCrit<<" b.GetCharge()"<<b.GetCharge() <<std::endl;
    return kFALSE;
  }

  if ( fPidSelect==99 ) {
    return kTRUE;
  } // no PID requested? Fine!

  // if e, mu, pi, k or p (but no + or -) is requested we reject neutrals
  if ( fPidSelect!=99 && fabs ( b.GetCharge() ) <0.001 ) {
    return kFALSE;
  }

  SetTypeAndMass ( b );

  double Lcheck = b.GetPidInfo ( fPidSelect );
  //std::cout<<"PndAnaPidSelector::Accept: Lcheck="<<Lcheck<<" fPidSelect="<<fPidSelect<<std::endl;

  if ( fCriterion == veryLoose ) {
    if ( Lcheck<fSelectPar->GetVeryLooseCrit ( fPidSelect ) ) {
      return kFALSE;
    }
  } else if ( fCriterion == loose ) {
    if ( Lcheck<fSelectPar->GetLooseCrit ( fPidSelect ) ) {
      return kFALSE;
    }
  } else if ( fCriterion == tight ) {
    if ( Lcheck<fSelectPar->GetTightCrit ( fPidSelect ) ) {
      return kFALSE;
    }
  } else if ( fCriterion == veryTight ) {
    if ( Lcheck<fSelectPar->GetVeryTightCrit ( fPidSelect ) ) {
      return kFALSE;
    }
  } else if ( fCriterion == variable ) {
    if ( Lcheck<fSelectPar->GetVariableCrit ( fPidSelect ) ) {
      return kFALSE;
    }
  } else if ( fCriterion == all ) {
    return kTRUE;
  } else if ( fCriterion == best ) {
    double Le  = b.GetPidInfo ( 0 );
    double Lmu = b.GetPidInfo ( 1 );
    double Lpi = b.GetPidInfo ( 2 );
    double Lk  = b.GetPidInfo ( 3 );
    double Lp  = b.GetPidInfo ( 4 );

    if ( Lcheck<Le || Lcheck<Lmu || Lcheck<Lpi || Lcheck<Lk || Lcheck<Lp ) {
      return kFALSE;
    }
  }

  return kTRUE;
}

Bool_t PndAnaPidSelector::Accept ( FairRecoCandidate& b )
{
  Warning ( "PndAnaPidSelector::Accept(VAbsMicroCandidate&)","No implementation for this. Please use PndAnaPidSelector::Accept(RhoCandidate&)" );
  return kFALSE;
}



