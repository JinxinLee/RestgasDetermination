//--------------------------------------------------------------------------
// File and Version Information:
// 	$Id: TDiagVertex.cxx,v 1.3 2002-02-01 23:00:10 marcel Exp $
//
// Description:
//	Class TOperator
//
//	Implementation, adds 4vectors at origin
//
// Environment:
//	Software developed for the BaBar Detector at the SLAC B-Factory.
//
// Author List:
//	Bob Jacobsen     		Original Author
//
// Copyright Information:
//	Copyright (C) 1996		Lawrence Berkeley Laboratory
//
// ROOT Version by Marcel Kunze, RUB
//------------------------------------------------------------------------

#include "RhoTools/TDiagVertex.h"
#include <assert.h>
#include "RhoBase/TCandidate.h"

ClassImp(TDiagVertex)

TBuffer &operator>>(TBuffer &buf, TDiagVertex *&obj)
{
   obj = (TDiagVertex *) buf.ReadObject(TDiagVertex::Class());
   return buf;
}

#include <iostream>
using namespace std;

TDiagVertex::TDiagVertex() 
  :_thePoint(), _theT(0.), _theChi2(0.), _nDof(0){
}

TDiagVertex::TDiagVertex( const TVector3& p, double t,
			      double sx, double sy, double sz, 
			      double chi2, int ndof) 
  :_thePoint(p), _theT(t), _theChi2(chi2), _nDof(ndof){
    SetError(sx,sy,sz);
}

TDiagVertex::TDiagVertex( double x, double y, double z, double t,
			      double sx, double sy, double sz, 
			      double chi2, int ndof )
  :_thePoint( TVector3(x, y, z) ), _theT(t),_theChi2(chi2), _nDof(ndof){
    SetError(sx,sy,sz);
}

TDiagVertex::TDiagVertex( const TPointErr& p, double t,
			      double chi2, int ndof)
  :_thePoint( p ),_theT(t),_theChi2(chi2), _nDof(ndof){
}

TDiagVertex::TDiagVertex( const TDiagVertex& o )
  : VAbsVertex( o ), _thePoint(o._thePoint), _theT(o._theT),
    _theChi2(o._theChi2), _nDof(o._nDof){
}


//--------------
// Destructor --
//--------------

TDiagVertex::~TDiagVertex()
{
}

TLorentzVector 
TDiagVertex::V4() const 
{ 
  return TLorentzVector( _thePoint.X(), _thePoint.Y(), _thePoint.Z(), _theT ); 
}

TMatrixD& 
TDiagVertex::Covariance() const 
{ 
  return (TMatrixD&) _thePoint.CovMatrix(); 
}

double 
TDiagVertex::ChiSquared() const 
{ 
  return _theChi2; 
}

int 
TDiagVertex::NDof() const 
{ 
  return _nDof; 
}

void 
TDiagVertex::Persist( TVertexPersister& aPersister ) const 
{}

		       
void 
TDiagVertex::SetError(double sx, double sy, double sz){
  TError errMat(3);
  errMat(0,0) = sx;
  errMat(1,1) = sy;
  errMat(2,2) = sy;
  _thePoint.SetCovMatrix(errMat);
}







