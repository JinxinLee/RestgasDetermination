//////////////////////////////////////////////////////////////////////////
//                                                                      //
// VAbsVertex								//
//                                                                      //
// Abstract Definition of the vertex					//
//                                                                      //
// Author: Marcel Kunze, RUB, Mar. 99					//
// Copyright (C) 1999-2001, Ruhr-University Bochum.			//
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#ifndef VABSVERTEX_H
#define VABSVERTEX_H

#include <iostream>

#include "TObject.h"
#include "TVector3.h"
#include "TLorentzVector.h"
#include "TMatrixD.h"

class TCandidate;
class TCandList;

//		---------------------
// 		-- Class Interface --
//		---------------------

class VAbsVertex : public TObject 
{
    
public:
    
    // status enum RF 8/14/98
    enum VtxStatus{ Success=0, NonConverged, BadInput, Failed, UnFitted };
    
    // type vertex enum FMV 01/24/00
    enum VtxType{ None=-1, Geometric, Kinematic }; 
    
private:
    
    //--------------------
    // Instance Members --
    //--------------------
    
    // status
    VtxStatus       fStatus; //!
    
    // type
    VtxType         fType; //!
    
    //
    // unowned links to incoming and outgoing candidates 
    //
    
    // _THE_ incoming candidate :
    // either  a non-resonance composite candidate
    // or a resonance candidate with no mother
    TCandidate* fInComingCand; //!
    
    // Outgoing candidates
    // either non composite candidates
    // or non-resonance composite candidate
    int nOutGoingCands; //!
    TCandidate *fOutGoingCands[5]; //! Can not be streamed
    
    // Resonances are produced and are decayed at the vertex
    // and they necessarily have a mother
    int nResonances; //!
    TCandidate *fResonances[5]; //!
    
public:
    
    // Constructors expect to be provided by lower classes
    // VAbsVertex(const TLorentzVector &v );
    VAbsVertex();
    VAbsVertex( const VAbsVertex& );
    
    // Destructor
    virtual ~VAbsVertex( );
    
    // Operations
    virtual void PrintOn( std::ostream& o=std::cout ) const;

    void SetResonances(TCandidate *c) { fResonances[nResonances++] = c; }
    void SetOutGoingCands(TCandidate *c) { fOutGoingCands[nOutGoingCands++] = c; }
    TCandidate& GetResonances(Int_t i) { return *fResonances[i]; }
    TCandidate& GetOutGoingCands(Int_t i) { return *fOutGoingCands[i]; }
    TCandidate& GetInComingCand() { return *fInComingCand; }
    void SetInComingCand(TCandidate *cand) { fInComingCand = cand; }
    
    //
    // Pure virtual accessors to contained information
    //        to be defined in subclasses
    //
    
    // position of the vertex in space and time
    virtual TLorentzVector V4()      const =0;
    
    // covariance matrix
    virtual TMatrixD& Covariance () const =0;
    
    // chi squared and number of dof of the vertex fit
    virtual double ChiSquared()        const =0;
    virtual int NDof()                 const =0;
    
    //
    // non-virtual accessors
    //
    
    // status of the fit
    VtxStatus    Status() const;
    
    // type of the fit
    VtxType    Type() const;
    
    // vertex position
    TVector3     Point()  const;
    
    // 4-momentum of the incoming particle
    TLorentzVector P4() const;
    
    // mass of the incoming particle
    double Mass() const;
    
    // momentum of the incoming particle
    double P() const;
    
    // number of out going candidates
    int NOutGoing() const;
    
    // number of resonances (states produced and decayed at that vertex)
    int NResonance() const;
    
    // incoming candidate
    const TCandidate* InComingCand() const;
    
    // mother particle
    // that's the incoming particle if not a resonance,
    // or the resonance at the top of the decay tree
    const TCandidate* TheMother() const;
    
    // Interface to vertexing algorithms (M.Bondioli & M.Carpinelli 7/17/98)
    // accessors to weight matrices defined as the 2nd derivative of   
    // chi2 wrt x and p.
    virtual TMatrixD&  XXWeight() const;
    virtual TMatrixD&  PPWeight() const;
    virtual TMatrixD&  XPWeight() const;
    virtual void       GetAllWeights(TVector3& xv,
				     TVector3& pv,
				     TMatrixD& xxWeight,
				     TMatrixD& ppWeight,
				     TMatrixD&    xpWeight) const;
    virtual TMatrixD&  XXCov() const;
    virtual TMatrixD&  PPCov() const;
    virtual TMatrixD&  XPCov() const;
    //
    // Modifiers
    //
    void SetStatus( VtxStatus );
    void SetType( VtxType );
    
private:
    
    // Operators
    VAbsVertex& operator=( const VAbsVertex& ) 
    { 
	//    abort(); 
	return *this; 
    }
    
public:
    ClassDef(VAbsVertex,1) // Vertex base class
	
};

// standalone print
std::ostream&  operator << (std::ostream& o, const VAbsVertex&);

#endif




