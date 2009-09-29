//-----------------------------------------------------------
// File and Version Information:
// $Id$
//
// Description:
//      TPC digitization: avalanche 
//      Models a cloud of charge produced by a gas amplification
//      process in the TPC-GEMs
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

#ifndef TPCAVALANCHE_HH
#define TPCAVALANCHE_HH

// Base Class Headers ----------------
#include "TObject.h"

// Collaborating Class Headers -------
#include <ostream>

// Collaborating Class Declarations --
class PndTpcDriftedElectron;

class PndTpcAvalanche : public TObject {
public:

  // Constructors/Destructors ---------
  PndTpcAvalanche();
  PndTpcAvalanche(const double x, 
	       const double y,
	       const double t,
	       const double Amp,
	       PndTpcDriftedElectron* tpcDElectronp);

  ~PndTpcAvalanche(){;}

  // Operators
  friend bool operator== (const PndTpcAvalanche&, const PndTpcAvalanche&);
  friend bool operator<  (const PndTpcAvalanche& lhs,const PndTpcAvalanche& rhs)
  {return lhs.ft < rhs.ft;}

  friend std::ostream& operator<< (std::ostream& s, const PndTpcAvalanche& me);


  // Accessors -----------------------
  double x() const {return fx;}
  double y() const {return fy;}
  double t() const {return ft;}
  double amp() const {return famp;}
  unsigned int mcTrackId() const; 

  unsigned int mcHitId() const; 

  PndTpcDriftedElectron* mother() const {return fmother;}
  
  // Modifiers -----------------------
  void addAmp(double a){famp+=a;}

private:

  // Private Data Members ------------
  double fx;
  double fy;
  double ft;
  double famp;
  PndTpcDriftedElectron* fmother;
  
  // Private Methods -----------------

public:
  ClassDef(PndTpcAvalanche,1)

};

#endif

//--------------------------------------------------------------
// $Log$
//--------------------------------------------------------------
