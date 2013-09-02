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
#include "PndDetectorList.h"
#include <iostream>

#include "TClass.h"
#include "TVector3.h"
#include "TBuffer.h"
#include "Riostream.h"

using namespace std;
TClonesArray* PndEmcDigi::fDigiArrayTBD = new TClonesArray("PndEmcDigi");
// -----   Default constructor   -----------------------------------
PndEmcDigi::PndEmcDigi():FairTimeStamp(0),fEnergy(0),fTrackId(-1),fDetectorId(-1),fHitIndex(-1),fWhere(0,0,0), fThetaInd(0), fPhiInd(0),fTheta(0), fPhi(0), fEvtNo(-1)
{
}
// -----------------------------------------------------------------


// -----   Destructor   --------------------------------------------
PndEmcDigi::~PndEmcDigi() {}
// -----------------------------------------------------------------
  
PndEmcDigi::PndEmcDigi(Int_t trackid, Int_t id, Float_t energy, Float_t time, Int_t hitIndex):FairTimeStamp(time),fEnergy(energy),fTrackId(trackid),fDetectorId(id),fHitIndex(hitIndex),fWhere(0,0,0), fThetaInd(0), fPhiInd(0),fTheta(0), fPhi(0)
{
	PndEmcMapper *emcMap=PndEmcMapper::Instance();
	PndEmcTwoCoordIndex* tci=emcMap->GetTCI(id);
	fThetaInd=tci->XCoord();
	fPhiInd=tci->YCoord();

	PndEmcTciXtalMap const &tciXtalMap=PndEmcStructure::Instance()->GetTciXtalMap();
	PndEmcXtal* xtal = tciXtalMap.find(tci)->second;
	fWhere = algPointer()(xtal);
	fTheta = fWhere.Theta();
	fPhi = fWhere.Phi();
	SetLink(FairLink("EmcHit", hitIndex));
}

//
// From another Digi
//

PndEmcDigi::PndEmcDigi( const PndEmcDigi& other ) 
: FairTimeStamp(other.GetTimeStamp()),
  fEnergy( other.fEnergy ),
  fTrackId(other.fTrackId),
  fDetectorId( other.fDetectorId),
  fHitIndex( other.fHitIndex),
  fThetaInd(0), fPhiInd(0),fTheta(0),fPhi(0), fWhere(0,0,0), fEvtNo(other.fEvtNo)
{
	PndEmcMapper *emcMap=PndEmcMapper::Instance();
	PndEmcTwoCoordIndex* tci=emcMap->GetTCI(fDetectorId);
	fThetaInd=tci->XCoord();
	fPhiInd=tci->YCoord();

	PndEmcTciXtalMap const &tciXtalMap=PndEmcStructure::Instance()->GetTciXtalMap();
	PndEmcXtal* xtal = tciXtalMap.find(tci)->second;
	fWhere = algPointer()(xtal);
	fTheta = fWhere.Theta();
	fPhi = fWhere.Phi();
}


void PndEmcDigi::SetDetectorId(Int_t id)
{
// PndEmcMapper assumed to instantiate first time with correct parameter before
	PndEmcMapper *emcMap=PndEmcMapper::Instance();
	PndEmcTwoCoordIndex* tci=emcMap->GetTCI(id);
	fThetaInd=tci->XCoord();
	fPhiInd=tci->YCoord();
	fDetectorId=id;

	fThetaInd=tci->XCoord();
	fPhiInd=tci->YCoord();

	PndEmcTciXtalMap const &tciXtalMap=PndEmcStructure::Instance()->GetTciXtalMap();
	PndEmcXtal* xtal = tciXtalMap.find(tci)->second;
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
	TVector3 norm = xtal->axisVector();
	int module=xtal->myIndex()->Index()/100000000;
	
	if (module==5) //shahslyk
		norm*=fPositionDepthShashlyk;
	else
		norm*=fPositionDepthPWO;
	
	pos += norm;
	
	return pos;
}

void
PndEmcDigi::selectDigiPositionMethod( PositionMethod alg, 
				double positionDepthPWO,
				double positionDepthShashlyk,
				double rescaleFactor)
{

	TVector3 (*algorithm)( const PndEmcXtal* ) = 0;

	switch ( alg )
	{
		case surface:
			algorithm = PndEmcDigi::surfacePosition;
			fRescaleFactor = rescaleFactor;
			fPositionDepthPWO = positionDepthPWO;
			fPositionDepthShashlyk = positionDepthShashlyk;
			break;
	
		case depth:
			algorithm = PndEmcDigi::depthPosition;
			fRescaleFactor = rescaleFactor;
			fPositionDepthPWO = positionDepthPWO;
			fPositionDepthShashlyk = positionDepthShashlyk;
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
double PndEmcDigi::fPositionDepthPWO = 6.2;
double PndEmcDigi::fPositionDepthShashlyk = 20.9;


const bool 
PndEmcDigi::isNeighbour( const PndEmcDigi* theDigi ) const
{
	PndEmcMapper *emcMap=PndEmcMapper::Instance();
	PndEmcTwoCoordIndex* tci=this->GetTCI();

	if ((theDigi->GetTCI()==0)||(tci==0)){
		std::cout<<"TwoCoordIndex of digi is not defined"<<std::endl;
		abort();
	}
	
	if (tci->IsNeighbour(theDigi->GetTCI())) return true;

	return false;
}

PndEmcTwoCoordIndex* PndEmcDigi::GetTCI() const
{
	PndEmcMapper *emcMap=PndEmcMapper::Instance();
	PndEmcTwoCoordIndex* tci=emcMap->GetTCI(fDetectorId);
	return tci;
};

Short_t PndEmcDigi::GetXPad() const {
  // Return the X pad value for clusterization
  
  // Barrel EMC
  if (GetModule()==1 || GetModule()==2)
    return (GetCrystal()+(GetCopy()-1)*10);
  
  // BwEndCap and forward EMC
  if ((GetModule()==4) || (GetModule()==5))
  {
    if (GetCopy()==1) {  return -GetRow()+1;}
    if (GetCopy()==2) {  return -GetRow()+1; }
    if (GetCopy()==3) {  return  GetRow();}
    if (GetCopy()==4) {  return  GetRow();}
  }
  
  // FwEndCap
  if (GetModule()==3)
    return -(GetCrystal()-36);//the minus sign before the paranthesis
			      //is introduced since the geometry of
			      //FwEndCap gets rotated by 180 deg
			      //around the y-axis in PndEmc.cxx;
                               
  //this rotation was done in turn due to the way the geometry was
  //defined in the geometry file of the forward end cap
    
  /*
    if (GetModule()==3 && GetCrystal()==999 && GetRow()==999)
    return GetCrystal();
  */
  
  // Test EMC
  if (GetModule()==6)
    return  GetRow();
  
 return -1000; // failure
}

Short_t PndEmcDigi::GetYPad() const {
  // Return the Y pad value for clusterization
  
  // Barrel EMC
  if (GetModule()==1)
    return (GetRow()+29);
  
  if (GetModule()==2)
    return (-GetRow()+30);
  
  // BwEndCap and forward EMC
  if ((GetModule()==4) || (GetModule()==5))
  {
    if (GetCopy()==1) {  return GetCrystal(); }
    if (GetCopy()==2) {  return -GetCrystal() + 1; }
    if (GetCopy()==3) {  return -GetCrystal() + 1; }
    if (GetCopy()==4) {  return GetCrystal(); }
  }
  
  // FwEndCap
  if (GetModule()==3)
    return (GetRow()- 37);
    
  /*
   if (GetModule()==3 && GetCrystal()==999 && GetRow()==999)
    return GetRow();
  */

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
  bool isEqual = false;
  
  // 2 EmcDigis are equal is their relative energy difference whithin the folloowing tolerance
  Double_t energy_tolerance=1e-5;

  if ( this->GetThetaInt() == otherDigi.GetThetaInt() &&
       this->GetPhiInt() == otherDigi.GetPhiInt() &&
       ((this->GetEnergy() - otherDigi.GetEnergy())/this->GetEnergy()<energy_tolerance )) isEqual =true;
  
  return isEqual;
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
  cout << "EMC digi: cellid=" << GetDetectorId() << ", Energy=" << fEnergy<<", Time="<<GetTimeStamp()<<", Evt="<<fEvtNo;
  if (fTrackId>0) cout << ", TrackID= " << fTrackId;
	//cout<<endl;
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

ClassImp(PndEmcDigi)
