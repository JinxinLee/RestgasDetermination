//--------------------------------------------------------------------------
// Description:
//      Class PndListProvider
//
//  List Container for PndSimpleAnalysis
//
//  This software was developed for the PANDA collaboration.  If you
//  use all or part of it, please give an appropriate acknowledgement.
//
// Author List:
//      Klaus Goetzen                    Original Author
//
// Copyright Information:
//      Copyright (C) 2008              GSI
//
//------------------------------------------------------------------------

//-----------------------
// This Class's Header --
//-----------------------
#include "PndListProvider.h"

//----------------------
// Base Class Headers --
//----------------------

#include <string>
#include <iostream>

//-------------------------------
// Collaborating class Headers --
//-------------------------------
#include "TDatabasePDG.h"
#include "TParticlePDG.h"
#include "TH1F.h"

#include "RhoCandList.h"
#include "RhoCandidate.h"


//--------------------------------------------
// Collaborating class forward declarations --
// -------------------------------------------
#include "ArgList.h"

using std::cout;
using std::endl;

// -------------------------------------------------------------------------

PndListProvider::PndListProvider(std::string name,std::string pdgType)
{
  fName=name;
  fNDaughters=0;
  fIsGeneric=false;
  fToDump=false;
  fHistoFilled=false;
  fCandsUptodate=false;

  fHisto=0;

  fDaughterListNames.clear();
  fDaughterPDG.clear();

  fOwnList.Cleanup();

  fMinusSel = new RhoMinusParticleSelector;
  fPlusSel  = new RhoPlusParticleSelector;
  fMassSel  = 0;

  if (pdgType!="") {
    fParticlePDG=TDatabasePDG::Instance()->GetParticle(pdgType.c_str());
  } else {
    fParticlePDG=0;
  }
}

// -------------------------------------------------------------------------

PndListProvider::PndListProvider(std::string name,int pdgcode)
{
  fName=name;
  fNDaughters=0;
  fIsGeneric=false;
  fHistoFilled=false;
  fToDump=false;
  fCandsUptodate=false;

  fHisto=0;

  fDaughterListNames.clear();
  fDaughterPDG.clear();

  fOwnList.Cleanup();

  fMinusSel = new RhoMinusParticleSelector;
  fPlusSel  = new RhoPlusParticleSelector;
  fMassSel  = 0;

  if (pdgcode!=0) {
    fParticlePDG=TDatabasePDG::Instance()->GetParticle(pdgcode);
  } else {
    fParticlePDG=0;
  }
}

// -------------------------------------------------------------------------


PndListProvider::~PndListProvider()
{
  delete fMinusSel;
  delete fPlusSel;
  delete fMassSel;
  Reset();
}

// -------------------------------------------------------------------------

void PndListProvider::SetType(std::string pdgType)
{
  fParticlePDG=TDatabasePDG::Instance()->GetParticle(pdgType.c_str());
}

// -------------------------------------------------------------------------

void PndListProvider::SetType(int pdgcode)
{
  fParticlePDG=TDatabasePDG::Instance()->GetParticle(pdgcode);
}

// -------------------------------------------------------------------------

void PndListProvider::AddDaughterType(std::string dtype)
{
  fDaughterPDG.push_back(TDatabasePDG::Instance()->GetParticle(dtype.c_str()));
}

// -------------------------------------------------------------------------

void PndListProvider::AddDaughterType(int pdgcode)
{
  fDaughterPDG.push_back(TDatabasePDG::Instance()->GetParticle(pdgcode));
}

// -------------------------------------------------------------------------

int PndListProvider::GetDaughterType(int i)
{
  if ((int)fDaughterPDG.size()>i) {
    return fDaughterPDG[i]->PdgCode();
  }
  return -1;
}

// -------------------------------------------------------------------------

void PndListProvider::AddDecayProduct(std::string dname)
{
  fDaughterListNames.push_back(std::string(dname));
  fNDaughters++;
}

// -------------------------------------------------------------------------

void PndListProvider::Print()
{
  cout <<"[";
  if (fParticlePDG) { cout <<fParticlePDG->PdgCode(); }
  cout <<"]("<<fName<<")";
  if (!fIsGeneric) {
    cout <<" -> ";
    for (int i=0; i<fNDaughters; i++) {
      cout <<"[";
      if (fDaughterPDG[i]) { cout << fDaughterPointers[i]->GetType(); }//cout <<fDaughterPDG[i]->PdgCode();
      cout <<"]("<<fDaughterPointers[i]->GetName()<<")";//fDaughterListNames[i]<<") ";
      if (fDaughterPointers[i]->IsGeneric()) { cout<<"G "; }
      else { cout <<" "; }
    }
  }
  cout <<endl;
}

// -------------------------------------------------------------------------

void PndListProvider::Reset()
{
  fOwnList.Cleanup();
  fCandsUptodate=false;
  fHistoFilled=false;
}

// -------------------------------------------------------------------------

void PndListProvider::SetCandList(RhoCandList& cl)
{
  Reset();

  if (fParticlePDG->AntiParticle()) {
    if (fParticlePDG->Charge()<0) {
      fOwnList.Append(cl,fMinusSel);
    } else {
      fOwnList.Append(cl,fPlusSel);
    }
  } else {
    fOwnList=cl;
  }

  fCandsUptodate=true;
}

// -------------------------------------------------------------------------

int PndListProvider::GetNCandidates()
{
  //if (fOwnList && fAntiOwnList) return (fOwnList->GetLength()+fAntiOwnList->GetLength());
  //else
  return fOwnList.GetLength();
}

// -------------------------------------------------------------------------

void
PndListProvider::GetCandList(RhoCandList& tcl)
{
  tcl.Cleanup();

  if (!fCandsUptodate) {
    // ********* 2 Daughters
    if (fNDaughters==2) {
      RhoCandList fD1List,fD2List;
      fDaughterPointers[0]->GetCandList(fD1List);
      if (fDaughterPointers[0]!=fDaughterPointers[1]) {
        fDaughterPointers[1]->GetCandList(fD2List);
        fOwnList.Combine(fD1List,fD2List);
      } else {
        fOwnList.Combine(fD1List,fD1List);
      }

      //cout <<"d1:"<<fD1List.GetLength()<<"  d2:"<<fD1List.GetLength()<<endl;
    }
    // ********* 3 Daughters
    else if (fNDaughters==3) {
      RhoCandList fDList[3];
      PndListProvider* lp[3];
      for (int i=0; i<3; i++) {
        lp[i]=fDaughterPointers[i];
        lp[i]->GetCandList(fDList[i]);
      }
      //3 different lists
      if (lp[0]!=lp[1] && lp[1]!=lp[2] && lp[0]!=lp[2]) {
        RhoCandList tmpL;
        tmpL.Combine(fDList[0],fDList[1]);
        fOwnList.Combine(tmpL,fDList[2]);
      }
      //at least 2 different lists
      else if (lp[0]!=lp[1] || lp[1]!=lp[2] || lp[0]!=lp[2]) {
        int id1=-1,id2=-1;
        if (lp[0]==lp[1] && lp[0]!=lp[2]) {id1=0; id2=2;}
        if (lp[0]==lp[2] && lp[0]!=lp[1]) {id1=0; id2=1;}
        if (lp[1]==lp[2] && lp[0]!=lp[1]) {id1=1; id2=0;}

        //exact 2 list are the same
        if (id1!=-1 && id2!=-1) {
          RhoCandList tmpL;
          tmpL.Combine(fDList[id1],fDList[id1]);
          fOwnList.Combine(tmpL,fDList[id2]);
        }
      }
      //all 3 lists are the same
      else {
        /*
        RhoCandList tmpL;

            tmpL.Combine(fDList[0],fDList[0]);
            fOwnList.Combine(tmpL,fDList[0]);
            //fOwnList.RemoveClones();
            */

        TLorentzVector vl;
        Double_t charge;
        Bool_t nearby = kTRUE;
        RhoCandList l1;
        lp[0]->GetCandList(l1);

        int endpos = l1.GetNumberOfTracks();

        //combination of a list with itself 3x
        for (Int_t comb_i=0; comb_i<endpos; ++comb_i) {
          for (Int_t comb_j=comb_i+1; comb_j<endpos; ++comb_j) {
            for (Int_t comb_k=comb_j+1; comb_k<endpos; ++comb_k) {
              if (    !l1[comb_i]->Overlaps( l1[comb_j] )
                      && !l1[comb_j]->Overlaps( l1[comb_k] )
                      && !l1[comb_i]->Overlaps( l1[comb_k] )  ) {
                vl=l1[comb_i]->P4()+l1[comb_j]->P4()+l1[comb_k]->P4();
                charge=l1[comb_i]->Charge()+l1[comb_j]->Charge()+l1[comb_k]->Charge();
                //if (selector) nearby = selector->Accept(l1[comb_i],l2[comb_k]);
                if (nearby) {
                  //fill list with new candidate
                  RhoCandidate c(vl,charge);
                  c.SetMarker(l1[comb_i]->GetMarker(0)|l1[comb_j]->GetMarker(0)|l1[comb_k]->GetMarker(0),0);
                  c.SetMarker(l1[comb_i]->GetMarker(1)|l1[comb_j]->GetMarker(1)|l1[comb_k]->GetMarker(1),1);
                  c.SetMarker(l1[comb_i]->GetMarker(2)|l1[comb_j]->GetMarker(2)|l1[comb_k]->GetMarker(2),2);
                  c.SetMarker(l1[comb_i]->GetMarker(3)|l1[comb_j]->GetMarker(3)|l1[comb_k]->GetMarker(3),3);
                  //if (selector!=0)
                  //{
                  //  c.SetPosition(selector->GetVertex());
                  //  c.SetVect(selector->GetMomentum());
                  //  c.SetEnergy(c.E());
                  //}
                  // *************** modified by K Goetzen
                  c.AddDaughterLinkSimple(l1[comb_i]);
                  c.AddDaughterLinkSimple(l1[comb_j]);
                  c.AddDaughterLinkSimple(l1[comb_k]);
                  // ****************
                  fOwnList.Put(&c);
                } //if nearby
              }// overlap
            }//for k
          }//for j
        }// for i

      }//else 3x same

    }

    if (fHisto)
      for (int i=0; i<fOwnList.GetLength(); ++i) { fHisto->Fill(fOwnList[i]->Mass()); }

    if (fMassSel) { fOwnList.Select(fMassSel); }

    fCandsUptodate=true;
  }


  //cout <<fName<<":"<<fParticlePDG->PdgCode()<<endl;
  //cout <<fOwnList<<endl;

  tcl=fOwnList;
}

// -------------------------------------------------------------------------


std::string
PndListProvider::GetDecayProdName(int i)
{
  if (i<fNDaughters) {
    return fDaughterListNames[i];
  } else { return ""; }
}

// -------------------------------------------------------------------------

void PndListProvider::AddDaughterPointer(PndListProvider* p)
{
  if (p) { fDaughterPointers.push_back(p); }
}

// -------------------------------------------------------------------------

int
PndListProvider::GetType()
{
  if (fParticlePDG) { return fParticlePDG->PdgCode(); }
  else { return 0; }
}

// -------------------------------------------------------------------------

void PndListProvider::FillHisto()
{
  if (fHisto && !fHistoFilled) {
    for (int i=0; i<fOwnList.GetLength(); ++i) { fHisto->Fill(fOwnList[i]->Mass()); }
    fHistoFilled=true;
  }
}

// -------------------------------------------------------------------------

void PndListProvider::SetMassSelector(double mean, double width)
{
  if (!fMassSel) {
    fMassSel=new RhoMassParticleSelector((fName+"sel").c_str(),mean,width);
  }
}


/*
void PndListProvider::SetHisto(double min, double max, int bins)
{
  fFillHisto=true;
  double tmp=0;
  if (min>max)
  {
    tmp=min;
    min=max;
    max=tmp;
  }
  fHisto=new TH1F(fName,fName,bins,min,max);
}
*/
