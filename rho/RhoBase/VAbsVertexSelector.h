#ifndef VABSVERTEXSELECTOR_H
#define VABSVERTEXSELECTOR_H
//////////////////////////////////////////////////////////////////////////
//                                                                      //
// VAbsVertexSelector	    						//
//                                                                      //
// Abstract Selector classes for vertexing				//
//                                                                      //
// Author: Marcel Kunze, RUB, July 99					//
// Copyright (C) 1999-2001, Ruhr-University Bochum.			//
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#include <iostream>

#include "TNamed.h"
#include "TVector3.h"

class TCandidate;
class TCandList;

class VAbsVertexSelector : public TNamed {
    
public:
    
    //Constructor
    VAbsVertexSelector(const char *name="VAbsVertexSelector");
    //Destructor
    virtual ~VAbsVertexSelector();
    
    //operations
    virtual Bool_t Accept(TCandidate&,TCandidate&) = 0;
    void SetDistanceOfClosestApproach(Double_t d) { fDoca = d; }
    void SetAngle(Double_t d) { fVtxip = d; }
    void SetRadius(Double_t r1, Double_t r2 = 1.E8) { fRmin = r1; fRmax = r2;}
    void SetPrimaryVertex(TVector3 x) { fPrimaryVertex = x; }
    TVector3 GetVertex() { return fVertex; }
    Double_t GetDistanceOfClosestApproach() { return fActualDoca; }
    Double_t GetAngle() { return fActualVtxip; }
    virtual void PrintOn( std::ostream& o=std::cout ) const;

    const TVector3& GetMomentumA() const {return fMomA;}
    const TVector3& GetMomentumB() const {return fMomB;}
    TVector3 GetMomentum() const { return fMomA+fMomB;}
    Double_t GetRadius() const {return fActualR;}
    void Combine(TCandList &in1,TCandList &in2,TCandList &out);
    
protected:    
  Double_t	fDoca;		    // Distance of closest approach
  Double_t	fVtxip;		    // Angle between momentum and (IP->Vertex)
  Double_t      fRmin;		    // Inner radius of acceptance
  Double_t      fRmax;		    // Outer radius of acceptance
  Double_t      fActualR;           // Calculated distance to IP
  Double_t	fActualVtxip;	    // Calculated angle
  Double_t	fActualDoca;	    // Calculated doca
  TVector3      fVertex;	    // Reconstructed vertex
  TVector3	fPrimaryVertex;	    // Origin
  TVector3      fMomA,fMomB;	    // P3 of the two candidates  at poca

public:
    ClassDef(VAbsVertexSelector,1) // Vertexing base class
};  

std::ostream&  operator << (std::ostream& o, const VAbsVertexSelector&);

#endif
