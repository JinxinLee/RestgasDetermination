//--------------------------------------------------------------------------
// File and Version Information:
// 	$Id: TThrustVector.cc,v 1.2 1997/1/20 10:05:13 
//
// Description:
//      Class TThrustVector - used to calculate the thrust object 
//                              TThrust, without using TSelectors.
//
// Environment:
//      Software developed for the BaBar Detector at the SLAC B-Factory.
//
// Author List:
//      G. Hamel de Monchenault 
//      John Back   Moved to BetaCoreTools
//      
// ROOT Version by Marcel Kunze, RUB
//--------------------------------------------------------------------------

#include <math.h>
#include <assert.h>
#include "RhoTools/TThrustVector.h"
#include "RhoBase/TCandList.h"
#include "RhoBase/TCandListIterator.h"
#include "TLorentzVector.h"
#include "RhoTools/TBooster.h"
#include "RhoBase/TCandidate.h"

ClassImp(TThrustVector)

TBuffer &operator>>(TBuffer &buf, TThrustVector *&obj)
{
   obj = (TThrustVector *) buf.ReadObject(TThrustVector::Class());
   return buf;
}

// constants global to this file
const double epsilon = 0.0001;
const int nSegsTheta = 10; // number of initial segments in theta
const int nSegsPhi = 10; // number of initial segments in phi
const int nSegs = nSegsTheta * nSegsPhi; // total number of segments

TThrustVector::TThrustVector()
  : _thrust(0), _denom_sum(0), _booster(0), _cutInCms(kFALSE), _charged(kFALSE),
    _checkCharge(kFALSE)
{
}

void
TThrustVector::BoostAndCompute( TCandList& list,
				     TBooster* booster, 
				     Bool_t cutInCms, Bool_t checkCharge,
				     Bool_t charged) {
  _cutInCms = cutInCms;
  _checkCharge = checkCharge;
  _charged = charged;

  _booster = booster;

  assert( _booster != 0 );
  TCandList boostedList;
  _booster->BoostTo( list, boostedList );
  Compute( boostedList);
  boostedList.Cleanup();
}

void
TThrustVector::Compute(TCandList& list) {

  // initialize
  Reset();
  
  // variables
  TCandidate* trkptr;
  int i,j;
 
  TCandListIterator iter(list);
   
  const int maxNtracks = 100; // maximum number of tracks in event

  // copy momentum components to local variables for speed
  double valX[maxNtracks], valY[maxNtracks], valZ[maxNtracks];
  int nTracks = 0; // number of tracks, initialized to 0
  double denominator = 0; // sum of magnitudes of momentum
  // start a loop over the all tracks
  while ( (trkptr = iter.Next()) && (nTracks<maxNtracks) ) {

    // selection

    if (_checkCharge == kTRUE && acceptedTrack(trkptr) == kFALSE) continue;

    TLorentzVector cmList(trkptr->P4());
    TVector3 tmp(cmList.X(),cmList.Y(),cmList.Z());
    valX[nTracks] = tmp.X();
    valY[nTracks] = tmp.Y();
    valZ[nTracks] = tmp.Z();
    denominator += tmp.Mag();
    nTracks++;
  } // end while
  // forward information to other init
  init( valX, valY, valZ, denominator, nTracks );
}

Bool_t
TThrustVector::acceptedTrack(const TCandidate *cand) const {

  // Do what TChargedSelector effectively does...

  Bool_t answer = kTRUE;
  Bool_t cutInCms = kFALSE;
  double pMin = 0.0, pMax = 10.0, thMin = 0.0, thMax = TMath::Pi();
  
  double q = cand->Charge();
  Bool_t smallQ = fabs(q)<0.0001;

  if ((_charged == kTRUE  && smallQ == kTRUE) ||
      (_charged == kFALSE && smallQ == kFALSE)) return kFALSE;

  double p = cand->P();
  double theta = cand->P3().Theta();

  if (_cutInCms == kTRUE && _booster != 0) {

    TCandidate cCMS = _booster->BoostTo(*cand);
    p     = cCMS.P();
    theta = cCMS.P3().Theta();
  } else {
    p     = cand->P();
    theta = cand->P3().Theta();
  }

  if( p < pMin || p > pMax ) {
    answer = kFALSE;
  }

  if( theta < thMin || theta > thMax ) {
    answer = kFALSE;
  }

  return answer;

}

void
TThrustVector::init(const double * valX, const double * valY,
		      const double * valZ, const double denominator,
		      const unsigned nTracks) {

  if (nTracks>0) { // make sure there is at least one track
    // calculate denominator
    calc_denom(valX,valY,valZ,nTracks);
    // get initial estimate for thrust axis
    double * init_axis = get_initial_axis(valX,valY,valZ,nTracks);
    // use initial estimate to get more refined axis
    double * final_axis = get_final_axis(init_axis[0],init_axis[1],valX,valY,
                            valZ,nTracks);
    // convert theta,phi to x,y,z
    double * theAxis = get_axis(final_axis[0],final_axis[1]);
    // set axis
    _axis = TVector3( theAxis[0], theAxis[1], theAxis[2] ); 
    if ( _axis.CosTheta()<0 ) 
      {
	_axis *= -1;
      } // reverse
    // set thrust
    _thrust = calc_thrust(theAxis,valX,valY,valZ,nTracks);
    // clean up
    delete [] init_axis;  delete [] final_axis; delete [] theAxis;
  } // end if nTracks>0
}

void
TThrustVector::Reset() 
{
  _thrust=0;
  _axis = TVector3(0.,0.,0.);
  _denom_sum=0.;
}

TThrustVector::~TThrustVector() {}


// return theta, phi initial
double * TThrustVector::get_initial_axis(const double * valX, 
					   const double * valY,
					   const double * valZ, 
					   const unsigned nTracks) const {

  int i,j;
  // get initial estimate for thrust axis
  double rInitial[3]; // 3 dimensions
  double thrust[nSegs], max=0;
  int indI=0,indJ=0,index = -1; // index to max

  // calculate the thrust in different theta, phi regions
  for (i=0;i<nSegsTheta;i++) { // loop over theta
      double z = cos( TMath::Pi() * i / (nSegsTheta-1) ); // z
    double invZ = sqrt(1-z*z);
    for (j=0;j<nSegsPhi;j++) { // loop over phi
      rInitial[0] = invZ * cos( 2 * TMath::Pi() * j / nSegsPhi ); // x
      rInitial[1] = invZ * sin( 2 * TMath::Pi() * j / nSegsPhi ); // x
      rInitial[2] = z;
      // get thrust for this axis
      thrust[i*nSegsPhi+j] = calc_thrust(rInitial,valX,valY,valZ,nTracks);
      if (thrust[i*nSegsPhi+j]>max) {
        index = i*nSegsPhi+j;
        indI = i;
	indJ = j;
        max = thrust[index];
      } // end if thrust>max
    } // end for i
  } // end for j

  // take max and one point on either size, fitting to a parabola and
  // extrapolating to the real max.  Do this separately for each dimension.
  // y = ax^2 + bx + c.  At the max, x = 0, on either side, x = +/-1.
  // do phi first
  double a, b, c=max;
  int ind1 = indJ + 1;
  if (ind1>=nSegsPhi)
    ind1 -= nSegsPhi;
  int ind2 = indJ - 1;
  if (ind2<0)
    ind2 += nSegsPhi;
  a = ( thrust[ind1]+thrust[ind2]-2*c ) / 2;
  b = thrust[ind1] - a - c;
  double maxPhiInd = 0; // avoid divide by 0
  if (a!=0)
    maxPhiInd = -b/(2*a); // max 
  // do theta
  double maxThetaInd;
  if (indI==0||indI==(nSegsTheta-1)) // special case of end points
    maxThetaInd = indI;
  else { // not at end point
    ind1 = indI + 1;
    ind2 = indI - 1;
    a = ( thrust[ind1] + thrust[ind2] - 2*c ) / 2;
    b = thrust[ind1] - a - c; 
    maxThetaInd = 0; // avoid divide by 0
    if (a!=0)
      maxThetaInd = -b/(2*a); // max
  } // end else

  // calculate best guess at theta and phi then get starting point to iterate
  double bestTheta = TMath::Pi() * (maxThetaInd+indI) / (nSegsTheta-1);
  double bestPhi = 2 * TMath::Pi() * (maxPhiInd+indJ) / nSegsPhi;

  // set return values
  double * result = new double[2];
  result[0] = bestTheta;
  result[1] = bestPhi;
  return result;
} // end get_initial_axis

// get the final axis to be used
double * TThrustVector::get_final_axis(double bestTheta, double bestPhi,
					 const double* valX, 
					 const double* valY,
					 const double* valZ, 
					 const unsigned nTracks) const {

  double maxChange1,maxChange2;
  double a,b,c;
  double theThrust;
  int mand_ct = 3; // mandatory number of passes
  int max_ct = 1000; // a very large number of iterations

  // loop until done
  int done;
  do { 
    // get three axis to estimate maximum
    double * theAxis = get_axis(bestTheta,bestPhi);
    double * Axis2 = get_axis(bestTheta+epsilon,bestPhi); // do differential
    double * Axis3 = get_axis(bestTheta-epsilon,bestPhi); // do differential

    // use parabolic approx as above
    c = calc_thrust(theAxis,valX,valY,valZ,nTracks);
    a = ( calc_thrust(Axis2,valX,valY,valZ,nTracks) - 2 * c +
          calc_thrust(Axis3,valX,valY,valZ,nTracks) ) / 2;
    b = calc_thrust(Axis2,valX,valY,valZ,nTracks) - a - c; 

    // calculate max
    maxChange1 = 10 * ( b<0 ? -1 : 1 ); // linear 
    if (a!=0)
      maxChange1 = -b/(2*a);

    // clean up
    delete [] theAxis; delete [] Axis2; delete [] Axis3;

    // make sure change is small to avoid convergence problems
    while (fabs(maxChange1*epsilon)>TMath::Pi()/4) {maxChange1 /= 2;} // small changes

    // special case, use a different phi
    if (maxChange1==0&&(bestTheta==0||bestTheta==TMath::Pi())) { 
      bestPhi += 2*TMath::Pi();
      if (bestPhi>2*TMath::Pi())
        bestPhi -= 2 * TMath::Pi();

      // get three axis to estimate maximum
      theAxis = get_axis(bestTheta,bestPhi);
      Axis2 = get_axis(bestTheta+epsilon,bestPhi); // do differential
      Axis3 = get_axis(bestTheta-epsilon,bestPhi); // do differential

      // use parabolic approx as above
      c = calc_thrust(theAxis,valX,valY,valZ,nTracks);
      a = ( calc_thrust(Axis2,valX,valY,valZ,nTracks) - 2 * c +
            calc_thrust(Axis3,valX,valY,valZ,nTracks) ) / 2;
      b = calc_thrust(Axis2,valX,valY,valZ,nTracks) - a - c;

      // calculate max
      maxChange1 = 10 * ( b<0 ? -1 : 1 ); // linear 
      if (a!=0)
        maxChange1 = -b/(2*a);

      // clean up
      delete [] theAxis; delete [] Axis2; delete [] Axis3;
    } // end special case

    // loop until change is at least good enough as started with
    do {
      Axis2 = get_axis(bestTheta+maxChange1*epsilon,bestPhi);
      theThrust = calc_thrust(Axis2,valX,valY,valZ,nTracks);
      if (theThrust<c)
        maxChange1 /= 2; // don't trust large jumps so be willing to reduce

      // clean up
      delete [] Axis2;
    } while (theThrust<c); // do until at least equal 

    // put bestTheta in correct units and check for consistency
    bestTheta += maxChange1 * epsilon;
    if (bestTheta>TMath::Pi()) {
      bestTheta = TMath::Pi() - (bestTheta-TMath::Pi());
      bestPhi += TMath::Pi();
      if (bestPhi>2*TMath::Pi())
        bestPhi -= 2 * TMath::Pi();
    } // end if >pi
    if (bestTheta<0) {
      bestTheta *= -1; // change sign
      bestPhi += TMath::Pi();
      if (bestPhi>2*TMath::Pi())
        bestPhi -= 2 * TMath::Pi();
    } // end if <0

    // do again for phi
    theAxis = get_axis(bestTheta,bestPhi);
    Axis2 = get_axis(bestTheta,bestPhi+epsilon); // do differential
    Axis3 = get_axis(bestTheta,bestPhi-epsilon); // do differential

    // use parabolic approx as above
    c = calc_thrust(theAxis,valX,valY,valZ,nTracks);
    a = ( calc_thrust(Axis2,valX,valY,valZ,nTracks) - 2 * c +
          calc_thrust(Axis3,valX,valY,valZ,nTracks) ) / 2;
    b = calc_thrust(Axis2,valX,valY,valZ,nTracks) - a - c;

    // get maximum
    maxChange2 = 10 * ( b<0 ? -1 : 1 ); // linear 
    if (a!=0)
      maxChange2 = -b/(2*a);

    // clean up
    delete [] theAxis; delete [] Axis2; delete [] Axis3;

    // require small change
    while (fabs(maxChange2*epsilon)>TMath::Pi()/4) { maxChange2 /= 2; }

    // loop until change is at least as good as started with
    do {
      Axis2 = get_axis(bestTheta,bestPhi+maxChange2*epsilon);
      theThrust = calc_thrust(Axis2,valX,valY,valZ,nTracks);
      if (theThrust<c)
        maxChange2 /= 2; // don't trust large jumps so be willing to reduce

      // clean up
      delete [] Axis2;
    } while (theThrust<c); // do until at least equal

    // put bestPhi in correct units and check for consistency
    bestPhi += maxChange2 * epsilon;
    if (bestPhi>2*TMath::Pi())
      bestPhi -= 2 * TMath::Pi();
    if (bestPhi<0)
      bestPhi += 2 * TMath::Pi();

    // update mandatory count
    if (mand_ct>0)
      mand_ct--;
    max_ct--;
    done = (maxChange1*maxChange1>1||maxChange2*maxChange2>1||mand_ct)
           &&(max_ct>0);
  } while (done);
  // continue until both are less than one epsilon

  // set return values
  double * result = new double[2];
  result[0] = bestTheta;
  result[1] = bestPhi;
  return result;
} // end of get_final_axis

// get x,y,z from theta phi
double * TThrustVector::get_axis(double theta, double phi) const {

  double * result = new double[3];
  result[2] = cos(theta); // z
  double theSin = sin(theta); // sin calculation is slow
  result[0] = theSin * cos(phi); // x
  result[1] = theSin * sin(phi); // y

  return result;
}  // end of get_axis

// calculate the thrust for a given axis and set of three vectors
double TThrustVector::calc_thrust(const double * axis, 
				    const double * valX, 
				    const double * valY,
				    const double * valZ,
				    const unsigned nTracks) const {

  double result = 0;
  double num_sum = 0;

  for (int i=0;i<nTracks;i++)
    // calculate sum of dot products
    num_sum += fabs( axis[0]*valX[i] + axis[1]*valY[i] + axis[2]*valZ[i] );

  // set return value
  if (_denom_sum>0) 
    result = num_sum/_denom_sum;
  return result;
}

void TThrustVector::calc_denom(const double * valX, const double * valY,
				 const double * valZ, 
				 const unsigned nTracks) {

    for (int i=0;i<nTracks;i++) // loop over all tracks
      // calculate denominator 
      _denom_sum += sqrt( valX[i]*valX[i] + valY[i]*valY[i] + 
			  valZ[i]*valZ[i] );
}
