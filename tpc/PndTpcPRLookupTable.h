//-----------------------------------------------------------
// File and Version Information:
// $Id$
//
// Description:
//      for PndTpc Digitization
//      Lookup table for the integration of the signal induction
//      on the pads
//
//
// Environment:
//      Software developed for the PANDA Detector at FAIR.
//
// Author List:
//      Sebastian Neubert    TUM            (original author)
//      Cristoforo Simonetto TUM
//
//
//-----------------------------------------------------------

#ifndef TPCPRLOOKUPTABLE_HH
#define TPCPRLOOKUPTABLE_HH

// Base Class Headers ----------------


// Collaborating Class Headers -------
#include <iostream> // remove if you do not need streaming op

// Collaborating Class Declarations --
class PndTpcGem;
class PndTpcAbsPadShape;


class PndTpcPRLookupTable {
public:

  // Constructors/Destructors ---------
  PndTpcPRLookupTable();
  ~PndTpcPRLookupTable();

  // Operators
  friend std::ostream& operator<< (std::ostream& s,
				   const PndTpcPRLookupTable& me);

  // Accessors -----------------------
  // (dx,dy) = vector from pad to avalanche!
  double GetValue(const double dx, const double dy) const;
  bool IsBuilt() const {return isbuilt;}
  int GetNStepsX() const {return 2*nx;}
  int GetNStepsY() const {return 2*ny;}
  double GetStep() const {return step;}

  // Modifiers -----------------------
  
  // Operations ----------------------
  void BuildTable(const PndTpcGem& a, 
		  const PndTpcAbsPadShape& pad, 
		  const double frange,      
		  const double fstep,
		  const double fintStep);

private:

  // Private Data Members ------------
  bool isbuilt;
  // the absolute range is the width/height of PndTpcPadPolygon +2*range
  // (in neg. and pos. direction => *2)
  double range;
  double step;   //step between two interpolation points
  int nx;        // number of steps in pos. x direction (total #=2*nx)
  int ny;        // number of steps in pos. y direction (total #=2*ny)
  double** table;

  // Private Methods -----------------
  double Integrate(const PndTpcGem& gem, 
		   const PndTpcAbsPadShape& p,
		   const double intStep,
		   const double dx,      
		   const double dy) const;

};

#endif

//--------------------------------------------------------------
// $Log$
//--------------------------------------------------------------
