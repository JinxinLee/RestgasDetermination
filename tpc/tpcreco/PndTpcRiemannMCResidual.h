//-----------------------------------------------------------
// File and Version Information:
// $Id$
//
// Description:
//      Conformal Mapping of a PndTpc Cluster (x,y)->(r,phi)-> riemann sphere
//
//
// Environment:
//      Software developed for the PANDA Detector at FAIR.
//
// Author List:
//      Sebastian Neubert    TUM            (original author)
//
//
//-----------------------------------------------------------

#ifndef TPCRIEMANNMCRESIDUAL_HH
#define TPCRIEMANNMCRESIDUAL_HH

// Base Class Headers ----------------
#include "TObject.h"

// Collaborating Class Headers -------
#include <ostream> // remove if you do not need streaming op
#include "McIdCollection.h"
#include "TRef.h"
#include "PndTpcRiemannHit.h"
#include "PndTpcRiemannTrack.h"
// Collaborating Class Declarations --
class PndTpcCluster;


class PndTpcRiemannMCResidual : public TObject{
public:

  // Constructors/Destructors ---------
  PndTpcRiemannMCResidual():_hit(NULL),_track(NULL),_riemd(999),_szd(999){}

  PndTpcRiemannMCResidual(PndTpcRiemannHit* hit,
			  PndTpcRiemannTrack* track,
			  const McIdCollection& mcidtrack)
    : _hit(hit),_track(track),_riemd(999),_szd(999),_mcIdTrack(mcidtrack){}
  ~PndTpcRiemannMCResidual(){}

  
  // Accessors -----------------------
  PndTpcRiemannHit* hit(){return (PndTpcRiemannHit*)_hit.GetObject();}
  PndTpcRiemannTrack* track(){return (PndTpcRiemannTrack*)_track.GetObject();}

   Double_t riemd(){return _riemd;} // distance to riemann plane
  Double_t szd(){return _szd;}   // residual of sz fit
  Double_t dclose(){return _dclose;} // distance to closest hit

  // Modifiers -----------------------
  void SetRiemD(Double_t d){_riemd=d;}
  void SetSZD(Double_t d){_szd=d;}
  void SetCloseD(Double_t d){_dclose=d;}

  // Operations ----------------------
  Bool_t isTruth();

private:

  // Private Data Members ------------
  TRef _hit; // riemann hit in TClonesArray (if available)
  TRef _track; //
  Double_t _riemd; // distance to riemann plane
  Double_t _szd;   // residual of sz fit
  Double_t _dclose; // distance to closest hit
  McIdCollection _mcIdTrack; // mc id collection of track

  // Private Methods -----------------

public:
  ClassDef(PndTpcRiemannMCResidual,1)

};

#endif

//--------------------------------------------------------------
// $Log$
//--------------------------------------------------------------
