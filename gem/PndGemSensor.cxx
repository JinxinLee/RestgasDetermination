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
  fPosition[0] = fPosition[1] = fPosition[2] = fRotation = fInnerRadius = fOuterRadius = fD = fStripAngle[0] = fStripAngle[1] = fPitch[0] = fPitch[1] = 0.;
  fNChannelsFront = fNChannelsBack = 0;
  fSigmaX = fSigmaY = fSigmaXY = 0.;
  cout << "-W- PndGemSensor: Do not use this constructor! " << endl;
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

  if ( TMath::Abs(fStripAngle[0]) < 89. ) {
    fNChannelsFront = (Int_t)(TMath::Ceil(2.*TMath::Pi()*fInnerRadius/fPitch[0]));
  }
  else {
    fNChannelsFront = (Int_t)(TMath::Ceil((fOuterRadius-fInnerRadius)/fPitch[0]));
  }
  if ( TMath::Abs(fStripAngle[1]) < 89. ) {
    fNChannelsBack  = (Int_t)(TMath::Ceil(2.*TMath::Pi()*fInnerRadius/fPitch[1]));
  }
  else {
    fNChannelsBack  = (Int_t)(TMath::Ceil((fOuterRadius-fInnerRadius)/fPitch[1]));
  }

  cout << tempName.Data() << " has " << fNChannelsFront << " front and " << fNChannelsBack << " back channels" << endl;

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

  if ( TMath::Abs(fStripAngle[0]) < 89. ) {
    fNChannelsFront = (Int_t)(TMath::Ceil(2.*TMath::Pi()*fInnerRadius/fPitch[0]));
  }
  else {
    fNChannelsFront = (Int_t)(TMath::Ceil((fOuterRadius-fInnerRadius)/fPitch[0]));
  }
  if ( TMath::Abs(fStripAngle[1]) < 89. ) {
    fNChannelsBack  = (Int_t)(TMath::Ceil(2.*TMath::Pi()*fInnerRadius/fPitch[1]));
  }
  else {
    fNChannelsBack  = (Int_t)(TMath::Ceil((fOuterRadius-fInnerRadius)/fPitch[1]));
  }

  cout << tempName.Data() << " has " << fNChannelsFront << " front and " << fNChannelsBack << " back channels" << endl;

  fSigmaX = fSigmaY = fSigmaXY = 0.;
}
// -------------------------------------------------------------------------


// -----   Destructor   ----------------------------------------------------
PndGemSensor::~PndGemSensor() { };
// -------------------------------------------------------------------------





// -----   Public method Reset   -------------------------------------------
void PndGemSensor::Reset() {
  fFrontActive.clear();
  fBackActive.clear();
  fTrueHits.clear();
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

  Double_t radius = TMath::Sqrt(x*x+y*y);
  Double_t cosAng = TMath::Cos(fStripAngle[iSide]*TMath::DegToRad());

  if ( !Inside(radius) ) return -1;

  if ( TMath::Abs(fStripAngle[iSide]) < 89. ) {
    Double_t corrThetaDeg = 0.;
    if ( fStripAngle[iSide] != 0. ) {
      Double_t xh = ( -2.*fInnerRadius*cosAng + TMath::Sqrt(4.*fInnerRadius*fInnerRadius*cosAng*cosAng-4.*fInnerRadius*fInnerRadius+4.*radius*radius) ) / 2.;
      corrThetaDeg = TMath::ACos((fInnerRadius*fInnerRadius+radius*radius-xh*xh)/(2.*fInnerRadius*radius));
      if ( fStripAngle[iSide] < 0 ) corrThetaDeg *= -1.;
   
      Double_t newX = x*TMath::Cos(corrThetaDeg)-y*TMath::Sin(corrThetaDeg);
      Double_t newY = x*TMath::Sin(corrThetaDeg)+y*TMath::Cos(corrThetaDeg);
      x = newX;
      y = newY;
    }

    Double_t hitTheta = 0;
    if ( y == 0 ) {
      hitTheta = (x>0?0.5:1.5);
    }
    else {
      hitTheta = TMath::ATan(x/y)/TMath::Pi();
      if ( y < 0. )   hitTheta = 1.+hitTheta;
      else 
	if ( x < 0. ) hitTheta = 2.+hitTheta;
    }
    
    return (Int_t)(TMath::Ceil((hitTheta*TMath::Pi()*fInnerRadius)/fPitch[iSide]));
  } 

  // concentric strips case
  return ( (Int_t)(TMath::Ceil((radius-fInnerRadius)/fPitch[iSide])) );
}
// -------------------------------------------------------------------------

// -----   Public method Inside   ------------------------------------------
Bool_t PndGemSensor::Inside(Double_t x, Double_t y) {
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
Int_t PndGemSensor::Intersect(Int_t iFStrip, Int_t iBStrip, Double_t& xCross, Double_t& yCross, Double_t& zCross) {
  //  cout << "trying to find intersection of strip " << iFStrip << " and " << iBStrip << endl;
  
  if ( fStripAngle[0] == 0. && fStripAngle[1] == 90. ) {
    Double_t theta  = fPitch[0]*((Double_t)iFStrip-0.5) / fInnerRadius;
    Double_t radius = fPitch[1]*((Double_t)iBStrip-0.5) + fInnerRadius;
    yCross = radius*TMath::Cos(theta);
    xCross = radius*TMath::Sin(theta);
    zCross = fPosition[2];
    return fDetectorId;
  }
  else {
    if ( fStripAngle[0] == - fStripAngle[1] && 
	 fPitch     [0] ==   fPitch     [1] ) {
      //      cout << iFStrip << " x " << iBStrip << flush;
      //      if ( iFStrip > 3*fNChannelsFront/4 && iBStrip < fNChannelsBack/4 ) iBStrip += fNChannelsBack;
      if ( iFStrip > iBStrip ) iBStrip += fNChannelsBack;
      //      cout << " --> " << iFStrip << " x " << iBStrip << flush;
      
      Double_t theta  = fPitch[0]*((Double_t)iFStrip+(Double_t)iBStrip-1.) / (2. * fInnerRadius);
      //      cout << " ----> theta = " << TMath::RadToDeg()*theta << flush;
      
      Double_t insideTheta = fPitch[0]*((Double_t)iBStrip-(Double_t)iFStrip) / (2. * fInnerRadius);
      //      cout << " ----> ins.theta = " << TMath::RadToDeg()*insideTheta << flush;
      
      if ( insideTheta < 0 || insideTheta > TMath::Pi()/2. ) { return -1; }
      
      Double_t radius = fInnerRadius*TMath::Cos(insideTheta);
      
      //      cout << " ----> radius = " << radius << flush;
      
      Double_t beta = TMath::Pi()-TMath::DegToRad()*fStripAngle[0]-(TMath::Pi()/2.-insideTheta);
      
      //      cout << " ----> beta = " << TMath::RadToDeg()*beta << flush;
      
      radius += TMath::Tan(beta)*fInnerRadius*TMath::Sin(insideTheta);
      
      //      cout << " >>> " << radius << endl;
      
      //      if ( radius > 0 ) { cout << endl; return -1;}
      
      if ( radius < fInnerRadius || radius > fOuterRadius ) { return -1;}
      
      //      cout << " at sensor " << fName.Data() << endl;
      
      //     Double_t theta1  = fPitch[0]*((Double_t)iFStrip-0.5) / fInnerRadius;
      //     Double_t theta2  = fPitch[0]*((Double_t)iBStrip-0.5) / fInnerRadius;
      
      //       Double_t radius = fPitch[1]*((Double_t)iBStrip-(Double_t)iFStrip-1.) /  2. + fInnerRadius;
      yCross = radius*TMath::Cos(theta);
      xCross = radius*TMath::Sin(theta);
      zCross = fPosition[2];
      return fDetectorId;
    }
    return -1;
  }
  //  }
  return -1;
}
// -------------------------------------------------------------------------


// -----   Public method Intersect   ---------------------------------------
Int_t PndGemSensor::Intersect(Int_t iFStrip, Int_t iBStrip, Double_t& xCross, Double_t& yCross, Double_t& zCross,
			      Double_t& dr, Double_t& dp) {
  //  cout << "trying to find intersection of strip " << iFStrip << " and " << iBStrip << endl;
  
  Double_t phi;

  if ( fStripAngle[0] == 0. && fStripAngle[1] == 90. ) {
    phi  = fPitch[0]*((Double_t)iFStrip-0.5) / fInnerRadius;
    Double_t radius = fPitch[1]*((Double_t)iBStrip-0.5) + fInnerRadius;
    yCross = radius*TMath::Cos(phi);
    xCross = radius*TMath::Sin(phi);
    zCross = fPosition[2];

    if ( phi > 2.*TMath::Pi() ) phi = phi - 2.*TMath::Pi();
    dp = fPitch[0]*radius/fInnerRadius/TMath::Sqrt(12.);
    dr = fPitch[1]/TMath::Sqrt(12.);

    return fDetectorId;
  }
  else {
    if ( fStripAngle[0] == - fStripAngle[1] && 
	 fPitch     [0] ==   fPitch     [1] ) {

      if ( iFStrip > iBStrip ) iBStrip += fNChannelsBack;
      
      phi  = fPitch[0]*((Double_t)iFStrip+(Double_t)iBStrip-1.) / (2. * fInnerRadius);
      
      Double_t insidePhi = fPitch[0]*((Double_t)iBStrip-(Double_t)iFStrip) / (2. * fInnerRadius);

      if ( insidePhi < 0 || insidePhi > TMath::Pi()/2. ) { return -1; }
      
      Double_t radius = fInnerRadius*TMath::Cos(insidePhi);
      
      Double_t beta = TMath::Pi()-TMath::DegToRad()*fStripAngle[0]-(TMath::Pi()/2.-insidePhi);
      
      radius += TMath::Tan(beta)*fInnerRadius*TMath::Sin(insidePhi);
      
      if ( radius < fInnerRadius || radius > fOuterRadius ) { return -1;}

 
      yCross = radius*TMath::Cos(phi);
      xCross = radius*TMath::Sin(phi);
      zCross = fPosition[2];

      insidePhi = fPitch[0]*((Double_t)iBStrip-(Double_t)iFStrip+1) / (2. * fInnerRadius);
      Double_t radiusBig = fInnerRadius*TMath::Cos(insidePhi);
      beta = TMath::Pi()-TMath::DegToRad()*fStripAngle[0]-(TMath::Pi()/2.-insidePhi);
      radiusBig += TMath::Tan(beta)*fInnerRadius*TMath::Sin(insidePhi);

      if ( phi > 2.*TMath::Pi() ) phi = phi - 2.*TMath::Pi();
      dr = 2.*(radiusBig - radius)/TMath::Sqrt(12.);
      dp = dr/TMath::Tan(beta);

      return fDetectorId;
    }
    return -1;
  }
  //  }
  return -1;
}
// -------------------------------------------------------------------------
 

ClassImp(PndGemSensor)
