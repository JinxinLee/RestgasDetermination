//--------------------------------------------------------------------------
// File and Version Information:
// 	$Id: TThrust.cc,v 1.2 1997/1/20 10:05:13 
//
// Description:
//
//	Class TThrust - a simple object to calculate thrust 
//
//	Thrust is a quantity that falls in the range (0.5,1.0).  It
//	is a measure of the alignment of the particles within an event
//	along a common axis.  The lower the thrust, the more spherical
//	the event is.  The higher the thrust, the more jet-like the
//	event is.
//
//	The thrust axis is the vector along which the maximum alignment
//	is found according to the following formula:
//
//
//	             _                               _
//	             |    sum_i=1^N(abs(A . P_i))    | 
//	thrust = max | ----------------------------- |
//	             | sum_i=1^N( sqrt(P_i . _P_i) ) |
//                   -                               -
//
//	where "A . B" is the dot product of three vectors A and B,
//	abs(X) is the absolute value of X,
//	N is the number of particles used in the calculation,
//	A is the thrust axis with unit length,
//	and P_i is the three momentum of particle i.
//
//	The thrust axis has a two-fold ambiguity due to taking the
//	absolute value of the dot product.  Both A and -A give the
//	same value for the thrust.  There is no inherent physical
//	meaning to the sign of the vector.  The two-fold ambiguity
//	is collapsed by this class such that the coefficient along 
//	the z-direction is positive.  A particular analysis may
//	find that there is a convenient definition of the sign of
//	A and may make the appropriate adjustments after this
//	calculation.
// 
//	There are three modes:
//
//		BTAllParticles (0) (default) - use both charged and neutrals
//		BTChargedOnly (1) - use only charged particles
//		BTNeutralOnly (2) - use only neutral particles
//
// Environment:
//      Software developed for the BaBar Detector at the SLAC B-Factory.
//
// Author List:
//	Scott Metzler		original author
//
// ROOT Version by Marcel Kunze, RUB
//------------------------------------------------------------------------

#ifndef TTHRUST_H
#define TTHRUST_H

//------------------------------------
// Collaborating Class Declarations --
//------------------------------------
class TEventInfo;
class TCandidate;
class TSelector;
#include "RhoBase/TCandList.h"
#include "TVector3.h"
#include "RhoTools/TThrustVector.h"

//		---------------------
// 		-- Class Interface --
//		---------------------
 
class TThrust  : public TObject {

  //--------------------
  // Instance Members --
  //--------------------

public:

  // enums
  enum TParticles { BTAllParticles, BTChargedOnly, BTNeutralOnly };

  // Constructors

  /**
   *  Creates a TThrust object.  The calculation of the thrust is
   *  done at construction time.  All the particles are boosted into
   *  the center-of-mass rest frame for the calculation.  A sub-set
   *  of the particles can be used for the calculation by using the
   *  mode parameter.
   *
   *  @param    partList        list of particles for calculation.
   *                            a sub-set may be used by selecting mode.
   *  @param    evtInfo         event info, which contains the center-of-mass.
   *  @param    mode            the mode for selecting a sub-set of particles.
   */
  TThrust(TCandList& partList, 
	    const TEventInfo& evtInfo,
	    TParticles mode = BTAllParticles, 
	    Bool_t cutInCms = kFALSE);

  // Destructor
  virtual ~TThrust();

  // accessors to get at results

  double Thrust() const { return _theThrustVector.Thrust(); }
  // return the thrust

  TVector3 ThrustAxis() const { return _theThrustVector.ThrustAxis(); }
  // return the thrust axis

private:

  // the thrust vector
  TThrustVector _theThrustVector; //!

  Bool_t _charged, _checkCharge, _cutInCms; //!

  // friends
  friend class testTThrust; // declare

  void init( TCandList&, 
	     const TEventInfo&);

public:
    ClassDef(TThrust,1) // A simple object to calculate thrust
};

#endif
