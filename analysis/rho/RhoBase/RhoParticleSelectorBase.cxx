//////////////////////////////////////////////////////////////////////////
//                                                                      //
// RhoParticleSelectorBase                                              //
//                                                                      //
// Selector classes for particle identification                         //
// Particle masses are set from the TDatabasePDG class                  //
//                                                                      //
// Author: Marcel Kunze, RUB, Feb. 99                                   //
// Copyright (C) 1999-2001, Ruhr-University Bochum.                     //
// Ralf Kliemt, HIM/GSI Feb.2013 (Cleanup & Restructuring)              //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#include "TDatabasePDG.h"
#include "RhoBase/RhoParticleSelectorBase.h"
#include "RhoBase/RhoCandidate.h"
#include "FairRecoCandidate.h"
#include "RhoBase/RhoCandList.h"

ClassImp ( RhoParticleSelectorBase )

#include <iostream>
using namespace std;

//TParm VAbsPidSelector::fParms;
//TAssociator *VAbsPidSelector::fAssociator=0;

RhoParticleSelectorBase::RhoParticleSelectorBase ( const char* name, const char* type ) :
  TNamed ( name,name ),fTypePlus ( 0 ),fTypeMinus ( 0 ),fCriterion ( loose )//,fQC ( 0 )
{
//  SetParm("criteria","loose"); // The default setting

  // Set the particle type (plus, minus, neutral)

  if ( type != 0 ) {
    TDatabasePDG* pdg = TDatabasePDG::Instance();
    //        TDatabasePDG *pdg = TDatabasePDG::Instance();
    TString sType ( type );
    if ( sType.Index ( "+" ) >0  || sType=="p+" || sType=="proton" ) {
      fTypePlus  = pdg->GetParticle ( type );
      fTypeMinus = CPConjugate ( fTypePlus );
      //cout <<sType<<" "<<pdg->GetParticle(sType)->PdgCode()<<" is fTypePlus "<<pdg->GetParticle(sType)->Charge()<<endl;
    } else if ( sType.Index ( "-" ) >0  || sType=="anti-p-" || sType=="antiproton" ) {
      fTypeMinus = pdg->GetParticle ( type );
      fTypePlus  = CPConjugate ( fTypeMinus );
    } else {
      fTypeMinus = fTypePlus = pdg->GetParticle ( type );
    }
  }
}

RhoParticleSelectorBase::~RhoParticleSelectorBase()
{}

// void VAbsPidSelector::Initialize()
// {
//   TString criterion = fParms.GetString("criteria",this);
//   SetCriterion(criterion.Data());
// }

void RhoParticleSelectorBase::SetCriterion ( const char* c )
{
  TString crit ( c );
  crit.ToLower();

  if ( crit=="best" ) {
    SetCriterion ( best );
  } else if ( crit=="veryloose" ) {
    SetCriterion ( veryLoose );
  } else if ( crit=="loose" ) {
    SetCriterion ( loose );
  } else if ( crit=="tight" ) {
    SetCriterion ( tight );
  } else if ( crit=="verytight" ) {
    SetCriterion ( veryTight );
  } else if ( crit=="variable" ) {
    SetCriterion ( variable );
  } else if ( crit=="all" ) {
    SetCriterion ( all );
  } else {
    cerr << GetName() << ": Unknown criterion " << crit.Data() << endl;
    return;
  }

//  fParms.Set("criteria",crit,"Selector criterion",this);
  //cout << this->ClassName() << "::SetCriterion " << c << " for " << GetName() << endl;
}

void RhoParticleSelectorBase::SetCriterion ( criterion crit )
{
  fCriterion = crit;
}

void RhoParticleSelectorBase::Select ( RhoCandList& l )
{
  l.Select(this);
  //  RhoCandList tmp ( l );
  //  Select ( tmp,l );
}

void RhoParticleSelectorBase::Select ( RhoCandList& in, RhoCandList& out )
{
  out.Cleanup();
  Int_t n = in.GetLength();
  for ( Int_t i=0; i<n; i++ ) {
    RhoCandidate* c = in[i];

    if ( Accept ( c ) ) { 
      //printf("selector \"%s\" accepted i=%i\n",fName.Data(),i);
      out.Put ( c ); 
    } //else printf("selector \"%s\" rejected i=%i\n",fName.Data(),i);
  }
}

// Parameter setting

//
// TAssociator* VAbsPidSelector::GetAssociator() const
// {
//   return fAssociator;
// }

// void VAbsPidSelector::SetAssociator(TAssociator *assoc)
// {
//   fAssociator = assoc;
// }

TParticlePDG*
RhoParticleSelectorBase::CPConjugate ( TParticlePDG* thePart )
{
  //    TDatabasePDG *pdt = TDatabasePDG::Instance();
  TDatabasePDG* pdt = TDatabasePDG::Instance();
  Int_t theCode = thePart->PdgCode();
  TString name = thePart->GetName();

  // Is it a charged particle ?
  if ( name.Index ( "-" ) >0 || name.Index ( "+" ) >0 || abs ( theCode ) ==2212 ) { theCode = -theCode; }

  return pdt->GetParticle ( theCode );
}

void
RhoParticleSelectorBase::SetTypeAndMass ( RhoCandidate* b )
{
  if ( b == 0 ) { return; }
  // Set the particle type
  if ( b->GetCharge() >0.1 ) {
    if ( fTypePlus == 0 ) { return; }
    b->SetType ( fTypePlus );
    b->SetMass ( fTypePlus->Mass() );
  } else {
    if ( fTypeMinus == 0 ) { return; }
    b->SetType ( fTypeMinus );
    b->SetMass ( fTypeMinus->Mass() );
  }
}
