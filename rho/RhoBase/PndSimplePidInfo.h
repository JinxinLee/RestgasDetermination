#ifndef PNDSIMPLEPIDINFO_H
#define PNDSIMPLEPIDINFO_H
//////////////////////////////////////////////////////////////////////////
//                                                                      //
// PndSimplePidInfo								//
//                                                                      //
// Particle identification info class					//
// Simple Implementation of PidInfo class				//
//                                                                      //
// Author: Klaus Goetzen, Nov. 2007					//
// Copyright (C) 2007 GSI                                         	//
//                                                                      //
//////////////////////////////////////////////////////////////////////////


#include <iostream>
#include "VAbsPidInfo.h"

class PndSimplePidInfo : public VAbsPidInfo {
 
  public :
     PndSimplePidInfo();
     ~PndSimplePidInfo() { };

    // Access significance levels
    virtual const Float_t* GetSignificance() {return fSignificance;}
    virtual Float_t GetSignificance(Int_t hypo){return fSignificance[hypo];}
    
    void SetLikelihood(Int_t hypo, Float_t llh);


    // Access likelihoods
    virtual const Float_t* GetLikelihood(){return fLikelihood;}
    virtual Float_t GetLikelihood(Int_t hypo) {return fLikelihood[hypo];}

    // Access statusbits
    virtual Int_t GetStatus(Int_t hypo) {return 0;}
    virtual Int_t GetSign(Int_t hypo){return 0;}
    
    // Check validity
    virtual Int_t IsValid(){return 0;}
    virtual UShort_t GetStatus(){return 0;}

    virtual void SetNoMeas(){};
    virtual void Invalidate(){};

    // dump
    virtual void PrintOn( std::ostream& o=std::cout );

  public :

    Float_t fSignificance[5];
    Float_t fLikelihood[5];

    ClassDef(PndSimplePidInfo,1) // Abstract base class for PID infos

};

#endif
