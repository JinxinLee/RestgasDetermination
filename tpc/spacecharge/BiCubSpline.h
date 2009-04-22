//-----------------------------------------------------------
// File and Version Information:
// $Id$
//
// Description:
//      BiCubic spline on 2 B-Spline bases
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

#ifndef BICUBSPLINE_HH
#define BICUBSPLINE_HH

// Base Class Headers ----------------


// Collaborating Class Headers -------
#include "BSpline.h"
#include <vector>
#include "TObject.h"


// Collaborating Class Declarations --


class TF2;
class SplineTF2Interface;

class BiCubSpline : public TObject {

friend class BiCubSplineFitter;

public:

  // Constructors/Destructors ---------
  BiCubSpline();
  BiCubSpline(const std::vector<double>* kx,
	      const std::vector<double>* ky,
	      std::vector<std::vector<double>*>* coeffs= NULL);
  virtual ~BiCubSpline();

    
  // Accessors -----------------------

  const std::vector<double>* getKx() {return _kx;}
  const std::vector<double>* getKy() {return _ky;}
  std::vector<std::vector<double>*>* getCoeffs() {return _coeffs;}

  // returns TF2 for area specified. Has to be inside Spline area!
  TF2* getTF2(double xMin, double xMax, double yMin, double yMax);

  // Modifiers -----------------------
 
  void setKx(const std::vector<double>* kx) {_kx = kx;}
  void setKy(const std::vector<double>* ky) {_ky = ky;}
  void setCoeffs(std::vector<std::vector<double>*>* c) {_coeffs = c;}

  //for ROOT interface:
  //comment: beware there is no possible consistency check since I need
  //to follow TF2's interface restrictions!
  //user is responsible for supplying the right array format
  void setCoeffsByArray(double*);  

  // Operations ----------------------

  double eval(double x, double y);
  

private:

  // Private Data Members ------------
  
  const std::vector<double>* _kx;    // knots in x and y direction
  const std::vector<double>* _ky;    
  std::vector<std::vector<double>*>* _coeffs;
  int _xLength, _yLength;         
  std::vector<BSpline*> _M;       // M(x)
  std::vector<BSpline*> _N;       // N(y)
  bool _xoutofrange;	          // flag for out of range
  bool _youtofrange;              

  TF2* _func;                     //!
  SplineTF2Interface* _ifc;       //!     
   
public:
  ClassDef(BiCubSpline,1)
  

};

#endif

//--------------------------------------------------------------
// $Log$
//--------------------------------------------------------------
