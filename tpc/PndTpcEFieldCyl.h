//-----------------------------------------------------------
// File and Version Information:
// 
//
// Description:
//      This class describes a cylinder symmetric electric field
//      read from file
//
// Environment:
//      Software developed for the PANDA Detector at FAIR.
//
// Author List:
//      Cristoforo Simonetto    TUM            (original author)
//      Felix Boehmer           TUM
//
//
//-----------------------------------------------------------

#ifndef TPCEFIELDCYL_H
#define TPCEFIELDCYL_H

// Base Class Headers ----------------
#include "PndTpcFieldCylGrid.h"

// Collaborating Class Headers -------
#include "PndTpcEFieldCylLoader.h"


class PndTpcEFieldCyl : public PndTpcFieldCylGrid<TVector3> {

friend class PndTpcEFieldCylLoader;


public:

  // Constructors/Destructors ---------
  PndTpcEFieldCyl(const char* const);
  virtual ~PndTpcEFieldCyl();

  // Operators
  virtual void print(std::ostream&) const;

  // Accessors -----------------------
  virtual TVector3 value(const TVector3&) const;
  
  // Modifiers -----------------------


private:

  // Private Data Members ------------

  PndTpcEFieldCylLoader* loader;  //this class takes care of the file I/O


};

#endif



//--------------------------------------------------------------
// $Log: PndTpcEFieldCyl.hh,v $
//
// 2007/05/15 fboehmer
// ported to work with ROOT. 
// data is now read in by a friend class
//
// Revision 1.3  2006/09/06 09:55:22  csimonet
// changed behaviour outside volume
//
// Revision 1.2  2006/08/31 10:08:08  csimonet
// inherits now from FieldBase
//
// Revision 1.1  2006/01/27 09:45:34  csimonet
// rotationally symmetric electric field, read in from file
//
//--------------------------------------------------------------
