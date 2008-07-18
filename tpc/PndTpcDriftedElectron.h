//-----------------------------------------------------------
// File and Version Information:
// $Id$
//
// Description:
//      Data class to carry information of drifted electrons
//      in PndTpc digitization. Defined at first GEM.
//
// Environment:
//      Software developed for the PANDA Detector at FAIR.
//
// Author List:
//      Sebastian Neubert    TUM            (original author)
//      Cristoforo Simonetto TUM  
//
//-----------------------------------------------------------

#ifndef TPCDRIFTEDELECTRON_HH
#define TPCDRIFTEDELECTRON_HH

// Base Class Headers ----------------
#include "TObject.h"

// Collaborating Class Headers -------


// Collaborating Class Declarations --

class PndTpcPrimaryCluster;

class PndTpcDriftedElectron : public TObject {
public:

  // Constructors/Destructors ---------
  PndTpcDriftedElectron();
  PndTpcDriftedElectron(const double x, 
		     const double y,
		     const double t,
		     PndTpcPrimaryCluster* tpcPClusterp);
  ~PndTpcDriftedElectron(){;}

  // Operators
  friend bool operator== (const PndTpcDriftedElectron& lhs,
			  const PndTpcDriftedElectron& rhs);

  friend bool operator<  (const PndTpcDriftedElectron& lhs,
			  const PndTpcDriftedElectron& rhs){return lhs._t < rhs._t;}

  // Accessors -----------------------
  double x() const {return _x;}
  double y() const {return _y;}
  double t() const {return _t;}
  unsigned int mcTrackId() const;
  unsigned int mcHitId() const;

  PndTpcPrimaryCluster* tpcPClusterp() const {return _tpcPClusterp;}
  
  void addtox(double dx) {_x+=dx;}
  void addtoy(double dy) {_y+=dy;}
  void addtot(double dt) {_t+=dt;}

private:

  // Private Data Members ------------
  double _x;
  double _y;
  double _t;
  PndTpcPrimaryCluster* _tpcPClusterp; //->
    
  // Private Methods -----------------

public:
  ClassDef(PndTpcDriftedElectron,1)

};

#endif

//--------------------------------------------------------------
// $Log$
