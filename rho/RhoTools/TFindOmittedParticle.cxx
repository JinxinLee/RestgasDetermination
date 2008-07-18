//--------------------------------------------------------------------------
// File and Version Information:
// 	$Id: TFindOmittedParticle.cxx,v 1.3 2002-02-01 23:00:13 marcel Exp $
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
//
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

#include "RhoTools/TFindOmittedParticle.h"
#include <assert.h>
#include <math.h>
#include "TLorentzVector.h"
#include "TVector3.h"

ClassImp(TFindOmittedParticle)

TBuffer &operator>>(TBuffer &buf, TFindOmittedParticle *&obj)
{
   obj = (TFindOmittedParticle *) buf.ReadObject(TFindOmittedParticle::Class());
   return buf;
}

#include <iostream>
using namespace std;

//----------------
// Constructors --
//----------------

/*
 * Constructor; sets mass of the child of the resonance and the 
 *   particle to be found.
 * For now, uses ErrMsg calls to ensure arguments
 *   make sense.
 */
TFindOmittedParticle::TFindOmittedParticle(
		   const TLorentzVector & ip4Reson,	// set Y(4S) p4.
		   const double imChild,		// B0 mass.
		   const double imSought,		// K mass
		   const Bool_t izBoostApprox)		// boost in z approx?
  //: p4Reson(ip4Reson)
  //: mChild(fabs(imChild))
  : mSought2(imSought*imSought)
  , p4ObsCache(0.0,0.0,0.0,-1.0)		// Non-physical initial value
  , zBoostApprox(izBoostApprox)
{
  if (imChild == 0.0)
    cerr << "TFindOmittedParticle constructed with mChild=0" << endl;
  
  if(mSought2 == 0.0)
    cerr << "TFindOmittedParticle constructed with mSought=0" << endl;  

  // Figure out the boost vectors to and from the center of mass

  //p4Reson = ip4Reson;
  beta = -ip4Reson.BoostVector();
      
  // Figure out the decay energies, gamma factors, beta factors.
  // Assume that resonance decays to two "child" particles (e.g. B + Bbar)

  cmEChild = ip4Reson.Mag() / 2;

  if (cmEChild < fabs(imChild))
    cerr << "TFindOmittedParticle: mChild too large for decay" << endl;

  // The square of the three-momentum of the child (B0).
  cmpChild2 = cmEChild*cmEChild - imChild*imChild;

}

//--------------
// Destructor --
//--------------

// The destructor should be limited to undoing the work of the constructor
TFindOmittedParticle::~TFindOmittedParticle( )
{
}


//--------------------
// Member Functions --
//--------------------

/*
 * Given the 4-momentum of the observed decay products, and assuming
 *  that p3Seen indicates a neutral cluster from the omitted particle,
 *  returns the most likely lab-frame 4-momentum with the sought mass
 *  specified in the constructor.  Returns the zero vector if no such
 *  consistent vector can be found.
 */

TLorentzVector
TFindOmittedParticle::FitToSeen( const TLorentzVector & p4Obs, 
				const TVector3 & p3Seen )
{
  if (p4Obs != p4ObsCache)
    {
      this->MakeCone(p4Obs);
      p4ObsCache = p4Obs;
    }
  return this->ClosestFit(p3Seen);
}


/*
 * Given the already known resonance p4, figures out the cone
 * of possible values for the sought particle in the center of
 * mass.
 */

void 
TFindOmittedParticle::MakeCone( const TLorentzVector  & p4Obs )
{
  // Get the observed mass, figure out the decay momentum of the
  // child -> sought + observed.

  // Square of mass of observed particle.
  const double mObs2 = p4Obs.Mag2();

  /* This was an old, labor-intensive way of calculating the 
     decay momentum.  Using conservation of energy is much simpler to
     understand and faster to compute.
  // The following is a solution of sqrt(pDecay*pDecay + mSought*mSought)
  // + sqrt(pDecay*pDecay + mObs*mObs) == mChild
  double pDecay;
  {
  double temp;
  
  temp = (mObs2 - mSought*mSought) / mChild;
  temp *= temp;
  temp += mChild*mChild - 2 * (mObs2 + mSought*mSought);
  
  pDecay = 0.5 * sqrt(temp);
  }
  
  //const double EObsDecay = sqrt(mObs2 + pDecay*pDecay);
  //const double ESoughtDecay = sqrt(mSought*mSought + pDecay*pDecay);
  */


  // Boost vectors into the CM frame; extract vital statistics.

  TLorentzVector cmp4Obs(p4Obs); cmp4Obs.Boost(beta);

  //const double cmEObs = cmp4Obs.t();
  const double cmpObs2 = TVector3(cmp4Obs.X(),cmp4Obs.Y(),cmp4Obs.Z()).Mag2();

  cmESought = cmEChild - cmp4Obs.T();
  double cmESought2 = cmESought*cmESought;
  if ( cmESought2 > mSought2)
      cmpSought2 = cmESought2 - mSought2;
  else
      cmpSought2 = 0.0;



  /* 
   * Because we know the lengths of the 3 momentum 3-vectors, we can
   * get the angles using the law of cosines.
   * abs(openCos) > 1 signifies an error, causing closestFit to return
   * a zero vector, signifying an error.
   */

  const double cmpSought = sqrt(cmpSought2);

  if (cmpSought != 0)
    openCos =
      ( cmpObs2 + cmpSought2 - cmpChild2 ) /
      ( 2 * sqrt(cmpObs2) * cmpSought );
  else
    openCos = 2;	// Error condition.

  cmAxis = -TVector3(cmp4Obs.X(),cmp4Obs.Y(),cmp4Obs.Z()).Unit() * openCos * cmpSought;

}


/*
 *  Given the cone of possible values for the sought momentum in the
 *  center of mass, takes that p3Seen points to a neutral cluster
 *  due to the sought particle in the lab frame.  Assuming that the
 *  boost is solely along the z-axis, finds the intersection of the 
 *  cone in the center of mass with the plane defined by the azimuthal
 *  angle of p3Seen.  If there is no intersection, returns 0.  Otherwise,
 *  boosts both intersections back to the lab frame.  It returns the one
 *  at the smallest angular separation from p3Seen.
 */

TLorentzVector 
TFindOmittedParticle::ClosestFit( const TVector3 & p3Seen )
{
  // Take care of these now to save clock cycles.
  if (fabs(openCos) > 1.0) {
    return TLorentzVector(0.0,0.0,0.0,0.0);  // Error condition.
  }

  TLorentzVector cmp4Sought[2];  // Two intersection points...

  // Set up local coordinate system:
  //  z = normal z.
  //  x-z plane contains p3Seen.  Since we only keep phi information,
  //  we needn't bother boosting.
  // I'm doing more stuff by hand than in previous versions in order
  // to speed things up.
  double cmCosPlaneAxis2;		// Squared cosine between the plane and axis.
  {
    // Original, most abstract method:
    //const TVector3 cmZ(0.0, 0.0, 1.0);
    //const TVector3 cmX = (p3Seen - p3Seen.dot(cmZ) * cmZ).unit();
    //const TVector3 cmY = cmZ.cross(cmX);

    // Slightly faster method:
    //const TVector3 cmX = TVector3(p3Seen.x(), p3Seen.y(), 0.0).unit();
    //const TVector3 cmY = TVector3(-cmX.y(), cmX.x(), 0.0);

    TVector3 cmY;
    // Fastest method:
    if (zBoostApprox)
	cmY = TVector3(-p3Seen.Y(), p3Seen.X(), 0.0).Unit();
    else
	cmY = beta.Cross(p3Seen).Unit();

    const TVector3 unitAxis = cmAxis.Unit();

    // Projection of unit vector of axis into x-z plane.

    // Project unitAxis into the xz plane.
    const TVector3 unitProj = unitAxis - unitAxis.Dot(cmY) * cmY;
    // While we know cmY.z() == 0, doing it by hand would make the
    // code too baroque for not a great speed advantage.

    // Square of cosine of angle between plane and axis:
    cmCosPlaneAxis2 = unitProj.Mag2();


    if (cmCosPlaneAxis2 < openCos*openCos) {
      return TLorentzVector(0.0,0.0,0.0,0.0);  // Error condition.
    }

    // Projection of full axis into x-z plane.
    // Actually, it's longer; touches the base of the cone.
    // cmAxis is projection of unitProj, hence 1/cosine.

    const TVector3 axisProj = 
      (cmAxis.Mag() / (cmCosPlaneAxis2) ) * unitProj;

    // vector difference between axisProj and the true
    // vector, which could lie to either side.
    
    //const TVector3  difference = cmAxis.mag() * 
    //sqrt(1/(openCos*openCos) - 1/(cmCosPlaneAxis2)) *
    //unitProj.cross(cmY).unit();

    const TVector3 difference =
      sqrt(cmpSought2 - axisProj.Mag2()) *
      unitProj.Cross(cmY).Unit();

    cmp4Sought[0] = TLorentzVector(axisProj + difference, cmESought);
    cmp4Sought[1] = TLorentzVector(axisProj - difference, cmESought);
  }


  // Boost the results back into the lab frame.
  // Return whichever one is closer in angle to the seen vector.

  cmp4Sought[0].Boost(-beta);
  cmp4Sought[1].Boost(-beta);

  if ( TVector3(cmp4Sought[0].X(),cmp4Sought[0].Y(),cmp4Sought[0].Z()).Unit().Dot(p3Seen) > 
       TVector3(cmp4Sought[1].X(),cmp4Sought[1].Y(),cmp4Sought[1].Z()).Unit().Dot(p3Seen) )
    {
      secondChoice = cmp4Sought[1];
      return cmp4Sought[0];
    }
  else
    {
      secondChoice = cmp4Sought[0];
      return cmp4Sought[1];
    }
}


/*
 *  Returns the worse-fit vector found in closestFit() and saved
 *  in secondChoice.
 */

TLorentzVector 
TFindOmittedParticle::SecondVector() const
{
  return secondChoice;
}
