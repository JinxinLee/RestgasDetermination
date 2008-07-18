//-----------------------------------------------------------
// File and Version Information:
// $Id$
//
// Description:
//      Inherits from the abstract class PndTpcAbsPadShape
//      This class defines the shape of a pad by a vector of ordered points.
//      The user should to take care that the origin is almost circa in the
//      centre of the polygon (origins outside the polygon are forbidden).
//
// Environment:
//      Software developed for the PANDA Detector at FAIR.
//
// Author List:
//      Cristoforo Simonetto    TUM            (original author)
//
//
//-----------------------------------------------------------

#ifndef TPCPADSHAPEPOLYGON_HH
#define TPCPADSHAPEPOLYGON_HH

// Base Class Headers ----------------
#include "PndTpcAbsPadShape.h"

// Collaborating Class Headers -------
#include <vector>

// Collaborating Class Declarations --


class PndTpcPadShapePolygon: public PndTpcAbsPadShape {
public:

  // Constructors/Destructors ---------
  PndTpcPadShapePolygon(const std::vector<double>& x,
		     const std::vector<double>& y,
		     const unsigned int _ID);
  virtual ~PndTpcPadShapePolygon();

  // Operators
  friend std::ostream& operator<< (std::ostream& s,
				  const PndTpcPadShapePolygon& me);

  // Accessors -----------------------
  
  // Modifiers -----------------------
  // Once a PndTpcPadShapePolygon is constructed it should not be modified.

  // Operations
  virtual void EvalBoundingRect(double& width, double& height,
				const double angle) const;
  virtual bool Contains(const double xp, const double yp) const;
  virtual int GetNBoundaryPoints() const {return(xCoord.size()-1);};
  virtual void GetBoundaryPoint(const int index,
				double& x, double& y) const;

private:

  // Private Data Members ------------
  std::vector<double> xCoord;              //Coordinates of the polygon in the
  std::vector<double> yCoord;              //right order. The last is the first
  
  // Private Methods -----------------
  
};

#endif

//--------------------------------------------------------------
// $Log$
//--------------------------------------------------------------
