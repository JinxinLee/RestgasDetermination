#ifndef VABSVERTEXALGORITHM_H
#define VABSVERTEXALGORITHM_H
//////////////////////////////////////////////////////////////////////////
//                                                                      //
// VAbsVertexAlgorithm							//
//                                                                      //
// Abstract interface of the vertexing algorithms			//
//                                                                      //
// Author: Marcel Kunze, RUB, Mar. 99					//
// Copyright (C) 1999-2001, Ruhr-University Bochum.			//
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#include "TObject.h"

// forward declarations
class TCandidate;
class VAbsVertex;
class TLorentzVector;
//class TMatrixD;
class TVector3;

class VAbsVertexAlgorithm : public TObject
{
    
public:

  // A VtxAbsAlgorithm is completely specified by
  //  {algMode,algId,flyResonances}

  // type of algorithm
  enum algMode{Fast,Standard,SingleTrack};
  enum algId{Unknown=0,Add4,LeastChi,GeoKin,FastVtx,GammaConv,
             TreeFitter,Cascade,NAlgorithms};

  // These public static members are provided so that a client can
  // unambiguously specify a unique algorithm if in doubt about one
  // of the values

  static const algMode  theDefaultAlgMode;
  static const algId    theDefaultAlgId;
//  static const bool     theDefaultFlyResonancesSetting;
    
public:
    
    // constructor
    VAbsVertexAlgorithm();
    
    // destructor
    virtual ~VAbsVertexAlgorithm();
    
    /**
    * This function has to be overridden by the real algoritms implementations
    */
    virtual VAbsVertex* Compute(const TCandidate* decayTree ) =0;
    
    // momentum of the decaying state
    virtual TLorentzVector   P4() const =0;
    virtual TMatrixD&        P4Err() =0; 
    virtual Double_t Chi2Contribution(const TCandidate&bc) const; // chi2 contribution of single daughters.
    
    // momentum and position of refitted daughters
    // these 5 functions must be overriden by the fitters
    // the default implementation returns the initial values
    virtual TLorentzVector   P4Daughters(const TCandidate &bc);
    virtual TMatrixD&        P4CovDaughters(const TCandidate &bc);
    virtual TVector3         PosDaughters(const TCandidate &bc);
    virtual TMatrixD&        PosCovDaughters(const TCandidate &bc);
    virtual TMatrixD&        PosP4CovDaughters(const TCandidate &bc);
    virtual TMatrixD&        Cov7Daughters(const TCandidate &bc);
    
    // modifiers
    void SetMode( algMode mode ) { fMode = mode; }
    void SetFlyResonances()      { fFlyResonances = kTRUE; }
    void SetNoFlyResonances()    { fFlyResonances = kFALSE; }
    
    // accessors
    algMode Mode()       const { return fMode; }
    Bool_t FlyResonances() const { return fFlyResonances; }
    
protected:
    
    // data members
    algMode fMode;
    Bool_t  fFlyResonances;
    
public:
    ClassDef(VAbsVertexAlgorithm,1) //Base class for fitter algorithms
};

#endif 


