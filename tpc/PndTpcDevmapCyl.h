//-----------------------------------------------------------
//
// Description:
//      Implementation of PndTpcAbsField for a Map (cylindrical
//      symmetry) of drift deviations relative to a straight
//      line.
//      The elements are TVector3's of the format (x,y,z).
//      This is ok since we assume rotational symmetry and 
//      therefore the value of time will never be altered.
//
// Environment:
//      Software developed for the PANDA Detector at FAIR.
//
// Author List:
//      Cristoforo Simonetto    TUM         (original author)
//      Felix Boehmer           TUM
//
//-----------------------------------------------------------


#ifndef TPCDEVMAPCYL_H
#define TPCDEVMAPCYL_H

// Base Class Headers ----------------
#include "PndTpcFieldCylGrid.h"

// Collaborating Class Headers -------
#include "PndTpcDevmapCylLoader.h"

// Collaborating Class Declarations --



class PndTpcDevmapCyl : public PndTpcFieldCylGrid<TVector3> {

friend class PndTpcDevmapCylLoader;


public:

  // Constructors/Destructors ---------
  PndTpcDevmapCyl(const char* const, double); 
  virtual ~PndTpcDevmapCyl();

  // Operators
  virtual void print(std::ostream&) const;

  // Accessors -----------------------
  virtual TVector3 value(const TVector3&) const;
  const double vDrift(){return _vDrift;}
  bool loaded() const {return _loaded;}

  // Modifiers -----------------------

private:
  // Private Data Members ------------
  
  PndTpcDevmapCylLoader* loader;  //this class takes care of the file I/O
  double _vDrift;
  bool _loaded;

};

#endif
