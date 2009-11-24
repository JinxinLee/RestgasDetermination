/////////////////////////////////////////////////////////////
//
//--------------------------------------------------------------------------
// File and Version Information:
// 	$Id:$
//
// Description:
//	EMC Digi.
//
//	 Software developed for the BaBar Detector at the SLAC B-Factory.
// Adapted for the PANDA experiment at GSI		
//
// Author List:
//	Xiaorong Shi            Lawrence Livermore National Lab
//	Stephen J. Gowdy        University of Edinburgh
// Dima Melnichuk - adaption for PANDA
//
// Copyright Information:
//	Copyright (C) 1996	Lawrence Livermore National Lab
//
/////////////////////////////////////////////////////////////// 

#include "PndEmcDigi.h"
#include "PndEmcSharedDigi.h"
#include "PndEmcMapper.h"
#include "PndEmcStructure.h"
#include "PndEmcXtal.h"
#include "PndEmcDataTypes.h"
#include <iostream>

#include "TClass.h"
#include "TVector3.h"
#include "TBuffer.h"
#include "Riostream.h"

using namespace std;

// -----   Default constructor   -----------------------------------
PndEmcDigi::PndEmcDigi()
{
}
// -----------------------------------------------------------------


// -----   Destructor   --------------------------------------------
PndEmcDigi::~PndEmcDigi() {}
// -----------------------------------------------------------------
  
PndEmcDigi::PndEmcDigi(Int_t trackid, Int_t id, Float_t energy, Float_t time, Int_t hitIndex):fEnergy(energy),fTrackId(trackid),fDetectorId(id),fTime(time),fHitIndex(hitIndex),fWhere(0,0,0)
{
	PndEmcMapper *emcMap=PndEmcMapper::Instance(0);
	fTCI=emcMap->GetTCI(id);
	fThetaInd=fTCI->XCoord();
	fPhiInd=fTCI->YCoord();

	PndEmcTciXtalMap const &tciXtalMap=PndEmcStructure::Instance()->GetTciXtalMap();
	PndEmcXtal* xtal = tciXtalMap.find(fTCI)->second;
	fWhere = algPointer()(xtal);
	fTheta = fWhere.Theta();
	fPhi = fWhere.Phi();
}

//
// From another Digi
//

PndEmcDigi::PndEmcDigi( const PndEmcDigi& other ) 
: fEnergy( other.fEnergy ),
  fTime( other.fTime ),
  fTrackId(other.fTrackId),
  fDetectorId( other.fDetectorId),
  fHitIndex( other.fHitIndex)
{
	PndEmcMapper *emcMap=PndEmcMapper::Instance(0);
	fTCI=emcMap->GetTCI(fDetectorId);
	fThetaInd=fTCI->XCoord();
	fPhiInd=fTCI->YCoord();

	PndEmcTciXtalMap const &tciXtalMap=PndEmcStructure::Instance()->GetTciXtalMap();
	PndEmcXtal* xtal = tciXtalMap.find(fTCI)->second;
	fWhere = algPointer()(xtal);
	fTheta = fWhere.Theta();
	fPhi = fWhere.Phi();
}


void PndEmcDigi::SetDetectorId(Int_t id)
{
// PndEmcMapper assumed to instantiate first time with correct parameter before
	PndEmcMapper *emcMap=PndEmcMapper::Instance(0);
	fTCI=emcMap->GetTCI(id);
	fThetaInd=fTCI->XCoord();
	fPhiInd=fTCI->YCoord();
	fDetectorId=id;

	fThetaInd=fTCI->XCoord();
	fPhiInd=fTCI->YCoord();

	PndEmcTciXtalMap const &tciXtalMap=PndEmcStructure::Instance()->GetTciXtalMap();
	PndEmcXtal* xtal = tciXtalMap.find(fTCI)->second;
	fWhere = algPointer()(xtal);
	fTheta = fWhere.Theta();
	fPhi = fWhere.Phi();

}


TVector3
PndEmcDigi::surfacePosition( const PndEmcXtal* xtal )
{
  return xtal->frontCentre();
}

TVector3
PndEmcDigi::depthPosition( const PndEmcXtal* xtal )
{
	//cout << "depth" << endl;
	
	TVector3 pos = xtal->frontCentre();
	TVector3 norm=xtal->normalToFrontFace();
	norm*=fPositionDepth;
	pos += norm;
	
	return pos;
}

void
PndEmcDigi::selectDigiPositionMethod( PositionMethod alg, 
				   double rescaleFactor,
				   double positionDepth )
{

	TVector3 (*algorithm)( const PndEmcXtal* ) = 0;

	switch ( alg )
	{
		case surface:
			algorithm = PndEmcDigi::surfacePosition;
			fRescaleFactor = rescaleFactor;
			fPositionDepth = positionDepth;
			break;
	
		case depth:
			algorithm = PndEmcDigi::depthPosition;
			fRescaleFactor = rescaleFactor;
			fPositionDepth = positionDepth;
			break;
	
		default:
			cout << "PndEmcDigi::selectDigiPositionMethod. " 
					<< "Attempted to select unknown digi position method." 
					<< endl;
	}
	
	// Now actually set the pointer
	algPointer() = algorithm;
}

//Set the default digi position method

TVector3 ( *&PndEmcDigi::algPointer() ) ( const PndEmcXtal* )
{
  static TVector3 (*pointer) ( const PndEmcXtal* ) = PndEmcDigi::depthPosition;

  return pointer;
}

double PndEmcDigi::fRescaleFactor = 1.0;
double PndEmcDigi::fPositionDepth = 6.2;


const bool 
PndEmcDigi::isNeighbour( const PndEmcDigi* theDigi ) const
{
	if ((theDigi->GetTCI()==0)||(fTCI==0)){
		std::cout<<"TwoCoordIndex of digi is not defined"<<std::endl;
		abort();
	}
	
	if (fTCI->IsNeighbour(theDigi->GetTCI())) return true;

	return false;
}

// When PndEmcDigi is read from root file fTCI is not valid it should be validated later
void PndEmcDigi::ValidateTCI()
{
	PndEmcMapper *emcMap=PndEmcMapper::Instance(0);
	fTCI=emcMap->GetTCI(fDetectorId);

}

Short_t PndEmcDigi::GetXPad() const {
  // Return the X pad value for clusterization
  
  // Barrel EMC
  if (GetModule()==1 || GetModule()==2)
    return (GetCrystal()+(GetCopy()-1)*10);
  
  // Endcups and forward EMC
  if ((GetModule()==3) || (GetModule()==4) || (GetModule()==5))
    {
      if (GetCopy()==1) {  return -GetRow()+1;}
      if (GetCopy()==2) {  return -GetRow()+1; }
      if (GetCopy()==3) {  return  GetRow();}
      if (GetCopy()==4) {  return  GetRow();}
    }
  return -1000; // failure 

  // Test EMC
  if (GetModule()==6)
    return  GetRow();
}

Short_t PndEmcDigi::GetYPad() const {
  // Return the Y pad value for clusterization
  
  // Barrel EMC
  if (GetModule()==1)
    return (GetRow()+29);
  
  if (GetModule()==2)
    return (-GetRow()+30);
  
  // Endcups and forward EMC
  if ((GetModule()==3) || (GetModule()==4) || (GetModule()==5))
    {
      if (GetCopy()==1) {  return  GetCrystal();   }
      if (GetCopy()==2) {  return -GetCrystal()+1;     }
      if (GetCopy()==3) {  return -GetCrystal()+1; }
      if (GetCopy()==4) {  return  GetCrystal();       }
    } 

  // Test EMC
  if (GetModule()==6)
    return  GetCrystal();
  
  return -1000; // failure
}

Double_t PndEmcDigi::GetEnergy()  const
{
	return fEnergy;
}


bool
PndEmcDigi::operator==( const PndEmcDigi& otherDigi ) const
{
  bool equal = false;
  
  // 2 EmcDigis are equal is their relative energy difference whithin the folloowing tolerance
  Double_t energy_tolerance=1e-5;

  if ( this->GetThetaInt() == otherDigi.GetThetaInt() &&
       this->GetPhiInt() == otherDigi.GetPhiInt() &&
       ((this->GetEnergy() - otherDigi.GetEnergy())/this->GetEnergy()<energy_tolerance )) equal=true;
  
  return equal;
}

bool PndEmcDigi::operator!=(const PndEmcDigi& otherDigi) const 
{
  return ! ( *this == otherDigi );
}

bool PndEmcDigi::operator<( const PndEmcDigi & otherDigi) const
{
  if (fEnergy < otherDigi.fEnergy) 
    return true;
  else if (fEnergy == otherDigi.fEnergy && fTheta< otherDigi.fTheta)
    return true;
  else if (fEnergy == otherDigi.fEnergy && fTheta == otherDigi.fTheta && fPhi < otherDigi.fPhi)
    return true;

  return false;
}



// -----   Public method Print   -----------------------------------
void PndEmcDigi::Print(const Option_t* opt) const {
  cout << "EMC digi: cellid=" << GetDetectorId() << ", Energy=" << fEnergy;
  if (fTrackId>0) cout << ", TrackID= " << fTrackId;
//  cout << ", x=" << GetX() << ", y=" << GetY() << endl << flush; 
}


PndEmcSharedDigi*
PndEmcDigi::dynamic_cast_PndEmcSharedDigi()
{
  return 0;
}

const PndEmcSharedDigi*
PndEmcDigi::dynamic_cast_PndEmcSharedDigi() const
{
  return 0;
}

void PndEmcDigi::Streamer(TBuffer &R__b)
{
   // Stream an object of class PndEmcWaveform.

   if (R__b.IsReading()) {
      PndEmcDigi::Class()->ReadBuffer(R__b, this);
		PndEmcMapper *fEmcMap=PndEmcMapper::Instance(0);
		if (fEmcMap!=0) {
			fTCI=fEmcMap->GetTCI(fDetectorId);
		} else {
			fTCI=0;
		}

   } else {
      PndEmcDigi::Class()->WriteBuffer(R__b, this);
   }
}

ClassImp(PndEmcDigi)
