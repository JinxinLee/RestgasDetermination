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

#ifndef PNDLISTPROVIDER_H
#define PNDLISTPROVIDER_H

//----------------------
// Base Class Headers --
//----------------------

#include <string>

//-------------------------------
// Collaborating class Headers --
//-------------------------------


//--------------------------------------------
// Collaborating class forward declarations --
// -------------------------------------------
#include "ArgList.h"
#include "RhoBase/RhoCandList.h"

//typedef std::vector<PndListProvider*>;

class TParticlePDG;
class TDatabasePDG;
#include "RhoSelector/RhoMassParticleSelector.h"
#include "RhoSelector/RhoNeutralParticleSelector.h"
#include "RhoSelector/RhoPlusParticleSelector.h"
#include "RhoSelector/RhoMinusParticleSelector.h"
#include "RhoSelector/RhoSimpleElectronSelector.h"
#include "RhoSelector/RhoSimpleKaonSelector.h"
#include "RhoSelector/RhoSimpleMuonSelector.h"
#include "RhoSelector/RhoSimplePionSelector.h"
#include "RhoSelector/RhoSimpleProtonSelector.h"

class TH1F;


class PndListProvider
{
  public:

    //--------------------
    // Public interface --
    //--------------------

    //
    // Constructors
    //

    PndListProvider(std::string name, std::string pdgType="");
    PndListProvider(std::string name, int pdgcode);

    //
    // Destructor
    //
    virtual ~PndListProvider();

    //
    //    Modifiers + Accessors to contained information
    //

    std::string GetName() {return fName;}
    bool IsGeneric() {return fIsGeneric;}
    bool ToDump() {return fToDump;}

    void SetType(std::string pdgType);
    void SetType(int pdgcode);
    void SetGeneric(bool isgeneric=true) {fIsGeneric=isgeneric;}
    void SetToDump(bool todump=true) {fToDump=todump;}
    void SetHisto(TH1F* h) {fHisto=h;}
    void SetCandList(RhoCandList& cl);
    void SetMassSelector(double mean, double width);

    void AddDaughterPointer(PndListProvider* p);
    void AddDecayProduct(std::string dname);
    void AddDaughterType(std::string dtype);
    void AddDaughterType(int pdgcode);
    int  GetType();
    int  GetDaughterType(int i);
    std::string GetDecayProdName(int i);
    int  GetNDaughters() {return fNDaughters;}

    void GetCandList(RhoCandList& tl);
    int GetNCandidates();

    TH1F* GetHisto() {return fHisto;}
    void FillHisto();

    void Reset();
    void Print();


  private:
    std::string      fName;

    RhoCandList       fOwnList;

    RhoMinusParticleSelector*   fMinusSel;
    RhoPlusParticleSelector*    fPlusSel;
    RhoMassParticleSelector*    fMassSel;

    TH1F*             fHisto;

    TParticlePDG*    fParticlePDG;
    std::vector<TParticlePDG*> fDaughterPDG;
    ArgVector        fDaughterListNames;
    std::vector<PndListProvider*> fDaughterPointers;

    int              fNDaughters;

    bool             fIsGeneric;
    bool             fToDump;
    bool             fCandsUptodate;
    bool             fHistoFilled;

    TDatabasePDG*    fdbPdg;

};

#endif
