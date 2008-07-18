#ifndef VABSFITTER_H
#define VABSFITTER_H
//////////////////////////////////////////////////////////////////////////
//                                                                      //
// VAbsFitter				    				//
//                                                                      //
// Abstract class for vertexers						//
//                                                                      //
// Author: Marcel Kunze, RUB, Feb. 99					//
// Copyright (C) 1999-2001, Ruhr-University Bochum.			//
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#include "TVectorD.h"
#include "RhoTools/TOperatorBase.h"

class TParticlePDG;
class TCandidate;
class TVertex;

class VAbsFitter: public TOperatorBase
{
public:
    
    /**
    *    constructor
    */
    VAbsFitter() {};
    VAbsFitter( const TCandidate& decayTree );
    // copy constructor
    VAbsFitter( const VAbsFitter&  );
    
    // destructor
    virtual ~VAbsFitter();
    
    /**
    * This function has to be overridden by the real algoritms implementations
    */
    virtual void Fit()=0;                  // fit only composites
    virtual void FitAll();                 // fit composites and refitted daughters. FMV, 07/21/99.
    
    /**
    *   access to the fitted candidates 
    */
    const TCandidate* FittedCand( const TCandidate& ) const;
    TCandidate GetFitted( const TCandidate& ) const;
    double Chi2Contribution( const TCandidate& ) const;
    void InsertChi2(const TCandidate& bc,const double& chi2){fChi2Map(bc.Uid()) = chi2;}
    
    inline void SetVerbose() {fVerbose=kTRUE;}
    
protected:
    
    // the decay tree
    TCandidate& HeadOfTree() const { return *fHeadOfTree; }
    Bool_t fVerbose;
    
     // the clone of the tree.
    TCandidate* fHeadOfTree;
   
    
private:
    
    
    // helper function
    TCandidate* CopyTree( const TCandidate& );
    TVectorD fChi2Map;
    
public:
    ClassDef(VAbsFitter,1) //Base class for fitters
};

#endif 
