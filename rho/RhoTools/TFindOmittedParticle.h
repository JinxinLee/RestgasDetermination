//--------------------------------------------------------------------------
// File and Version Information:
// 	$Id: TFindOmittedParticle.h,v 1.1.1.1 2001-05-04 16:20:06 marcel Exp $
//
// Description:
//	Class TFindOmittedParticle.
//	Given resonance (Y(4S)) momentum, B0 momentum, and
//	K0L mass at creation, this object then finds the best
//	candidate for the K0L 4-momentum given the 4-momentum
//	of the J/psi and the direction of the neutral cluster.
//
//	Because this is a general utility class, B0 is labeled Child,
//	Y(4S) is labeled Reson (for Resonance), J/psi is labeled Obs 
//	(for Observed), and K0L is labeled Sought (for the calculated
//	values) or Seen (for the actual IFR/EMC cluster).
//
//	Constructor takes CM 4-momentum, Child mass, and Sought mass.
//
//	fitToSeen() calls makeCone(), then closestFit().
//
//	makeCone() finds the possible Sought 4-momenta given the Observed
//	momentum.
//
//	closestFit() finds the best fit between the cone of possibilities
//	and the Seen neutral cluster.
//
//	secondVector() returns the second momentum found by closestFit()
//	but rejected as an inferior fit, for diagnostic purposes.
//
//	If there is demand, a function may be added to extract the
//	parameters of the cone describing possible Sought momenta;
//	makeCone() and closestFit() would then be made public;
//	or perhaps a subclass could implement this functionality.
//
//	Note: this class by default currently only works in 
//	coordinate systems with the boost along the z-axis.
//	Setting zBoostApprox to false overrides this.
//	Note: this class currently assumes (as is the case for B0->J/psi KL)
//	that in the resonance CM (Y(4S)) frame, the 3-momentum of the 
//	observed (J/psi) particle is much larger than the child (B0) 
//	3-momentum, insuring that the CM angle between the observed and
//	sought (KL) is greater than pi/2.  (In the language of this class,
//	openCos>0, since we use the opposite of the observed momentum).
//
// Environment:
//	Software developed for the BaBar Detector at the SLAC B-Factory.
//
// Author List:
//      Adam Breon                      Original Author
//
// Copyright Information:
//	Copyright (C) 1997-1999		Lawrence Berkeley Laboratory
//
// ROOT Version by Marcel Kunze, RUB
//------------------------------------------------------------------------
/**/
#ifndef TFINDOMITTEDPARTICLE_H
#define TFINDOMITTEDPARTICLE_H

#include "TVector3.h"
#include "TLorentzVector.h"

class TFindOmittedParticle : public TObject {

//--------------------
// Instance Members --
//--------------------

public:

  // Constructors
  TFindOmittedParticle(
	     const TLorentzVector & ip4Reson, // set Y(4S) p4, usually.
	     const double imChild,		// B0 mass.
	     const double imSought,		// K mass (p4 not known).
	     const Bool_t izBoostApprox=kTRUE);	// Approximate boost along z?

  virtual ~TFindOmittedParticle( );

  TLorentzVector FitToSeen( const TLorentzVector & p4Obs, 
			      const TVector3 & p3Seen );

  // Returns the second-choice fit vector.
  TLorentzVector SecondVector( ) const;

protected:

  void MakeCone( const TLorentzVector & p4Obs );

  TLorentzVector ClosestFit( const TVector3 & p3Seen );

  //TLorentzVector p4Reson;		// 4-momentum of resonance
  //float mChild;			// mass of Child
  float mSought2;			//! squared mass of Sought

  Bool_t zBoostApprox;			//! Flag: do we approximate boost in
					// the z direction, or use real boost?
					// (Default = kTRUE).

  float cmEChild, cmpChild2;		//! energy, squared 3-momentum of Child.

  //Caches value of p4Obs to avoid recalculating the code unnecessarily.
  TLorentzVector p4ObsCache; //!

  //Stores value of worse fit so we can look at it.
  TLorentzVector secondChoice;	//! The _other_ vector, stored.

  // BOOST vectors.
  TVector3	beta;			//! From lab to CM (-beta CM to lab)

  // The cone parameters.
  TVector3	cmAxis;			//! Axis of CM cone
  double       	openCos;		//! cos of opening angle.
  double	cmESought;		//! CM energy of sought particle.
  double	cmpSought2;		//! square of CM 3-momentum of sought.

public:
    ClassDef(TFindOmittedParticle,1) // Finds the best candidate for the K0L 4-momentum
};
#endif
