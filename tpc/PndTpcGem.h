//-----------------------------------------------------------
// File and Version Information:
// $Id$
//
// Description:
//      Data class carrying information about GEMs  
//      in the TPC
//
// Environment:
//      Software developed for the PANDA Detector at FAIR.
//
// Author List:
//      Sebastian Neubert    TUM            (original author)
//
//
//-----------------------------------------------------------

#ifndef TPCGEM_HH
#define TPCGEM_HH

// Base Class Headers ----------------


// Collaborating Class Headers -------
#include <ostream>
//#include "CLHEP/GenericFunctions/AbsFunction.hh"
//using Genfun::AbsFunction;
#include "TF1.h"

// Collaborating Class Declarations --

class PndTpcGem {
public:

  // Constructors/Destructors ---------
  PndTpcGem();
  PndTpcGem(const double Gain, const double Spread);
  ~PndTpcGem();

  // Operators
  friend bool operator== (const PndTpcGem&,const PndTpcGem&);
  friend std::ostream& operator<<(std::ostream&, const PndTpcGem&);

  // Accessors -----------------------
  double gain() const {return fgain;}
  double spread() const {return fspread;}

  const TF1* cloudShape() const {return fcloudShape;}

  void PrintAll(std::ostream& s) const {s<<*this;}

  // Modifiers -----------------------
  void setgain(const double G){fgain=G;}
  void setspread(const double S){fspread=S;}

private:

  // Private Data Members ------------
  double fgain;
  double fspread;
  
  TF1* fcloudShape;
  
  // Private Methods -----------------
  bool operator<  (const PndTpcGem&) const; // forbidden, PndTpcGem
                                         // is not an ordered type
};




#endif

//--------------------------------------------------------------
// $Log: PndTpcGem.hh,v $
// Revision 1.2  2006/03/10 14:45:24  sneubert
// update naming scheme
//
// Revision 1.1  2005/08/03 13:42:20  sneubert
// added PndTpcGem PndTpcPad and Lookuptable
//
//
//
//--------------------------------------------------------------
