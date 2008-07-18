//--------------------------------------------------------------------------
// File and Version Information:
// 	$Id: TDiagVertex.h,v 1.1.1.1 2001-05-04 16:20:08 marcel Exp $
//
// Description:
//
//   The simpliest subclass of TAbsVertex
//	
// Environment:
//	Software developed for the BaBar Detector at the SLAC B-Factory.
//
// Author List:
//      Gautier Hamel de Monchenault
//	Gabriella Sciolla      		Original Author of TMcVertex
//
// Copyright Information:
//	Copyright (C) 1997	        SLAC 
//
// ROOT Version by Marcel Kunze, RUB
//------------------------------------------------------------------------


#ifndef TDIAGVERTEX_H
#define TDIAGVERTEX_H

//----------------------
// Base Class Headers --
//----------------------
#include "RhoBase/VAbsVertex.h"
#include "RhoMath/TPointErr.h"
class TCandidate;
class TVertexPersister;



class TDiagVertex  : public VAbsVertex {

  // instance members
  TPointErr _thePoint;
  double   _theT;
  double   _theChi2;
  int      _nDof;

public:

  // public interface
  
  // constructors
  TDiagVertex(); 

  TDiagVertex( double x, double y, double v, double t=0,
		 double sx=0., double sy=0., double sz=0., 
		 double chi2=0, int ndof=0);
  
  TDiagVertex( const TVector3& p, double t=0,
		 double sx=0., double sy=0., double sz=0., 
		 double chi2=0, int ndof=0); 

  TDiagVertex( const TPointErr& p, double t=0,		 
		 double chi2=0, int ndof=0); 
  
  TDiagVertex( const TDiagVertex& o );

  // destructor
  virtual ~TDiagVertex();
  
  // access
  virtual const TPointErr& pointErr() const{
    return _thePoint;
  }
  virtual TLorentzVector V4() const; 
  virtual TMatrixD& Covariance() const;
  virtual double ChiSquared() const;
  virtual int NDof() const;

  // not implemented yet
  virtual void Persist( TVertexPersister& aPersister ) const;

protected:

  // helpers
  void SetError(double sx=0, double sy=0, double sz=0);

public:
  ClassDef(TDiagVertex,1) //The simpliest subclass of VAbsVertex

};

#endif







