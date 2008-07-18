//-----------------------------------------------------------
// File and Version Information:
// $Id$
//
// Description:
//      Inherits from the abstract class PndTpcAbsPadShape.
//      This class defines the shape of a square pad with the
//      origin in the centre
//
// Environment:
//      Software developed for the PANDA Detector at FAIR.
//
// Author List:
//      Cristoforo Simonetto    TUM            (original author)
//
//
//-----------------------------------------------------------

#ifndef TPCPADSHAPESQUARE_HH
#define TPCPADSHAPESQUARE_HH

// Base Class Headers ----------------
#include "PndTpcAbsPadShape.h"

// Collaborating Class Headers -------


// Collaborating Class Declarations --



class PndTpcPadShapeSquare: public PndTpcAbsPadShape {
public:

  // Constructors/Destructors ---------
  PndTpcPadShapeSquare(const double _d,
		       const unsigned int _ID);
  virtual ~PndTpcPadShapeSquare();

  // Operators
  friend std::ostream& operator<<(std::ostream& s,
				  const PndTpcPadShapeSquare& me);

  // Accessors -----------------------
  virtual void EvalBoundingRect(double& width, double& height,
				const double angle) const;
  virtual bool Contains(const double xp, const double yp) const;
  virtual int GetNBoundaryPoints() const {return(4);};
  virtual void GetBoundaryPoint(const int index,
				double& x, double& y) const;

private:

  // Private Data Members ------------
  double d; //length of a size

  // Private Methods -----------------

};

#endif

//--------------------------------------------------------------
// $Log$
//--------------------------------------------------------------
