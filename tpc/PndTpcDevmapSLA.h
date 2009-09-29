//-----------------------------------------------------------
//
// Description:
//
//      Straight Line representation of a Deviation Map
//      needs parameter management
//      EXPERIMENTAL
//
//
// Environment:
//      Software developed for the PANDA Detector at FAIR.
//
// Author List:
//      Felix Boehmer           TUM       (original author)
//
//-----------------------------------------------------------


#ifndef TPCDEVMAPSLA_H
#define TPCDEVMAPSLA_H

// Base Class Headers ----------------
#include "PndTpcFieldCylGrid.h"
#include "PndTpcDevmapCyl.h"




class PndTpcDevmapSLA : public PndTpcDevmapCyl {

  
 public:

  // Constructors/Destructors ---------
  PndTpcDevmapSLA(const char* const, double); 
  virtual ~PndTpcDevmapSLA() {;}

  // Operators
  virtual void print(std::ostream&) const {;}

  // Accessors -----------------------
  virtual TVector3 value(const TVector3&) const;
  
  // Modifiers -----------------------

 private:
  
                                      //slopes for straight lines for
  std::vector<double> fslopesR;       //dev. in R
  std::vector<double> fslopesPerpR;   //dev. perp to R

};

#endif
