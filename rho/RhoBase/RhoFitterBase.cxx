//////////////////////////////////////////////////////////////////////////
//                                                                      //
// RhoFitterBase                                                        //
//                                                                      //
// Abstract class for composite particle fitting                        //
//                                                                      //
// Author: Marcel Kunze, RUB, Feb. 99                                   //
// Copyright (C) 1999-2001, Ruhr-University Bochum.                     //
// Ralf Kliemt, HIM/GSI Feb.2013 (Cleanup & Restructuring)              //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#include <assert.h>

#include "RhoBase/RhoFitterBase.h"
#include "RhoBase/RhoFactory.h"


ClassImp ( RhoFitterBase )

TBuffer& operator>> ( TBuffer& buf, RhoFitterBase *&obj )
{
  obj = ( RhoFitterBase* ) buf.ReadObject ( RhoFitterBase::Class() );
  return buf;
}

#include <iostream>
using namespace std;

RhoFitterBase::RhoFitterBase ( RhoCandidate* head ) : fVerbose ( kFALSE ), fChiSquare(-9999), fNDegreesOfFreedom(-9999)
{
  //fHeadOfTree=CopyCand( head );
  //TODO do we want to copy the trere here and now?
  fHeadOfTree=CopyTree(head);
  fChi2Map.clear();
}

RhoFitterBase::~RhoFitterBase()
{
}

RhoFitterBase::RhoFitterBase ( const RhoFitterBase& other ) : TObject(other), fVerbose ( other.fVerbose )
{
  //fHeadOfTree=CopyCand(*other.fHeadOfTree);
  //TODO do we want to copy the tree here and now?
  fHeadOfTree=CopyTree(other.fHeadOfTree);
}


RhoCandidate*
RhoFitterBase::CopyCand ( RhoCandidate* b )
{
  RhoCandidate* newCand = RhoFactory::Instance()->NewCandidate ( b );
  newCand->RemoveAssociations();
  b->SetFit(newCand);//ready to be modified
  return newCand;
}


RhoCandidate*
RhoFitterBase::CopyTree ( RhoCandidate* head )
{
  //std::cout<<"\n\tcopy tree "<<head->Uid()<<" "<<&head<<" "<<head->PdgCode()<<" "<<head->NDaughters()<<"...";
  RhoCandidate* headcopy=CopyCand(head);
  RhoCandidate* daucopy=0;
  RhoCandidate* dau=0;
  for(Int_t i=0;i<head->NDaughters();i++)
  {
    dau=head->Daughter(i);
    //std::cout<<"  daugter "<<dau->Uid()<<" "<<i<<" "<<dau->PdgCode()<<" at "<<dau<<"  ";
    if(dau == head) {
      std::cout<<endl<<"*** Candidate is its own mother???  *** \n"<<std::endl;
      std::cout<<"  print:   "<<*head<<std::endl;;
    }
    assert(dau != head);
    if(dau->IsComposite()) daucopy=CopyTree(dau);
	else daucopy=CopyCand(dau);
	//std::cout<<"CopyTree: copied candidate "<<dau->Uid()<<std::endl;
    daucopy->SetMotherLink(headcopy); //daughter link is set automatically, too
  }
  return headcopy;
}



Double_t RhoFitterBase::Chi2Contribution ( const RhoCandidate* b )
{
  if(!b) return -999.;
  Int_t uid = b->Uid();
  Double_t chi2=fChi2Map[uid];
  return chi2 >=0.0 ? chi2 : -1.;
}


Bool_t RhoFitterBase::Fit()
{
  fChi2Map.clear();
  return FitNode(fHeadOfTree);
}

Bool_t RhoFitterBase::FitAll()
{
  if(fHeadOfTree->IsLocked())
  {
    Warning("RhoFitterBase::FitAll","You tried to fit a locked candidate. Retuning kFALSE now.");
    return kFALSE;
  }
  return IterateAndFit(fHeadOfTree);
}


Bool_t RhoFitterBase::IterateAndFit(RhoCandidate* b)
{
  RhoCandidate* dau=0;
  Bool_t check = kFALSE;
  for(Int_t i=0;i<b->NDaughters();i++)
  {
    dau=b->Daughter(i);
    if (dau->IsComposite() && !dau->IsLocked())
    {
      check = IterateAndFit(dau);
      if(kFALSE==check) return kFALSE;
    }
  }
  check = FitNode(b);
  if(kFALSE==check) return kFALSE;
  return kTRUE;
}

Bool_t RhoFitterBase::FitNode(RhoCandidate* )
{
  Warning("RhoFitterBase::FitNode","Method not implemented in %s",this->GetName());
  return kFALSE;
}

void RhoFitterBase::SetDaugthersFromComposite(RhoCandidate* cand)
{
  fDaughters.clear();
  if(cand->IsComposite()){
    RhoCandidate* tc;
    for (int k=0;k<cand->NDaughters();k++) {
      tc=cand->Daughter(k);
      fDaughters.push_back(tc);
    }
  }
  return;
}

void RhoFitterBase::FindAndAddFinalStateDaughters(RhoCandidate* cand)
{
  RhoCandidate* tc;
  for (int k=0;k<cand->NDaughters();k++) {
    tc=cand->Daughter(k);
    if (!tc->IsComposite() || tc->IsLocked()) { fDaughters.push_back(tc); }
    else { FindAndAddFinalStateDaughters(tc); }
  }
  return;
}

void RhoFitterBase::SetFourMomentumByDaughters(RhoCandidate* composite)
{
  RhoCandidate* tc;
  TLorentzVector tmpLV;
  TMatrixD tmpCov(7,7);
  // Sum daughter fourmomenta, dive into nodes if necessary
  for (int k=0;k<composite->NDaughters();k++) {
    tc=composite->Daughter(k);
    if (tc->IsComposite() && !tc->IsLocked()) { SetFourMomentumByDaughters(tc); }
    tmpLV += tc->P4();
    tmpCov = tmpCov + tc->Cov7();
  }
  composite->SetP4(tmpLV);
  composite->SetCov7(tmpCov);
  //std::cout<<" Base fitter cov7 from tc "<<tc->Uid()<<"/"<<tc->Charge()<<"/"<<tc->PdgCode()<<": ";tmpCov.Print();
  return;
}

void RhoFitterBase::SetDecayVertex(RhoCandidate* composite, const TVector3& vtx, const TMatrixD& CovVV)
{
  RhoError decaypointcov(CovVV);
  RhoVector3Err decayvertex(vtx,decaypointcov);
  composite->SetDecayVtx(decayvertex);
}


