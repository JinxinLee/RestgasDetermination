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
#include "FairMultiLinkedData.h"

// Collaborating Class Headers -------


// Collaborating Class Declarations --

class PndTpcPrimaryCluster;

class PndTpcDriftedElectron : public FairMultiLinkedData {
public:

  // Constructors/Destructors ---------
  PndTpcDriftedElectron();
  PndTpcDriftedElectron(const double X, 
		     const double Y,
		     const double T,
		     PndTpcPrimaryCluster* tpcPClusterp);
  ~PndTpcDriftedElectron(){;}

  // Operators
  friend bool operator== (const PndTpcDriftedElectron& lhs,
			  const PndTpcDriftedElectron& rhs);

  friend bool operator<  (const PndTpcDriftedElectron& lhs,
			  const PndTpcDriftedElectron& rhs){return lhs.ft < rhs.ft;}

  // Accessors -----------------------
  double x() const {return fx;}
  double y() const {return fy;}
  double t() const {return ft;}
  unsigned int mcTrackId() const;
  unsigned int mcSecId() const;
  unsigned int mcHitId() const;

  PndTpcPrimaryCluster* tpcPClusterp() const {return ftpcPClusterp;}
  
  void addtox(double dx) {fx+=dx;}
  void addtoy(double dy) {fy+=dy;}
  void addtot(double dt) {ft+=dt;}

private:

  // Private Data Members ------------
  double fx;
  double fy;
  double ft;
  PndTpcPrimaryCluster* ftpcPClusterp; //->
    
  // Private Methods -----------------

public:
  ClassDef(PndTpcDriftedElectron,2)

};

#endif

//--------------------------------------------------------------
// $Log$
