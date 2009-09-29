//-----------------------------------------------------------
// File and Version Information:
// $Id: PndTpcPrimaryCluster.hh,v 1.8 2006/08/17 20:03:05 sneubert Exp $
//
// Description:
//      Data class for the Digitization. (Transient)
//      Represents primary clusters of electrons which are created
//      by the passage of a charged particle through the TPC gas-volume
//
//      Created by: PndTpcClusterizerTask
//      Used by:    PndTpcDrifterTask
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

#ifndef TPCPRIMARYCLUSTER_HH
#define TPCPRIMARYCLUSTER_HH

// Base Class Headers ----------------
#include "TObject.h"

// Collaborating Class Headers -------
#include "TVector3.h"

// Collaborating Class Declarations --


class PndTpcPrimaryCluster : public TObject {
public:

  // Constructors/Destructors ---------
  PndTpcPrimaryCluster();

  PndTpcPrimaryCluster(const double t, 
		    const int q,
		    const TVector3& pos,
		    const unsigned int mcTrackId,
		    const unsigned int mcHitId); // for creation without G4

  ~PndTpcPrimaryCluster();

  // Operators
  friend bool operator<  (const PndTpcPrimaryCluster& lhs,
			  const PndTpcPrimaryCluster& rhs)
  {return lhs.ft<rhs.ft;}

  // Accessors -----------------------
  double t() const {return ft;}
  int q() const {return fq;}
  const TVector3& pos() const {return fpos;}
  double pos(int i) const {return fpos(i);}
  double x() const {return fpos(0);}
  double y() const {return fpos(1);}
  double z() const {return fpos(2);}
  unsigned int mcTrackId() const;
  unsigned int mcHitId() const;

  // Modifiers -----------------------
  void sett(const double t){ft=t;}
  void setq(const int q){fq=q;}
  void setpos(const double x, const double y, const double z)
  {fpos(0)=x;fpos(1)=y;fpos(2)=z;}
  

private:

  // Private Data Members ------------
  double ft;               // time of creation
  int fq;                  // charge
  unsigned int fmcTrackId;      // internal mc id if GEANT is not used
  unsigned int fmcHitId;
  TVector3 fpos;         // position

  // Private Methods -----------------

public: 
  ClassDef(PndTpcPrimaryCluster,1)

};

#endif

//--------------------------------------------------------------
// $Log: PndTpcPrimaryCluster.hh,v $
// Revision 1.8  2006/08/17 20:03:05  sneubert
// enhanced interface
//
// Revision 1.7  2006/02/24 13:38:00  sneubert
// Code revision done
//
// Revision 1.6  2006/02/23 11:45:17  sneubert
// CodeRevision: revised PndTpcGHit
//
// Revision 1.5  2005/08/18 09:34:48  sneubert
// added chain of responsibility for MCIDs
//
// Revision 1.4  2005/08/05 13:47:50  csimonet
// replaced the IDs by a pointer
//
// Revision 1.3  2005/07/27 11:31:04  sneubert
// reworked operators
//
// Revision 1.2  2005/07/19 16:40:11  sneubert
// fixed bug in SetPos
//
// Revision 1.1  2005/07/18 10:06:45  sneubert
// initial import
//
//--------------------------------------------------------------
