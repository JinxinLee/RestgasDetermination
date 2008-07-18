#ifndef TSIMPLEVERTEX_H
#define TSIMPLEVERTEX_H
//////////////////////////////////////////////////////////////////////////
//                                                                      //
// TSimpleVertex							//
//                                                                      //
// Simple vertex class							//
//									//
// Author List:								//
// M.Kunze, Bochum University, Feb. 01					//
// Copyright (C) 1999-2001, Ruhr-University Bochum.			//
//									//
//////////////////////////////////////////////////////////////////////////

#include "RhoBase/VAbsVertex.h"

class TCandidate;

class TSimpleVertex : public VAbsVertex
{
  TVector3 fV;
  Double_t fT;
  Double_t fChiSquared;
  Int_t    fNDof;

public:
  TSimpleVertex(); 
  TSimpleVertex( Double_t x, Double_t y, Double_t v, Double_t t=0, Int_t ndof = 0, Double_t c = 999.);
  TSimpleVertex( const TVector3& v, Double_t t=0, Int_t ndof = 0, Double_t c = 999.); 
  TSimpleVertex( const TSimpleVertex& o );
  virtual ~TSimpleVertex();
  virtual TLorentzVector V4() const; 
  virtual TMatrixD& Covariance () const;
  virtual Double_t ChiSquared() const;
  virtual Int_t NDof() const;

  ClassDef(TSimpleVertex,1) // The simpliest subclass of VAbsVertex
};

#endif

