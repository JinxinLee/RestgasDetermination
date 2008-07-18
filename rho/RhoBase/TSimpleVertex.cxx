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

#include "RhoBase/TSimpleVertex.h"

ClassImp(TSimpleVertex)

#include <iostream>
using namespace std;

TSimpleVertex::TSimpleVertex() 
  : fV(), fT(0), fNDof(0), fChiSquared(999.)
{}

TSimpleVertex::TSimpleVertex( const TVector3& v, double t, int ndof, double c) 
  : fV(v), fT(t), fNDof(ndof), fChiSquared(c)
{}

TSimpleVertex::TSimpleVertex( double x, double y, double z, double t, int ndof, double c )
  : fV( x, y, z ), fT(t), fNDof(ndof), fChiSquared(c)
{}

TSimpleVertex::TSimpleVertex( const TSimpleVertex& o )
  : VAbsVertex( o ), fV(o.fV), fT(o.fT), fNDof(o.fNDof), fChiSquared(o.fChiSquared)
{}

TSimpleVertex::~TSimpleVertex()
{
}


TLorentzVector 
TSimpleVertex::V4() const 
{ 
  return TLorentzVector( fV.X(), fV.Y(), fV.Z(), fT ); 
}

TMatrixD& 
TSimpleVertex::Covariance() const 
{ 
  // set large errors
  static TMatrixD dum(3,3);
  int i,j;
  for (i = dum.GetRowLwb(); i <= dum.GetRowUpb(); i++)
    for(j = dum.GetColLwb(); j <= dum.GetColUpb(); j++)
       dum(i,j) = 1.0;
  dum(1,1)=1.e-04;
  dum(2,2)=1.e-04;
  dum(3,3)=1.e-04;
  return dum; 
}

double 
TSimpleVertex::ChiSquared() const 
{ 
  return fChiSquared; 
}

int 
TSimpleVertex::NDof() const 
{ 
  return fNDof; 
}

		       
