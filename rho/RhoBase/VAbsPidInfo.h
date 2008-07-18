#ifndef VABSPIDINFO_H
#define VABSPIDINFO_H
//////////////////////////////////////////////////////////////////////////
//                                                                      //
// VAbsPidInfo								//
//                                                                      //
// Particle identification info class					//
// Abstract definition of the PidInfo database				//
//                                                                      //
// Author: Marcel Kunze, RUB, Nov. 1999					//
// Copyright (C) 1999-2001, Ruhr-University Bochum.			//
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#define CONS_OK 0
#define CONS_noMeasure 1
#define CONS_underFlow 2
#define CONS_unPhysical 3

#define CONS_left 1
#define CONS_right 2
#define CONS_unknown 0

#include <iostream>
#include "TObject.h"

class VAbsPidInfo : public TObject {
 
  public :
    VAbsPidInfo();
    virtual ~VAbsPidInfo() { };

    // Access significance levels
    virtual const Float_t* GetSignificance() const = 0;
    virtual Float_t GetSignificance(Int_t hypo) const =0;

    // Access likelihoods
    virtual const Float_t* GetLikelihood() const  = 0;
    virtual Float_t GetLikelihood(Int_t hypo) const  =  0;

    // Access statusbits
    virtual Int_t GetStatus(Int_t hypo) const =  0;
    virtual Int_t GetSign(Int_t hypo) const  =  0;
    
    // Check validity
    virtual Int_t IsValid() const  =  0;
    virtual UShort_t GetStatus() const =0;

    virtual void SetNoMeas()=0;
    virtual void Invalidate()=0;

    // dump
    virtual void PrintOn( std::ostream& o=std::cout ) const  = 0;

  public :
    ClassDef(VAbsPidInfo,1) // Abstract base class for PID infos

};

#endif
