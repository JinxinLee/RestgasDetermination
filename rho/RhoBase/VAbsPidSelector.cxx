//////////////////////////////////////////////////////////////////////////
//                                                                      //
// VAbsPidSelector							//
//                                                                      //
// Selector classes for particle identification				// 
// Particle masses are set from the TDatabasePDG class			//
//                                                                      //
// Author: Marcel Kunze, RUB, Feb. 99					//
// Copyright (C) 1999-2001, Ruhr-University Bochum.			//
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#include "TDatabasePDG.h"
#include "RhoBase/VAbsPidSelector.h"
#include "RhoBase/TRho.h"
#include "RhoBase/TCandidate.h"
#include "RhoBase/VAbsMicroCandidate.h"
#include "RhoBase/TCandList.h"

ClassImp(VAbsPidSelector)

#include <iostream>
using namespace std;

TParm VAbsPidSelector::fParms;
TAssociator *VAbsPidSelector::fAssociator=0;

VAbsPidSelector::VAbsPidSelector(const char *name, const char* type) : 
TNamed(name,name),fTypePlus(0),fTypeMinus(0),fCriterion(loose),fQC(0)
{
  SetParm("criteria","loose"); // The default setting
  
  // Set the particle type (plus, minus, neutral)
  
  if (type != 0) {
    TDatabasePDG *pdg = TRho::Instance()->GetPDG();
    //        TDatabasePDG *pdg = TDatabasePDG::Instance();
    TString sType(type);
    if (sType.Index("+")>0  || sType=="p+" || sType=="proton" ) {
	    fTypePlus  = pdg->GetParticle(type);
	    fTypeMinus = CPConjugate(fTypePlus);
	    //cout <<sType<<" "<<pdg->GetParticle(sType)->PdgCode()<<" is fTypePlus "<<pdg->GetParticle(sType)->Charge()<<endl;
    }
    else if (sType.Index("-")>0  || sType=="anti-p-" || sType=="antiproton" ) {
	    fTypeMinus = pdg->GetParticle(type);
	    fTypePlus  = CPConjugate(fTypeMinus);
    }
    else
	    fTypeMinus = fTypePlus = pdg->GetParticle(type);
  }
}

VAbsPidSelector::~VAbsPidSelector() 
{}

void VAbsPidSelector::Initialize() 
{
  TString criterion = fParms.GetString("criteria",this);
  SetCriterion(criterion.Data());
}

void VAbsPidSelector::SetCriterion(const char* c)
{
  TString crit(c);
  
  if (crit=="best")
    SetCriterion(best);
  else if (crit=="veryLoose")
    SetCriterion(veryLoose);
  else if (crit=="loose")
    SetCriterion(loose);
  else if (crit=="tight")
    SetCriterion(tight);
  else if (crit=="veryTight")
    SetCriterion(veryTight);
  else if (crit=="variable")
    SetCriterion(variable);
  else if (crit=="all")
    SetCriterion(all);
  else {
    cerr << GetName() << ": Unknown criterion " << crit.Data() << endl;
    return;
  }
  
  fParms.Set("criteria",crit,"Selector criterion",this);
  cout << this->ClassName() << "::SetCriterion " << c << " for " << GetName() << endl;
}

void VAbsPidSelector::SetCriterion(criterion crit)
{
  fCriterion = crit;
}

void VAbsPidSelector::Select(TCandList &l) 
{
  TCandList tmp(l);
  Select(tmp,l);
}

void VAbsPidSelector::Select(TCandList& in, TCandList& out) 
{
  out.Cleanup();
  Int_t n = in.GetLength();
  for (Int_t i=0;i<n;++i) {
    TCandidate &c = in[i];
    if (Accept(c)) out.Put(c);
  }
}

// Parameter setting

void VAbsPidSelector::SetParm(const char *key, Bool_t value) 
{
  fParms.Set(key,value,"VAbsPidSelector",this); 
}

void VAbsPidSelector::SetParm(const char *key, Double_t value) 
{ 
  fParms.Set(key,value,"VAbsPidSelector",this); 
}

void VAbsPidSelector::SetParm(const char *key, const char *value) 
{ 
  fParms.Set(key,value,"VAbsPidSelector",this); 
}

Bool_t VAbsPidSelector::GetBoolParm(const char *key) 
{ 
  return fParms.GetBool(key,this); 
}

TString VAbsPidSelector::GetStringParm(const char *key) 
{ 
  return fParms.GetString(key,this); 
}

Double_t VAbsPidSelector::GetParm(const char *key) 
{ 
  return fParms.GetDouble(key,this); 
}

Double_t VAbsPidSelector::GetParmValue(const char *key) 
{ 
  return GetParm(key); 
}

TAssociator* VAbsPidSelector::GetAssociator() const
{
  return fAssociator;
}

void VAbsPidSelector::SetAssociator(TAssociator *assoc) 
{ 
  fAssociator = assoc; 
}

TParticlePDG*
VAbsPidSelector::CPConjugate( TParticlePDG* thePart )
{
  //    TDatabasePDG *pdt = TDatabasePDG::Instance();
  TDatabasePDG *pdt = TRho::Instance()->GetPDG();
  Int_t theCode = thePart->PdgCode();
  TString name = thePart->GetName();
  
  // Is it a charged particle ?
  if (name.Index("-")>0 || name.Index("+")>0 || abs(theCode)==2212 ) theCode = -theCode; 
  
  return pdt->GetParticle(theCode);
}

void
VAbsPidSelector::SetTypeAndMass(TCandidate &b)
{
  if (&b == 0) return;
  // Set the particle type
  if (b.GetCharge()>0.1) {
    if (fTypePlus == 0) return;
    b.SetType(fTypePlus);
    b.SetMass(fTypePlus->Mass());
  }
  else {
    if (fTypeMinus == 0) return;
    b.SetType(fTypeMinus);
    b.SetMass(fTypeMinus->Mass());
  }
}
