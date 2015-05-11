#ifndef RHOFITTERBASE_H
#define RHOFITTERBASE_H
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

#include<map>
#include "TObject.h"
#include "TVectorD.h"
#include "TMath.h"
#include "RhoCandidate.h"

class RhoFitterBase: public TObject // TOperatorBase
{
  public:

    /**
    *    constructor
    */
    RhoFitterBase() {};
    RhoFitterBase ( RhoCandidate* decayTree );
    // copy constructor
    RhoFitterBase ( const RhoFitterBase& );

    // destructor
    ~RhoFitterBase();

    /**
    * This function has to be overridden by the real algoritms implementations
    */
    Bool_t Fit();                            // fit only the head composite (node)
    Bool_t FitAll();                         // fit composites and refitted daughters. FMV, 07/21/99.

    /**
    *   access to the fitted candidates
    */
//    const RhoCandidate* FittedCand ( const RhoCandidate& ) const;
//    RhoCandidate GetFitted ( const RhoCandidate& ) const;
    double Chi2Contribution ( const RhoCandidate* ) ;
    double GetChi2() const {return fChiSquare;};
    int    GetNdf()  const {return fNDegreesOfFreedom;};
    double GetProb() const {return TMath::Prob(fChiSquare,fNDegreesOfFreedom);};

    inline void SetVerbose(Bool_t v=kTRUE) {fVerbose=v;}

  protected:

    // the decay tree
    RhoCandidate& HeadOfTree() const { return *fHeadOfTree; }
    Bool_t fVerbose;
    virtual Bool_t FitNode(RhoCandidate* b);                 // fit one decay in the tree. 
    
    // the clone of the tree.
    RhoCandidate* fHeadOfTree;                      //! uppermost particle composite in tree
    RhoCandidate* CopyCand ( RhoCandidate* );
    //Bool_t CopyDaughters ( RhoCandidate* );
    RhoCandidate* CopyTree ( RhoCandidate* );
    void InsertChi2 ( const RhoCandidate* bc,const double chi2 ) {fChi2Map[ bc->Uid()] = chi2;}
    
    //Helpers for final state fitting
    std::vector<RhoCandidate*>   fDaughters;  //!
    void SetDaugthersFromComposite(RhoCandidate* cand);
    void FindAndAddFinalStateDaughters(RhoCandidate* cand);
    void SetFourMomentumByDaughters(RhoCandidate* composite);
    void SetDecayVertex(RhoCandidate* composite, const TVector3& vtx, const TMatrixD& CovVV);
    double fChiSquare; //global chi^2 value
    int fNDegreesOfFreedom; //degrees of freedom


  private:
    // helper function
    std::map<Int_t,Double_t> fChi2Map; //!<! each particle's contribution to the chi^2
    Bool_t IterateAndFit(RhoCandidate* b);  // iterate the decay tree and fit each node recoursively

    
  public:
    ClassDef ( RhoFitterBase,1 ) //Base class for fitters
};

#endif
