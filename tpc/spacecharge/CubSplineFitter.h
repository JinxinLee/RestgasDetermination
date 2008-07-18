//-----------------------------------------------------------
// File and Version Information:
// $Id$
//
// Description:
//      Algorithm for fitting a 1-dimensional CubSpline to
//      a set of data f(x) using Householder Transformations
//
//
// Environment:
//      Software developed for the PANDA Detector at FAIR.
//
// Author List:
//      Felix Boehmer        TUM            (original author)
//
//
//-----------------------------------------------------------


#ifndef CUBSPLINEFITTER_H
#define CUBSPLINEFITTER_H

// Collaborating Class Headers -------
#include "CubSpline.h"
#include "TVector3.h"
#include "TMatrixD.h"
#include "TArrayD.h"
#include "TDecompQRH.h"
#include <vector>



// Collaborating Class Declarations --



class CubSplineFitter {
public:

  // Constructors/Destructors ------------------------------------
  CubSplineFitter(CubSpline* CSP, 
		  const std::vector<std::vector<double>*>* data = NULL);
                  //takes data as doublets (coordinate, value)

  ~CubSplineFitter();


  // Accessors ---------------------------------------------------
  TMatrixD* getSpMatrix() {return _matrix;}
  //returns the matrix with removed empty columns:
  TMatrixD* getFixedMatrix() {return _newmat;}
  const TMatrixD* getQ() {return _Q;}
  const TMatrixD* getR() {return _R;}

  TDecompQRH* getDecomQRH() {return _householder;}
  //returns the list of removed row indices:
  std::vector<int>* getRC() {return &_rc;} 

  // Methods -----------------------------------------------------
  void decompose(double TOL=0.0);
  TVectorD solve(const TVectorD&);

private:
  
  TMatrixD* _matrix;           //the spline-matrix
  TMatrixD* _newmat;            //the spline-matrix without empty columns
  const TMatrixD* _Q;          //product of Householder matrices
  const TMatrixD* _R;          //upper triangular matrix
  const TVectorD* _Up; 
  const TVectorD* _W; 
  CubSpline* _CSP;
  TArrayD _mData;              //temp _matrix data
  const std::vector<std::vector<double>*>* _data;
  TDecompQRH* _householder;

  std::vector<int> _rc;         //removed coeff indices
  bool _decomp;
  bool _emptycols;

};

#endif
