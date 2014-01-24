// -------------------------------------------------------------------------
// -----                     PndGemSensor source file                  -----
// -----                  Created 12/02/2009  by R. Karabowicz         -----
// -------------------------------------------------------------------------

#include "PndGemSensor.h"

#include "TMath.h"
#include "TRandom.h"

#include <iostream>
#include <list>
#include <vector>

using std::cout;
using std::endl;
using std::flush;
using std::pair;
using std::vector;


// -----   Default constructor   -------------------------------------------
PndGemSensor::PndGemSensor() {

  fDetectorId = 0;
  fType       = 0;
  fPosition[0] = fPosition[1] = fPosition[2] = fRotation = fInnerRadius = fOuterRadius = fD = fStripAngle[0] = fStripAngle[1] = fPitch[0] = fPitch[1] = 0.;
  fNChannelsFront = fNChannelsBack = 0;
  fSigmaX = fSigmaY = fSigmaXY = 0.;
}
// -------------------------------------------------------------------------



// -----   Enhanced constructor (by z0 and d)  ------------------------------------------
PndGemSensor::PndGemSensor(TString tempName, Int_t detId, Int_t iType, 
			   Double_t x0, Double_t y0, Double_t z0,
			   Double_t rotation, 
			   Double_t innerRad, Double_t outerRad,
			   Double_t d, 
			   Double_t stripAngle0, Double_t stripAngle1,
			   Double_t pitch0, Double_t pitch1)
{

  fName = tempName.Data();
  fDetectorId   = detId;
  fType         = iType;
  fPosition[0]  = x0;
  fPosition[1]  = y0;
  fPosition[2]  = z0;  // z position of the station
  fRotation     = rotation;
  fInnerRadius  = innerRad;
  fOuterRadius  = outerRad;
  fD            = d;   // thickness of the station
  fStripAngle[0]= stripAngle0; // strip angle
  fStripAngle[1]= stripAngle1; // strip angle
  fPitch[0]     = pitch0; // strip pitch
  fPitch[1]     = pitch1; // strip pitch

  if ( fType == 0 ) { // r phi version
    fNChannelsFront = (Int_t)(TMath::Ceil(2.*TMath::Pi()*fInnerRadius/fPitch[0]));
    fNChannelsBack  = 2*(Int_t)(TMath::Ceil((fOuterRadius-fInnerRadius)/fPitch[1]));
  }
  if ( fType == 1 ) { // tilted version, should not be used any more
    fNChannelsFront = (Int_t)(TMath::Ceil(2.*TMath::Pi()*fInnerRadius/fPitch[0]));
    fNChannelsBack  = (Int_t)(TMath::Ceil(2.*TMath::Pi()*fInnerRadius/fPitch[1]));
  }
  if ( fType == 2 ) { // x y version
    fNChannelsFront =   (Int_t)(TMath::Ceil(2.*fOuterRadius/fPitch[0]))+
                        (Int_t)(TMath::Ceil(2.*fInnerRadius/fPitch[0]));
    fNChannelsBack  = 2*(Int_t)(TMath::Ceil(2.*fOuterRadius/fPitch[1]));
  }

  cout << tempName.Data() << " type " << fType << " has " << fNChannelsFront << " front and " << fNChannelsBack << " back channels" << endl;

  fSigmaX = fSigmaY = fSigmaXY = 0.;
}
// -------------------------------------------------------------------------

// -----   Enhanced constructor (by z0 and d)  ------------------------------------------
PndGemSensor::PndGemSensor(TString tempName, Int_t stationNr, Int_t sectorNr, Int_t iType, 
			   Double_t x0, Double_t y0, Double_t z0,
			   Double_t rotation, 
			   Double_t innerRad, Double_t outerRad,
			   Double_t d, 
			   Double_t stripAngle0, Double_t stripAngle1,
			   Double_t pitch0, Double_t pitch1)
{

  fName = tempName.Data();
  SetDetectorId(stationNr, sectorNr);
  fType         = iType;
  fPosition[0]  = x0;
  fPosition[1]  = y0;
  fPosition[2]  = z0;  // z position of the station
  fRotation     = rotation;
  fInnerRadius  = innerRad;
  fOuterRadius  = outerRad;
  fD            = d;   // thickness of the station
  fStripAngle[0]= stripAngle0; // strip angle
  fStripAngle[1]= stripAngle1; // strip angle
  fPitch[0]     = pitch0; // strip pitch
  fPitch[1]     = pitch1; // strip pitch

  if ( fType == 0 ) { // r phi version
    fNChannelsFront = (Int_t)(TMath::Ceil(2.*TMath::Pi()*fInnerRadius/fPitch[0]));
    fNChannelsBack  = 2*(Int_t)(TMath::Ceil((fOuterRadius-fInnerRadius)/fPitch[1]));
  }
  if ( fType == 1 ) { // tilted version, should not be used any more
    fNChannelsFront = (Int_t)(TMath::Ceil(2.*TMath::Pi()*fInnerRadius/fPitch[0]));
    fNChannelsBack  = (Int_t)(TMath::Ceil(2.*TMath::Pi()*fInnerRadius/fPitch[1]));
  }
  if ( fType == 2 ) { // x y version
    fNChannelsFront = 2*(Int_t)(TMath::Ceil((fOuterRadius-fInnerRadius)/fPitch[0]))+
                      4*(Int_t)(TMath::Ceil(fInnerRadius/fPitch[0]));
    fNChannelsBack  = 2*(Int_t)(TMath::Ceil(2.*fOuterRadius/fPitch[1]));
  }

  cout << tempName.Data() << " type " << fType << " has " << fNChannelsFront << " front and " << fNChannelsBack << " back channels" << endl;

  fSigmaX = fSigmaY = fSigmaXY = 0.;
}
// -------------------------------------------------------------------------

// -----   Copy constructor  -----------------------------------------------
PndGemSensor::PndGemSensor(const PndGemSensor &tempSensor) :
  TNamed(tempSensor),
  fDetectorId    (tempSensor.fDetectorId),
  fType          (tempSensor.fType),
  // fPosition      {tempSensor.fPosition[0],
  //   tempSensor.fPosition[1],
  //   tempSensor.fPosition[2]},
  fRotation      (tempSensor.fRotation),
  fInnerRadius   (tempSensor.fInnerRadius),
  fOuterRadius   (tempSensor.fOuterRadius),
  fD             (tempSensor.fD),
  // fStripAngle[0] (tempSensor.fStripAngle[0]),
  // fStripAngle[1] (tempSensor.fStripAngle[1]),
  // fPitch[0]      (tempSensor.fPitch[0]),
  // fPitch[1]      (tempSensor.fPitch[1]),
  fNChannelsFront(tempSensor.fNChannelsFront),
  fNChannelsBack (tempSensor.fNChannelsBack),
  fSigmaX        (tempSensor.fSigmaX),
  fSigmaY        (tempSensor.fSigmaY),
  fSigmaXY       (tempSensor.fSigmaXY)
{
}
// -------------------------------------------------------------------------


// -----   Destructor   ----------------------------------------------------
PndGemSensor::~PndGemSensor() { };
// -------------------------------------------------------------------------





// -----   Public method Reset   -------------------------------------------
void PndGemSensor::Reset() {
}
// -------------------------------------------------------------------------



// -----   Public method Print   -------------------------------------------
void PndGemSensor::Print() {
  cout << fName.Data() << ": Sensor #";
  cout.width(3);
  cout << GetSensorNr() << ", Type ";
  cout.width(1);
  cout << fType << ", centre (";
  cout.width(6);
  cout << fPosition[0] << ", ";
  cout.width(6);
  cout << fPosition[1] << ", ";
  cout.width(6);
  cout << fPosition[2] << ") cm, rotation " ;
  cout.width(6);
  cout << fRotation*180./TMath::Pi() << " deg, radius from ";
  cout.width(6);
  cout << fInnerRadius << " to ";
  cout.width(6);
  cout << fOuterRadius << ", thickness ";
  cout.width(6);
  cout << fD << " cm, strip angle (";
  cout.width(6);
  cout << fStripAngle[0] << ", ";
  cout.width(6);
  cout << fStripAngle[1] << ") cm, pitch (";
  cout.width(6);
  cout << fPitch[0] << ", ";
  cout.width(6);
  cout << fPitch[1] << ") cm. ";
  cout << endl;

  Int_t tempSId = fDetectorId;
  Int_t bc = 0;
  cout << "SENSOR :                                          " << flush;
  while ( tempSId > 0 ) {
    bc++;
    cout << "\b" << tempSId%2 << "\b" << flush;
    if ( bc == 27 || bc == 21 || bc == 8 || bc == 6 || bc == 5 )
      cout << "\b|\b" << flush;
    tempSId = tempSId/2;
  }
  cout << endl;

}
// -------------------------------------------------------------------------

// -----   Public method GetChannel   --------------------------------------
Int_t PndGemSensor::GetChannel(Double_t x, Double_t y, Int_t iSide) {

  if (iSide !=0 && iSide != 1) {
    cout << "-W- PndGemSensor::GetChannel: Illegal side number " 
	 << iSide << endl;
    return -1;
  }

  if ( !Inside(x,y) ) return -1;
  Double_t radius = TMath::Sqrt(x*x+y*y);
  
  if ( fType == 1 ) {
    cout << "do not use this type anymore" << endl;
    return -1;
  }
  if ( fType == 0 ) { // angle info for iSide 0, radius info for iSide 1
    if ( iSide == 0 ) {
      Double_t hitPhi = TMath::ACos(y/radius);
      if ( x < 0. ) 
	hitPhi = 2.*TMath::Pi()-hitPhi;
      hitPhi = 2.*TMath::Pi()-hitPhi;
      return (Int_t)(TMath::Ceil((hitPhi*fInnerRadius)/fPitch[iSide]));
    }
    if ( iSide == 1 ) {
      if ( x < 0. )
	return (Int_t)(TMath::Ceil((radius-fInnerRadius)/fPitch[1]));
      else
	return (Int_t)(TMath::Ceil((radius-fInnerRadius)/fPitch[1])) + fNChannelsBack/2;
    }
  }
  if ( fType == 2 ) { // x y strips
    if ( iSide == 0 ) { // x information encoded
      Int_t nlStrips = (Int_t)(TMath::Ceil((fOuterRadius-fInnerRadius)/fPitch[0]));
      Int_t nsStrips = (Int_t)(TMath::Ceil(fInnerRadius/fPitch[0]));
      if ( x <= -fInnerRadius )
	return (Int_t)((x+fOuterRadius)/fPitch[0]);
      if ( x < 0. && y >= 0. )
	return nlStrips           +(Int_t)((x+fInnerRadius)/fPitch[0]);
      if ( x < 0. && y < 0. )
	return nlStrips+  nsStrips+(Int_t)((x+fInnerRadius)/fPitch[0]);
      // now x can't be smaller than 0.
      if ( x >= fInnerRadius ) 
	return nlStrips+4*nsStrips+(Int_t)((x-fInnerRadius)/fPitch[0]);
      if ( y >= 0. )
	return nlStrips+2*nsStrips+(Int_t)((x)/fPitch[0]);
      if ( y <  0. )
	return nlStrips+3*nsStrips+(Int_t)((x)/fPitch[0]);
    }
    if ( iSide == 1 ) { // y information encoded
      if ( x <  0. ) 
	return (Int_t)((y+fOuterRadius)/fPitch[1]);
      if ( x >= 0. ) 
	return (Int_t)((y+fOuterRadius)/fPitch[1])+fNChannelsBack/2;
    }
  }
  return -1;
}
// -------------------------------------------------------------------------

// -----   Public method GetNeighbours   -----------------------------------
Int_t PndGemSensor::GetNeighbours(Int_t iSide, Int_t iChan, Int_t& nChan1, Int_t& nChan2, Int_t& nChan3) {
  nChan1=-1;
  nChan2=-1;
  nChan3=-1;
  if ( fType == 0 ) {
    nChan1 = iChan-1;
    nChan2 = iChan+1;
    return 2;
  }
  if ( fType == 1 ) {
    nChan1 = iChan-1;
    nChan2 = iChan+1;
    return 2;
  }
  if ( fType == 2 ) {
    if ( iSide == 1 ) {
      nChan1 = iChan-1;
      nChan2 = iChan+1;
      return 2;
    }
    if ( iSide == 0 ) {
      nChan1 = iChan-1;
      nChan2 = iChan+1;
      return 2;
    }
  }
  return 0;
}
// -------------------------------------------------------------------------

// -----   Public method GetSensorPart   --------------------------------------
Int_t PndGemSensor::GetSensorPart(Int_t iSide, Int_t chan) {
  if ( fType == 0 ) {
    return -1;
  }
  if ( fType == 1 ) {
    return -1;
  }
  if ( fType == 2 ) {
    if ( iSide == 1 ) {
      return -1;
    }
    if ( iSide == 0 ) {
      Int_t nsStrips = (Int_t)(TMath::Ceil(fInnerRadius/fPitch[0]));
      if      ( TMath::Abs(chan-fNChannelsFront/2) > nsStrips*2 ) 
	return 0;
      else if ( TMath::Abs(chan-fNChannelsFront/2) < nsStrips   )
	return 2;
      else 
	return 1;
    }
  }
  return -1;
}
// -------------------------------------------------------------------------

// -----   Public method GetStripOrientation   --------------------------------------
Double_t PndGemSensor::GetStripOrientation(Double_t x, Double_t y, Int_t iSide) {
  if (iSide !=0 && iSide != 1) return -1.;
  if ( !Inside(x,y) ) return -1.;

  if ( fType == 1 ) {
    return -1.;
  }
  if ( fType == 0 ) { // angle info for iSide 0, radius info for iSide 1
    if ( iSide == 0 ) {
      Double_t radius = TMath::Sqrt(x*x+y*y);
      Double_t hitPhi = TMath::ACos(y/radius);
      if ( x < 0. ) 
	hitPhi = 2.*TMath::Pi()-hitPhi;
      hitPhi = 2.*TMath::Pi()-hitPhi;
      return hitPhi;
    }
    if ( iSide == 1 ) {
      Double_t radius = TMath::Sqrt(x*x+y*y);
      Double_t hitPhi = TMath::ACos(y/radius);
      if ( x < 0. ) 
	hitPhi = 2.*TMath::Pi()-hitPhi;
      hitPhi = 2.*TMath::Pi()-hitPhi;
      return hitPhi+TMath::Pi()/2.;
    }
  }
  if ( fType == 2 ) { // x y strips
    if ( iSide == 0 ) { // x information encoded
      return 0.;
    }
    if ( iSide == 1 ) { // y information encoded
      return TMath::Pi()/2.;
    }
  }
  return -1.;
}
// -------------------------------------------------------------------------

// -----   Public method GetDistance   --------------------------------------
Double_t PndGemSensor::GetDistance(Int_t iSide, Double_t chan1, Double_t chan2) {
  if ( iSide == 0 && ( chan1 - fNChannelsFront/2. - 1 ) * ( chan2 - fNChannelsFront/2. ) < 0. ) return -1.;
  if ( iSide == 1 && ( chan1 - fNChannelsBack /2. - 1 ) * ( chan2 - fNChannelsBack /2. ) < 0. ) return -1.;
  if ( fType == 0 ) {
    return TMath::Abs(chan1-chan2);
  }
  if ( fType == 1 ) {
    return TMath::Abs(chan1-chan2);
  }
  if ( fType == 2 ) {
    if ( iSide == 0 ) {
      Int_t nlStrips = (Int_t)(TMath::Ceil((fOuterRadius-fInnerRadius)/fPitch[0]));
      Int_t nsStrips = (Int_t)(TMath::Ceil(fInnerRadius/fPitch[0]));
      Int_t part1 = 666;
      Int_t part2 = 666;
      if      ( TMath::Abs(chan1-fNChannelsFront/2) >= nsStrips*2 ) part1 =  0;
      else if ( TMath::Abs(chan1-fNChannelsFront/2) <  nsStrips   ) {
	part1 = -1;
	if ( chan1 < fNChannelsFront/2 ) chan1 = chan1-nsStrips;
	else                             chan1 = chan1+nsStrips;
      }
      else                                                         part1 =  1;
      if      ( TMath::Abs(chan2-fNChannelsFront/2) >= nsStrips*2 ) part2 =  0;
      else if ( TMath::Abs(chan2-fNChannelsFront/2) <  nsStrips   ) {
	part2 = -1;
	if ( chan2 < fNChannelsFront/2 ) chan2 = chan2-nsStrips;
	else                             chan2 = chan2+nsStrips;
      }
      else                                                         part2 =  1;

      if ( part1*part2 == -1 ) return -1;
      return TMath::Abs(chan1-chan2);
    }
    if ( iSide == 1 ) {
      return TMath::Abs(chan1-chan2);
    }
  }
}
// -------------------------------------------------------------------------

// -----   Public method GetDistance   -------------------------------------
Int_t PndGemSensor::GetDistance(Int_t iSide, Int_t chanMin, Int_t chanMax, Int_t chanTest) {
  if ( iSide == 0 && ( chanMin - fNChannelsFront/2 ) * ( chanTest - fNChannelsFront/2 ) < 0. ) return -1;
  if ( iSide == 1 && ( chanMin - fNChannelsBack /2 ) * ( chanTest - fNChannelsBack /2 ) < 0. ) return -1;
  if ( fType == 0 ) {
    if ( chanTest > chanMin && chanTest < chanMax ) return 0;
    return TMath::Min(TMath::Abs(chanTest-chanMax),TMath::Abs(chanTest-chanMin));
  }
  if ( fType == 1 ) {
    if ( chanTest > chanMin && chanTest < chanMax ) return 0;
    return TMath::Min(TMath::Abs(chanTest-chanMax),TMath::Abs(chanTest-chanMin));
  }
  if ( fType == 2 ) {
    if ( iSide == 0 ) {
      Int_t nlStrips = (Int_t)(TMath::Ceil((fOuterRadius-fInnerRadius)/fPitch[0]));
      Int_t nsStrips = (Int_t)(TMath::Ceil(fInnerRadius/fPitch[0]));
      // THIS BROKEN VERTICAL STRIPS NEED SPECIAL ATTENTION
      // DO NOT YET CONSIDER ALL THE PROBLEMS...
      if      ( TMath::Abs(chanMin-fNChannelsFront/2) < nsStrips ) {
	//	cout << "chanMin " << chanMin << " is in bad position, consider " << fNChannelsFront << "/" << nlStrips << "/" << nsStrips << flush;
	if ( chanMin < fNChannelsFront/2 ) chanMin -= nsStrips;
	else                               chanMin += nsStrips;
	//	cout << " -----> " << chanMin << " ( "  << chanMax << " , " << chanTest << " )" << endl;
      }
      if      ( TMath::Abs(chanMax-fNChannelsFront/2) < nsStrips ) {
	//	cout << "chanMax " << chanMax << " is in bad position, consider " << fNChannelsFront << "/" << nlStrips << "/" << nsStrips << flush;
	if ( chanMax < fNChannelsFront/2 ) chanMax -= nsStrips;
	else                               chanMax += nsStrips;
	//	cout << " -----> " << chanMax << " ( "  << chanMin << " , " << chanTest << " )" << endl;
      }
      if      ( TMath::Abs(chanTest-fNChannelsFront/2) < nsStrips ) {
	//	cout << "chanTest " << chanTest << " is in bad position, consider " << fNChannelsFront << "/" << nlStrips << "/" << nsStrips << flush;
	if ( chanTest < fNChannelsFront/2 ) chanTest -= nsStrips;
	else                                chanTest += nsStrips;
	//	cout << " -----> " << chanTest << " ( "  << chanMin << " , " << chanMax << " )" << endl;
      }
      if ( chanTest > chanMin && chanTest < chanMax ) return 0;
      return TMath::Min(TMath::Abs(chanTest-chanMax),TMath::Abs(chanTest-chanMin));
    }
    if ( iSide == 1 ) {
      if ( chanTest > chanMin && chanTest < chanMax ) return 0;
      return TMath::Min(TMath::Abs(chanTest-chanMax),TMath::Abs(chanTest-chanMin));
    }
  }
}
// -------------------------------------------------------------------------

// -----   Public method GetMeanChannel   --------------------------------------
Double_t PndGemSensor::GetMeanChannel(Int_t iSide, Double_t chan1, Double_t weight1, Double_t chan2, Double_t weight2) {
  if ( iSide == 0 && ( chan1 - fNChannelsFront/2. ) * ( chan2 - fNChannelsFront/2. ) < 0. ) return -1.;
  if ( iSide == 1 && ( chan1 - fNChannelsBack /2. ) * ( chan2 - fNChannelsBack /2. ) < 0. ) return -1.;
  if ( fType == 0 ) {
    return (chan1*weight1+chan2*weight2)/(weight1+weight2);
  }
  if ( fType == 1 ) {
    return (chan1*weight1+chan2*weight2)/(weight1+weight2);
  }
  if ( fType == 2 ) {
    if ( iSide == 0 ) {
      Int_t nlStrips = (Int_t)(TMath::Ceil((fOuterRadius-fInnerRadius)/fPitch[0]));
      Int_t nsStrips = (Int_t)(TMath::Ceil(fInnerRadius/fPitch[0]));
      Int_t part1 = 666;
      Int_t part2 = 666;
      if      ( TMath::Abs(chan1-fNChannelsFront/2) >= nsStrips*2 ) part1 =  0;
      else if ( TMath::Abs(chan1-fNChannelsFront/2) <  nsStrips   ) {
	part1 = -1;
	if ( chan1 < fNChannelsFront/2 ) chan1 = chan1-nsStrips;
	else                             chan1 = chan1+nsStrips;
      }
      else                                                          part1 =  1;
      if      ( TMath::Abs(chan2-fNChannelsFront/2) >= nsStrips*2 ) part2 =  0;
      else if ( TMath::Abs(chan2-fNChannelsFront/2) <  nsStrips   ) {
	part2 = -1;
	if ( chan2 < fNChannelsFront/2 ) chan2 = chan2-nsStrips;
	else                             chan2 = chan2+nsStrips;
      }
      else                                                         part2 =  1;

      if ( part1*part2 == -1 ) return -1;

      Double_t meanCh = (chan1*weight1+chan2*weight2)/(weight1+weight2);

      if ( part1+part2 < 0 ) {
	if ( meanCh > nlStrips && meanCh < nlStrips+nsStrips ) meanCh += nsStrips;
	if ( meanCh > fNChannelsFront/2+nsStrips && meanCh < fNChannelsFront-nlStrips ) meanCh -= nsStrips;
      }

      return meanCh;
    }
    if ( iSide == 1 ) {
      return (chan1*weight1+chan2*weight2)/(weight1+weight2);
    }
  }
}
// -------------------------------------------------------------------------

// -----   Public method GetChannel2   --------------------------------------
Int_t PndGemSensor::GetChannel2(Double_t x, Double_t y, Int_t iSide, Double_t& feeDist) {
  feeDist = -1.;

  if (iSide !=0 && iSide != 1) {
    cout << "-W- PndGemSensor::GetChannel: Illegal side number " 
	 << iSide << endl;
    return -1;
  }

  if ( !Inside(x,y) ) return -1;
  Double_t radius = TMath::Sqrt(x*x+y*y);
  
  if ( fType == 1 ) {
    cout << "do not use this type anymore" << endl;
    return -1;
  }
  if ( fType == 0 ) { // angle info for iSide 0, radius info for iSide 1
    if ( iSide == 0 ) {
      feeDist = fOuterRadius - radius;

      Double_t hitPhi = TMath::ACos(y/radius);
      if ( x < 0. ) 
	hitPhi = 2.*TMath::Pi()-hitPhi;
      hitPhi = 2.*TMath::Pi()-hitPhi;
      return (Int_t)(TMath::Ceil((hitPhi*fInnerRadius)/fPitch[iSide]));
    }
    if ( iSide == 1 ) {
      Double_t hitPhi = TMath::ACos(y/radius);
      if ( x < 0. ) 
	hitPhi = 2.*TMath::Pi()-hitPhi;
      hitPhi = 2.*TMath::Pi()-hitPhi;
      //      feeDist = hitPhi;
      if ( hitPhi < TMath::Pi()/2. )
 	feeDist = hitPhi*radius;
      else if ( hitPhi < TMath::Pi() )
 	feeDist = (TMath::Pi()-hitPhi)*radius;
       else if ( hitPhi < 3.*TMath::Pi()/2. )
 	feeDist = (hitPhi-TMath::Pi())*radius;
       else
 	feeDist = (2.*TMath::Pi()-hitPhi)*radius;
       feeDist -= fStripAngle[1]/2.; // that's the width of middle bar

      if ( x < 0. )
	return (Int_t)(TMath::Ceil((radius-fInnerRadius)/fPitch[1]));
      else
	return (Int_t)(TMath::Ceil((radius-fInnerRadius)/fPitch[1])) + fNChannelsBack/2;
    }
  }
  if ( fType == 2 ) { // x y strips
    if ( iSide == 0 ) { // x information encoded
      Double_t hitPhi = TMath::ACos(y/radius);
      if ( x < 0. ) 
	hitPhi = 2.*TMath::Pi()-hitPhi;
      hitPhi = 2.*TMath::Pi()-hitPhi;
      Double_t feeY = TMath::Cos(hitPhi)*fOuterRadius;
      feeDist = TMath::Abs(feeY)-TMath::Abs(y);

      Int_t nlStrips = (Int_t)(TMath::Ceil((fOuterRadius-fInnerRadius)/fPitch[0]));
      Int_t nsStrips = (Int_t)(TMath::Ceil(fInnerRadius/fPitch[0]));
      if ( x <= -fInnerRadius )
	return (Int_t)((x+fOuterRadius)/fPitch[0]);
      if ( x < 0. && y >= 0. )
	return (Int_t)(nlStrips           +(x+fInnerRadius)/fPitch[0]);
      if ( x < 0. && y < 0. )
	return (Int_t)(nlStrips+  nsStrips+(x+fInnerRadius)/fPitch[0]);
      // now x can't be smaller than 0.
      if ( x >= fInnerRadius ) 
	return (Int_t)(nlStrips+4*nsStrips+(x-fInnerRadius)/fPitch[0]);
      if ( y >= 0. )
	return (Int_t)(nlStrips+2*nsStrips+(x)/fPitch[0]);
      if ( y <  0. )
	return (Int_t)(nlStrips+3*nsStrips+(x)/fPitch[0]);
    }
    if ( iSide == 1 ) { // y information encoded
      feeDist = TMath::Abs(x) - fStripAngle[1]/2.;

      if ( x <  0. ) 
	return (y+fOuterRadius)/fPitch[1];
      if ( x >= 0. ) 
	return (y+fOuterRadius)/fPitch[1]+fNChannelsBack/2;
    }
  }
  return -1.;
}
// -------------------------------------------------------------------------
// -----   Public method GetChannel   --------------------------------------
Double_t PndGemSensor::GetChannel(Double_t x, Double_t y, Int_t iSide, Double_t& stripWidth) {
  stripWidth = fPitch[iSide];

  if (iSide !=0 && iSide != 1) {
    cout << "-W- PndGemSensor::GetChannel: Illegal side number " 
	 << iSide << endl;
    return -1;
  }

  if ( !Inside(x,y) ) return -1;
  Double_t radius = TMath::Sqrt(x*x+y*y);
  
  if ( fType == 1 ) {
    cout << "do not use this type anymore" << endl;
    return -1;
  }
  if ( fType == 0 ) { // angle info for iSide 0, radius info for iSide 1
    if ( iSide == 0 ) {
      stripWidth = stripWidth*radius/fInnerRadius;
      Double_t hitPhi = TMath::ACos(y/radius);
      if ( x < 0. ) 
	hitPhi = 2.*TMath::Pi()-hitPhi;
      hitPhi = 2.*TMath::Pi()-hitPhi;
      return (hitPhi*fInnerRadius)/fPitch[iSide];
    }
    if ( iSide == 1 ) {
      if ( x < 0. )
	return (radius-fInnerRadius)/fPitch[1];
      else
	return (radius-fInnerRadius)/fPitch[1] + fNChannelsBack/2;
    }
  }
  if ( fType == 2 ) { // x y strips
    if ( iSide == 0 ) { // x information encoded
      Int_t nlStrips = (Int_t)(TMath::Ceil((fOuterRadius-fInnerRadius)/fPitch[0]));
      Int_t nsStrips = (Int_t)(TMath::Ceil(fInnerRadius/fPitch[0]));
      if ( x <= -fInnerRadius )
	return (x+fOuterRadius)/fPitch[0];
      if ( x < 0. && y >= 0. )
	return nlStrips           +(x+fInnerRadius)/fPitch[0];
      if ( x < 0. && y < 0. )
	return nlStrips+  nsStrips+(x+fInnerRadius)/fPitch[0];
      // now x can't be smaller than 0.
      if ( x >= fInnerRadius ) 
	return nlStrips+4*nsStrips+(x-fInnerRadius)/fPitch[0];
      if ( y >= 0. )
	return nlStrips+2*nsStrips+(x)/fPitch[0];
      if ( y <  0. )
	return nlStrips+3*nsStrips+(x)/fPitch[0];
    }
    if ( iSide == 1 ) { // y information encoded
      if ( x <  0. ) 
	return (y+fOuterRadius)/fPitch[1];
      if ( x >= 0. ) 
	return (y+fOuterRadius)/fPitch[1]+fNChannelsBack/2;
    }
  }
  return -1.;
}
// -------------------------------------------------------------------------

// -----   Public method Inside   ------------------------------------------
Bool_t PndGemSensor::Inside(Double_t x, Double_t y) {
  if ( TMath::Abs(x) < fStripAngle[1]/2. ) return kFALSE;
  Double_t radSq = x*x+y*y;
  if ( radSq < fInnerRadius*fInnerRadius ) return kFALSE;
  if ( radSq > fOuterRadius*fOuterRadius ) return kFALSE;
  return kTRUE;
}
// -------------------------------------------------------------------------

// -----   Public method Inside   ------------------------------------------
Bool_t PndGemSensor::Inside(Double_t radius) {
  if ( radius < fInnerRadius ) return kFALSE;
  if ( radius > fOuterRadius ) return kFALSE;
  return kTRUE;
}
// -------------------------------------------------------------------------

// -----   Public method Intersect   ---------------------------------------
Int_t PndGemSensor::Intersect(Double_t iFStrip, Double_t iBStrip, Double_t& xCross, Double_t& yCross, Double_t& zCross) {
  //  cout << "trying to find intersection of strip " << iFStrip << " and " << iBStrip << endl;

  if ( fType == -1 ) {
    cout << "not supported anymore" << endl;
    return -1;
  }
  // the hits are on different sides
  if ( iFStrip <  fNChannelsFront/2 && iBStrip >= fNChannelsBack/2 ) return -1;
  if ( iFStrip >= fNChannelsFront/2 && iBStrip <  fNChannelsBack/2 ) return -1;
  
  Double_t bs = iBStrip;
  if ( bs >= fNChannelsBack/2 ) bs -= fNChannelsBack/2;
  if ( fType == 0 ) { // r phi strips
    Double_t phi    = fPitch[0]*((Double_t)iFStrip-0.5) / fInnerRadius;
    Double_t radius = fPitch[1]*((Double_t) bs    -0.5) + fInnerRadius;
    yCross =  radius*TMath::Cos(phi);
    xCross = -radius*TMath::Sin(phi);
    zCross = fPosition[2];
    if ( Inside(xCross,yCross) )
      return fDetectorId;
    else
      return -1;
  }
  if ( fType == 2 ) { // x y strips
    Int_t nlStrips = (Int_t)(TMath::Ceil((fOuterRadius-fInnerRadius)/fPitch[0]));
    Int_t nsStrips = (Int_t)(TMath::Ceil(fInnerRadius/fPitch[0]));
    Double_t x = -666.;
    yCross = -fOuterRadius+(Double_t(bs)+0.5)*fPitch[1];
    zCross = fPosition[2];
    if ( iFStrip < nlStrips ) {
      xCross = -fOuterRadius+(Double_t(iFStrip)+0.5)*fPitch[0];
      if ( !Inside(xCross,yCross) ) return -1;
      return fDetectorId;
    }
    if ( iFStrip < nlStrips+  nsStrips ) {
      if ( bs <  fNChannelsBack/4 ) return -1;
      xCross = -fOuterRadius+(Double_t(iFStrip)+0.5)*fPitch[0];
      if ( !Inside(xCross,yCross) ) return -1;
      return fDetectorId;
    }
    if ( iFStrip < nlStrips+2*nsStrips ) {
      if ( bs >= fNChannelsBack/4 ) return -1;
      xCross = -fOuterRadius+(Double_t(iFStrip-nsStrips)+0.5)*fPitch[0];
      if ( !Inside(xCross,yCross) ) return -1;
      return fDetectorId;
    }
    if ( iFStrip < nlStrips+3*nsStrips ) {
      if ( bs <  fNChannelsBack/4 ) return -1;
      xCross = -fOuterRadius+(Double_t(iFStrip-nsStrips)+0.5)*fPitch[0];
      if ( !Inside(xCross,yCross) ) return -1;
      return fDetectorId;
    }
    if ( iFStrip < nlStrips+4*nsStrips ) {
      if ( bs >= fNChannelsBack/4 ) return -1;
      xCross = -fOuterRadius+(Double_t(iFStrip-3*nsStrips)+0.5)*fPitch[0];
      if ( !Inside(xCross,yCross) ) return -1;
      return fDetectorId;
    }

  }

  return -1;
}
// -------------------------------------------------------------------------


// -----   Public method Intersect   ---------------------------------------
Int_t PndGemSensor::Intersect(Double_t iFStrip, Double_t iBStrip, Double_t& xCross, Double_t& yCross, Double_t& zCross,
			      Double_t& dr, Double_t& dp) {
  //  cout << "trying to find intersection of strip " << iFStrip << " and " << iBStrip << endl;

  if ( fType == -1 ) {
    cout << "not supported anymore" << endl;
    return -1;
  }
  // the hits are on different sides
  //cout << iFStrip << " of " << fNChannelsFront << " and " << iBStrip << " of " << fNChannelsBack << endl;
  if ( iFStrip <  fNChannelsFront/2 && iBStrip >= fNChannelsBack/2 ) return -1;
  if ( iFStrip >= fNChannelsFront/2 && iBStrip <  fNChannelsBack/2 ) return -1;
  Double_t bs = iBStrip;
  if ( bs >= fNChannelsBack/2 ) bs -= fNChannelsBack/2;
  if ( fType == 0 ) { // r phi strips
    Double_t phi    = fPitch[0]*((Double_t)iFStrip-0.5) / fInnerRadius;
    Double_t radius = fPitch[1]*((Double_t) bs    -0.5) + fInnerRadius;
    yCross =  radius*TMath::Cos(phi);
    xCross = -radius*TMath::Sin(phi);
    zCross = fPosition[2];

    dp = fPitch[0]*radius/fInnerRadius/TMath::Sqrt(12.);
    dr = fPitch[1]/TMath::Sqrt(12.);

    if ( Inside(xCross,yCross) )
      return fDetectorId;
    else
      return -1;
  }
  if ( fType == 2 ) { // x y strips
    Int_t nlStrips = (Int_t)(TMath::Ceil((fOuterRadius-fInnerRadius)/fPitch[0]));
    Int_t nsStrips = (Int_t)(TMath::Ceil(fInnerRadius/fPitch[0]));
    Double_t x = -666.;
    yCross = -fOuterRadius+(Double_t(bs)+0.5)*fPitch[1];
    zCross = fPosition[2];

    if      ( iFStrip < nlStrips )
      xCross = -fOuterRadius+(Double_t(iFStrip)+0.5)*fPitch[0];
    else if ( iFStrip < nlStrips+  nsStrips ) {
      if ( bs <  fNChannelsBack/4 ) return -1;
      xCross = -fOuterRadius+(Double_t(iFStrip)+0.5)*fPitch[0];
    }
    else if ( iFStrip < nlStrips+2*nsStrips ) {
      if ( bs >= fNChannelsBack/4 ) return -1;
      xCross = -fOuterRadius+(Double_t(iFStrip-nsStrips)+0.5)*fPitch[0];
    }
    else if ( iFStrip < nlStrips+3*nsStrips ) {
      if ( bs <  fNChannelsBack/4 ) return -1;
      xCross = -fOuterRadius+(Double_t(iFStrip-nsStrips)+0.5)*fPitch[0];
    }
    else if ( iFStrip < nlStrips+4*nsStrips ) {
      if ( bs >= fNChannelsBack/4 ) return -1;
      xCross = -fOuterRadius+(Double_t(iFStrip-2*nsStrips)+0.5)*fPitch[0];
    }
    else
      xCross = -fOuterRadius+(Double_t(iFStrip-2*nsStrips)+0.5)*fPitch[0];

    if ( !Inside(xCross,yCross) ) return -1;

    /*
      Double_t rMax = TMath::Sqrt( (TMath::Abs(xCross)+fPitch[0])*(TMath::Abs(xCross)+fPitch[0])+
      (TMath::Abs(yCross)+fPitch[1])*(TMath::Abs(yCross)+fPitch[1]) );
      Double_t rMin = TMath::Sqrt( (TMath::Abs(xCross)-fPitch[0])*(TMath::Abs(xCross)-fPitch[0])+
      (TMath::Abs(yCross)-fPitch[1])*(TMath::Abs(yCross)-fPitch[1]) );
`      Double_t phi1 = TMath::ATan( (TMath::Abs(yCross)-fPitch[1])/(TMath::Abs(xCross)+fPitch[0]) );
      Double_t phi2 = TMath::ATan( (TMath::Abs(yCross)+fPitch[1])/(TMath::Abs(xCross)-fPitch[0]) );
      dp = TMath::Abs(phi1-phi2);
      dp = TMath::Tan(dp)*(rMax+rMin)/2./TMath::Sqrt(12.);
      dr = (rMax-rMin)/TMath::Sqrt(12.);
      //     if ( dp > 350 ) 
      //       dp = TMath::Abs(360.-dp);
      */
    dr = fPitch[0]/TMath::Sqrt(12.);
    dp = fPitch[1]/TMath::Sqrt(12.);

    return fDetectorId;
  }
  
  return -1;
}
// -------------------------------------------------------------------------

// -----   Public method Intersect   ---------------------------------------
Int_t PndGemSensor::Intersect(Double_t iFStrip, Double_t iBStrip, Double_t& xCross, Double_t& yCross, Double_t& zCross,
			      Double_t& dx, Double_t& dy, Double_t& dr, Double_t& dp) {
  //  cout << "trying to find intersection of strip " << iFStrip << " and " << iBStrip << endl;

  if ( fType == -1 ) {
    cout << "not supported anymore" << endl;
    return -1;
  }
  // the hits are on different sides
  //cout << iFStrip << " of " << fNChannelsFront << " and " << iBStrip << " of " << fNChannelsBack << endl;
  if ( iFStrip <  fNChannelsFront/2 && iBStrip >= fNChannelsBack/2 ) return -1;
  if ( iFStrip >= fNChannelsFront/2 && iBStrip <  fNChannelsBack/2 ) return -1;
  Double_t bs = iBStrip;
  if ( bs >= fNChannelsBack/2 ) bs -= fNChannelsBack/2;
  if ( fType == 0 ) { // r phi strips
    Double_t phi    = fPitch[0]*((Double_t)iFStrip-0.5) / fInnerRadius; // the angle is counted from Y axis
    Double_t radius = fPitch[1]*((Double_t) bs    -0.5) + fInnerRadius;
    yCross =  radius*TMath::Cos(phi);
    xCross = -radius*TMath::Sin(phi);
    zCross = fPosition[2];

    dp = fPitch[0]*radius/fInnerRadius/TMath::Sqrt(12.);
    dr = fPitch[1]/TMath::Sqrt(12.);
    dx = dr*TMath::Abs(TMath::Sin(phi))+dp*TMath::Abs(TMath::Cos(phi));
    dy = dr*TMath::Abs(TMath::Cos(phi))+dp*TMath::Abs(TMath::Sin(phi));

    if ( Inside(xCross,yCross) )
      return fDetectorId;
    else
      return -1;
  }
  if ( fType == 2 ) { // x y strips
    Int_t nlStrips = (Int_t)(TMath::Ceil((fOuterRadius-fInnerRadius)/fPitch[0]));
    Int_t nsStrips = (Int_t)(TMath::Ceil(fInnerRadius/fPitch[0]));
    Double_t x = -666.;
    yCross = -fOuterRadius+(Double_t(bs)+0.5)*fPitch[1];
    zCross = fPosition[2];

    if      ( iFStrip < nlStrips )
      xCross = -fOuterRadius+(Double_t(iFStrip)+0.5)*fPitch[0];
    else if ( iFStrip < nlStrips+  nsStrips ) {
      if ( bs <  fNChannelsBack/4 ) return -1;
      xCross = -fOuterRadius+(Double_t(iFStrip)+0.5)*fPitch[0];
    }
    else if ( iFStrip < nlStrips+2*nsStrips ) {
      if ( bs >= fNChannelsBack/4 ) return -1;
      xCross = -fOuterRadius+(Double_t(iFStrip-nsStrips)+0.5)*fPitch[0];
    }
    else if ( iFStrip < nlStrips+3*nsStrips ) {
      if ( bs <  fNChannelsBack/4 ) return -1;
      xCross = -fOuterRadius+(Double_t(iFStrip-nsStrips)+0.5)*fPitch[0];
    }
    else if ( iFStrip < nlStrips+4*nsStrips ) {
      if ( bs >= fNChannelsBack/4 ) return -1;
      xCross = -fOuterRadius+(Double_t(iFStrip-2*nsStrips)+0.5)*fPitch[0];
    }
    else
      xCross = -fOuterRadius+(Double_t(iFStrip-2*nsStrips)+0.5)*fPitch[0];

    if ( !Inside(xCross,yCross) ) return -1;

    /*
      Double_t rMax = TMath::Sqrt( (TMath::Abs(xCross)+fPitch[0])*(TMath::Abs(xCross)+fPitch[0])+
      (TMath::Abs(yCross)+fPitch[1])*(TMath::Abs(yCross)+fPitch[1]) );
      Double_t rMin = TMath::Sqrt( (TMath::Abs(xCross)-fPitch[0])*(TMath::Abs(xCross)-fPitch[0])+
      (TMath::Abs(yCross)-fPitch[1])*(TMath::Abs(yCross)-fPitch[1]) );
`      Double_t phi1 = TMath::ATan( (TMath::Abs(yCross)-fPitch[1])/(TMath::Abs(xCross)+fPitch[0]) );
      Double_t phi2 = TMath::ATan( (TMath::Abs(yCross)+fPitch[1])/(TMath::Abs(xCross)-fPitch[0]) );
      dp = TMath::Abs(phi1-phi2);
      dp = TMath::Tan(dp)*(rMax+rMin)/2./TMath::Sqrt(12.);
      dr = (rMax-rMin)/TMath::Sqrt(12.);
      //     if ( dp > 350 ) 
      //       dp = TMath::Abs(360.-dp);
      */
    dx = fPitch[0]/TMath::Sqrt(12.);
    dy = fPitch[1]/TMath::Sqrt(12.);
    dr = 0.;
    dp = 0.;

    return fDetectorId;
  }
  
  return -1;
}
// -------------------------------------------------------------------------
 

ClassImp(PndGemSensor)
