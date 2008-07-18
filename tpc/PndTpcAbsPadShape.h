//-----------------------------------------------------------
// File and Version Information:
// $Id$
//
// Description:
//      The abstract basis class for the shapes used by the pads.
//      Each shape owns a LookupTable.
//
// Environment:
//      Software developed for the PANDA Detector at FAIR.
//
// Author List:
//      Cristoforo Simonetto    TUM            (original author)
//
//
//-----------------------------------------------------------

#ifndef TPCABSPADSHAPE_HH
#define TPCABSPADSHAPE_HH

// Base Class Headers ----------------


// Collaborating Class Headers -------
#include <ostream>

// Collaborating Class Declarations --
class PndTpcPRLookupTable;

class PndTpcAbsPadShape {
public:

  // Constructors/Destructors ---------
  PndTpcAbsPadShape(const unsigned int _ID);
  virtual ~PndTpcAbsPadShape();

  // Operators
  friend bool operator== (const PndTpcAbsPadShape& lhs,
			  const PndTpcAbsPadShape& rhs);
  friend std::ostream& operator<< (std::ostream& s, const PndTpcAbsPadShape& me);

  // Accessors -----------------------
  PndTpcPRLookupTable* GetLookupTable() const {return(lookupTable);}
  unsigned int GetID() const {return(ID);}
  
  //returns the width and height of an covering rectangle parallel to the axis,
  //if the sahpe is rotated by angle (in math. direction of rotation)
  virtual void EvalBoundingRect(double& width, double& height, 
				const double angle) const =0;
  //returns if a point (x,y) in the coordinate system of the shape
  //is boundary point
  virtual bool Contains(const double x, const double y) const =0;
  
  //returns the number of points, you can get with GetBoundaryPoint
  virtual int GetNBoundaryPoints() const =0;

  //returns those points on the boundary, which describe the form
  //e.g. a polygon returns the edges,
  //e.g a circle could return points with small distances from each other
  virtual void GetBoundaryPoint(const int index,
				double& x, double& y) const =0;

  //returns the integrated value of the signal caused by an electron
  //avalanche at (x,y) (in the coordinate system of the shape)
  double GetValue(const double x, const double y) const;

  // Modifiers -----------------------
  // The LookupTable should not be modified

  // Draw Pad with this shape at point (x,y) rotated by angle alpha
  virtual void Draw(double x, double y, double alpha, int color) const;

protected:

  // Private Data Members ------------
  PndTpcPRLookupTable* lookupTable;
  unsigned int ID;
  // Private Methods -----------------

};

#endif

//--------------------------------------------------------------
// $Log$
//--------------------------------------------------------------
