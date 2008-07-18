//-----------------------------------------------------------
// File and Version Information:
// $Id$
//
// Description:
//      A covariance ellipse
//
//
// Environment:
//      Software developed for the PANDA Detector at FAIR.
//
// Author List:
//      Sebastian Neubert    TUM            (original author)
//
//
//-----------------------------------------------------------

#ifndef TCOVELLIPSE_HH
#define TCOVELLIPSE_HH

// Base Class Headers ----------------
#include "TEllipse.h"

// Collaborating Class Headers -------
#include "TMatrixT.h"

// Collaborating Class Declarations --



class TCovEllipse : public TEllipse{
public:

  // Constructors/Destructors ---------
  TCovEllipse();
  TCovEllipse(const TMatrixT<double>& cov, 
	      double x0=0, double y0=0,
	      int i=0, int j=1);
  virtual ~TCovEllipse(){}

  // Operators
  

  // Accessors -----------------------
  TMatrixT<double> getCov() const {return _cov;}

  // Modifiers -----------------------
  void setCov(const TMatrixT<double>& cov);
  void setMean(double x, double y);
  void select(int i, int j){_i=i;_j=j;recalc();}

  // Operations ----------------------

private:

  // Private Data Members ------------
  TMatrixT<double> _cov;
  double _sig;
  double _x0;
  double _y0;

  int _i,_j; // row and column index to be plotted

  // Private Methods -----------------
  void recalc();

public: 
  ClassDef(TCovEllipse,1)

};

#endif

//--------------------------------------------------------------
// $Log$
//--------------------------------------------------------------
