#ifndef VABSTRUTH_H
#define VABSTRUTH_H
//////////////////////////////////////////////////////////////////////////
//                                                                      //
// VAbsTruth								//
//                                                                      //
// Monte Carlo truth class						//
//									//
// Author List:								//
// Marcel Kunze, Bochum University, Aug. 99				//
// Copyright (C) 1999-2001, Ruhr-University Bochum.			//
//									//
//////////////////////////////////////////////////////////////////////////

#include <iostream>

#include "TObject.h"
#include "TObjArray.h"
#include "TLorentzVector.h"

class TDatabasePDG;
class TParticlePDG;

class VAbsTruth : public TObject {
    
public:
    VAbsTruth();
    virtual ~VAbsTruth();
    virtual void PrintOn( std::ostream& o=std::cout ) const = 0;

    virtual VAbsTruth*   GetMother()        const = 0;
    virtual TObjArray*   GetDaughterArray() const = 0;
    virtual VAbsTruth*   GetDaughter( const Int_t i ) const = 0;
    virtual Int_t        GetNumberOfDaughters() const = 0;
    
    virtual Int_t GetStatus()         const = 0;    // Particle status
    virtual TParticlePDG* GetPdtEntry()   const = 0; 
    virtual Double_t GetCharge()      const = 0;
    virtual Double_t GetPx()          const = 0;    // X momentum
    virtual Double_t GetPy()          const = 0;    // y momentum
    virtual Double_t GetPz()          const = 0;    // z momentum
    virtual Double_t GetEnergy()      const = 0;    // Track energy
    virtual Double_t GetMass()        const = 0;    // Particle mass
    virtual TLorentzVector GetMomentum() const = 0;
    virtual Double_t GetStartVx()     const = 0;    // x position of vertex
    virtual Double_t GetStartVy()     const = 0;    // y position of vertex
    virtual Double_t GetStartVz()     const = 0;    // z position of vertex
    virtual TVector3 GetStartVertex() const = 0;
    virtual Double_t GetLifeTime()    const = 0;    // Production time
    virtual Int_t GetAodMatchIndex()  const = 0;    // Track number of AOD Parter (if none: -1)
    virtual Double_t GetAodMatchConsistency()  const = 0;   // Track number of AOD Parter (if none: -1)
    
public:
    ClassDef(VAbsTruth,1) //Monte-Carlo truth
	
};      

// standalone print
std::ostream&  operator << (std::ostream& o, const VAbsTruth&);

#endif
