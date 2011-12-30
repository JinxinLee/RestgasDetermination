#ifndef VABSPIDSELECTOR_H
#define VABSPIDSELECTOR_H
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

#include <iostream>

#include "TNamed.h"
#include "RhoBase/TParm.h"

class TCandidate;
class TCandList;
class VAbsMicroCandidate;
class TAssociator;
class TPidQuality;
class TParticlePDG;

enum criterion{best,veryLoose,loose,tight,veryTight,variable,all};

class VAbsPidSelector : public TNamed {
    
public:
    
    //Constructor
    VAbsPidSelector(const char *name="VAbsPidSelector",const char *type = 0);
    //Destructor
    virtual ~VAbsPidSelector();
    
    void Initialize();

    //operations
    virtual Bool_t Accept(TCandidate&) = 0;
    virtual Bool_t Accept(VAbsMicroCandidate&) = 0;

    virtual void SetParms() {}
    virtual void SetCriterion(const char* crit);
    void SetCriterion(criterion crit);
    void Select(TCandList &l);
    void Select(TCandList &in,TCandList &out);

    void SetParm(const char *key, Bool_t value);
    void SetParm(const char *key, Double_t value);
    void SetParm(const char *key, const char *value);
    Bool_t GetBoolParm(const char *key);
    TString GetStringParm(const char *key);
    Double_t GetParm(const char *key);
    Double_t GetParmValue(const char *key);

    TAssociator* GetAssociator() const;
    void SetAssociator(TAssociator *assoc);

protected:    
    TParticlePDG* CPConjugate( TParticlePDG* aPart );

    void SetTypeAndMass(TCandidate &b);

    TParticlePDG	*fTypePlus;	//!Particle type to accept
    TParticlePDG	*fTypeMinus;	//!Particle type to accept
    criterion           fCriterion;	//!Criterion to apply for selection
    TPidQuality	*fQC;		//!Produce quality plots
    static TAssociator* fAssociator;  //!MC association
    static TParm	fParms;		//!Parameter store

public:
    ClassDef(VAbsPidSelector,1) // Particle Selector base class
};  

#endif
