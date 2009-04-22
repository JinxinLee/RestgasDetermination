//-----------------------------------------------------------
// File and Version Information:
// Version 1.0  (Feb 27, 2008)
//
// Description:
//      Algorithm for fitting a 2-dimensional BiCubSpline to
//      a set of data g(x,y) using Householder Transformations
//      c.f. Hayes, Halliday 1973
//           J. Inst. Maths Applics (1974) 14, 89-103
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


#ifndef BICUBSPLINEFITTER_H
#define BICUBSPLINEFITTER_H

// Collaborating Class Headers -------
#include "TError.h"
#include "BiCubSpline.h"
#include "TVector3.h"
#include "TMatrixD.h"
#include "TArrayD.h"
#include "TDecompQRH.h"
#include <vector>

// Collaborating Class Declarations --


class BiCubSplineFitter {
public:

  // Constructors/Destructors ------------------------------------
  BiCubSplineFitter(BiCubSpline* BCSP, 
		    const std::vector<std::vector<double>*>* data);
                    //takes data as 4-element vectors of :
  	            //coordinates (x,y), value, error (ignored when zero)
  
  ~BiCubSplineFitter();


  // Accessors ---------------------------------------------------
  TMatrixD* getSpMatrix() {return _matrix;}
  TMatrixD* getFixedMatrix() {return _newmat;}
  TDecompQRH* getDecomp() {return _householder;}

  const BiCubSpline* getBCSP() {return _BCSP;}
  
  std::vector<int> getRC() {return _rc;} 
  
  
  // Methods -----------------------------------------------------
  
  //perform a QR decomposition of the Spline Matrix _matrix
  //using Householder transformations  
  //TOL specifies the tolerance boundary for small entries.
  void decompose(double TOL=0.0);

  //solve the problem A y = f
  //where A = Spline Matrix _matrix
  //y are the least square coefficents for the Spline
  //and f is the _data to fit
  //the result is returned as a TVectorD of length f, where the first 
  //entries are the proper coefficients and the rest contains the error.
  TVectorD solve();

private:
  
  const BiCubSpline* _BCSP;      //input Bi-Cubic Spline containing the
                                 //knot grid

  TMatrixD* _matrix;             //the spline-matrix
  TMatrixD* _newmat;             //_matrix with removed empty rows
  TArrayD _mData;                //temporary data to initialize the matrix

  std::vector<std::vector<double>*> _data; //input data to fit
  
  TDecompQRH* _householder;      //does the QR-decomposition

  std::vector<int> _rc;          //list of removed column indices
  bool _emptycols;               //flag for empty columns in _matrix

  void sortData();               //sorting on the input data using STL sort()
    
};

#endif
