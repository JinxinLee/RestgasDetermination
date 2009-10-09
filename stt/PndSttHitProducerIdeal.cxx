// -------------------------------------------------------------------------
// -----                CbmStsHitProducerIdeal source file             -----
// -----                  Created 10/01/06  by V. Friese               -----
// -------------------------------------------------------------------------

#include "PndSttHitProducerIdeal.h"

#include "PndSttHit.h"
#include "PndSttHitInfo.h"
#include "PndSttPoint.h"
#include "PndSttSingleStraw.h"

#include "FairRootManager.h"

#include "TClonesArray.h"
#include "TRandom.h"

#include <iostream>
#include <cmath>

using std::cout;
using std::endl;
using std::sqrt;

// TODO: read this from a configuration file
#define foldResolution 0 // <===== NO SMEARING
#define radialResolutionPolynomialConstant1 0.0150
#define radialResolutionPolynomialConstant2 0.
#define radialResolutionPolynomialConstant3 0.
//#define longitudinalResolutionPolynomialConstant1 3.
#define longitudinalResolutionPolynomialConstant1 0.0001
#define longitudinalResolutionPolynomialConstant2 0.
#define longitudinalResolutionPolynomialConstant3 0.

// TODO: read this from geant initialization
#define innerStrawDiameter 1. 

// -----   Default constructor   -------------------------------------------
PndSttHitProducerIdeal::PndSttHitProducerIdeal() :
  FairTask("Ideal STT Hit Producer") 
{ 
}
// -------------------------------------------------------------------------



// -----   Destructor   ----------------------------------------------------
PndSttHitProducerIdeal::~PndSttHitProducerIdeal() 
{ 
}
// -------------------------------------------------------------------------



// -----   Public method Init   --------------------------------------------
InitStatus PndSttHitProducerIdeal::Init() 
{
  // Get RootManager
  FairRootManager* ioman = FairRootManager::Instance();

  if ( ! ioman ) 
    {
      cout << "-E- PndSttHitProducerIdeal::Init: "
	   << "RootManager not instantiated!" << endl;
      return kFATAL;
    }

  // Get input array
  fPointArray = (TClonesArray*) ioman->GetObject("STTPoint");

  if ( ! fPointArray ) 
    {
      cout << "-W- PndSttHitProducerIdeal::Init: "
	   << "No STTPoint array!" << endl;
      return kERROR;
  }

  // Create and register output array
  fHitArray = new TClonesArray("PndSttHit");
  ioman->Register("STTHit", "STT", fHitArray, kTRUE);

  // Create and register output array
  fHitInfoArray = new TClonesArray("PndSttHitInfo");
  ioman->Register("STTHitInfo", "STT", fHitInfoArray, kTRUE);

  cout << "-I- PndSttHitProducerIdeal: Intialisation successfull" << endl;
  return kSUCCESS;
}
// -------------------------------------------------------------------------



// -----   Public method Exec   --------------------------------------------
void PndSttHitProducerIdeal::Exec(Option_t* opt) 
{
  // Reset output array
  if ( ! fHitArray ) 
    Fatal("Exec", "No HitArray");
  
  fHitArray->Clear();
  fHitInfoArray->Clear();
  
  // Declare some variables
  PndSttPoint
    *point = NULL;

  Int_t 
    detID = 0,       // Detector ID
    trackID = 0;     // GFTrack index

  TVector3 
    pos, dpos;       // Position and error vectors

  // Loop over SttPoints
  Int_t 
    nPoints = fPointArray->GetEntriesFast();
  Int_t counter=0;
  for (Int_t iPoint = 0; iPoint < nPoints; iPoint++) 
    {
      point = (PndSttPoint*) fPointArray->At(iPoint);

      if ( ! point) 
	continue;

      // Detector ID
      detID = point->GetDetectorID();

      // MCTrack ID
      trackID = point->GetTrackID();

      // Determine hit position and isochrone (x,y of wire, measured z  position)
      TVector3
	posInLocal(point->GetXInLocal(), point->GetYInLocal(), point->GetZInLocal()),
	posOutLocal(point->GetXOutLocal(), point->GetYOutLocal(), point->GetZOutLocal()),
	position(point->GetX(), point->GetY(), point->GetZ());

      Double_t
	closestDistance,
	closestDistanceError;
      
      //GetClostestApproachToWire(closestDistance, closestDistanceError, 
      //				posInLocal, posOutLocal);
  
      //minimum distance ----------------

      Double_t fd_in  = sqrt(point->GetXInLocal()*point->GetXInLocal()+point->GetYInLocal()*point->GetYInLocal());
      //cout<<fd_in<<endl;
      double InOut[6];
      memset(InOut, 0, sizeof(InOut));

      InOut[0] = point->GetXInLocal();
      InOut[1] = point->GetYInLocal();
      InOut[2] = point->GetZInLocal();
      InOut[3] = point->GetXOutLocal();
      InOut[4] = point->GetYOutLocal();
      InOut[5] = point->GetZOutLocal();
      
      PndSttSingleStraw stt;
      stt.PutWireXYZ(0.,  0., -75., 0., 0., 75.);
      closestDistance  = stt.TrueDist(InOut);
      closestDistanceError =0.;

      Double_t eloss = point->GetEnergyLoss();
      Double_t halflength = point->GetTubeHalfLength();
      //---------------------

      Double_t
	zpos = position.Z() + ((posOutLocal.Z() + posInLocal.Z()) / 2.),
	zposError;
      
      FoldZPosWithResolution(zpos, zposError, 
			     posInLocal, posOutLocal);
 
      TVector3
	  wireDirection(point->GetXWireDirection(), point->GetYWireDirection(), point->GetZWireDirection());

      // Create new hit
      //      pos.SetXYZ(position.X(), position.Y(), zpos);
      pos.SetXYZ(position.X(), position.Y(), position.Z()); // CHECK!
      dpos.SetXYZ(innerStrawDiameter / 2., innerStrawDiameter / 2., GetLongitudinalResolution(position.Z()));

      if(fabs(fd_in-0.5)>0.001) continue;
      
      PndSttHit *hit = NULL;
    
      hit = new ((*fHitArray)[counter]) PndSttHit(detID, pos, dpos, iPoint, 0,
				     closestDistance, closestDistanceError, wireDirection);;
      hit->SetTrackID(trackID);
      hit->SetEnergyLoss(eloss); // CHECK
      hit->SetDepCharge(0.);     // CHECK no charge simulation
      // dedx
      TVector3 diff3(InOut[0] - InOut[3], InOut[1] - InOut[4], InOut[2] - InOut[5]); 
      double distance = diff3.Mag(); //
      Double_t dedx = 999;
      if (distance != 0)  dedx = eloss/(distance);  // in GeV/cm (I guess) CHECK
      hit->SetdEdx(dedx);                 // CHECK
      hit->SetTubeHalfLength(halflength); // CHECK

      new ((*fHitInfoArray)[counter]) PndSttHitInfo(0, 0, trackID, iPoint,
						   0, kFALSE);
      counter++;
    }   // Loop over MCPoints

  // Event summary
  cout << "-I- PndSttHitProducerIdeal: " << nPoints << " SttPoints, "
       << nPoints << " Hits created." << endl;

}
// -------------------------------------------------------------------------

// -----   Private method GetRadialResolution-------------------------------
Double_t PndSttHitProducerIdeal::GetRadialResolution(Double_t radius)
{
  return radialResolutionPolynomialConstant1 + 
    radius * radialResolutionPolynomialConstant2 + 
    radius * radius * radialResolutionPolynomialConstant3;
}
// -------------------------------------------------------------------------



// -----   Private method GetLongitudinalResolution ------------------------
Double_t PndSttHitProducerIdeal::GetLongitudinalResolution(Double_t zpos)
{
  return longitudinalResolutionPolynomialConstant1 + 
    zpos * longitudinalResolutionPolynomialConstant2 + 
    zpos * zpos * longitudinalResolutionPolynomialConstant3;
}
// -------------------------------------------------------------------------

// -----   Private method GetClostestApproachToWire ------------------------
void PndSttHitProducerIdeal::GetClostestApproachToWire(Double_t &closestDistance, 
						       Double_t &closestDistanceError,
						       TVector3 localInPos, 
						       TVector3 localOutPos)
{
  Double_t
    a = (localOutPos.X() - localInPos.X()),
    b = (localOutPos.Y() - localInPos.Y()),
    c = sqrt(a * a + b * b) / 2.;
    
  // fold with Gaussian for resolution

  if (c > innerStrawDiameter / 2.) 
    {
      c = innerStrawDiameter / 2.;
    }
                
  closestDistance = sqrt((innerStrawDiameter / 2.) * (innerStrawDiameter / 2.) - c * c); 
  closestDistanceError = 0.;

  if (foldResolution)
    {
      closestDistanceError = GetRadialResolution(closestDistance);
      closestDistance += gRandom->Gaus(0., closestDistanceError);
    }
}
// -------------------------------------------------------------------------

void PndSttHitProducerIdeal::FoldZPosWithResolution(Double_t &zpos, Double_t &zposError, 
						    TVector3 localInPos, TVector3 localOutPos)
{
  Double_t
    zPosInStrawFrame = (localOutPos.Z() - localInPos.Z()) / 2.;
 
  zposError = gRandom->Gaus(0., GetLongitudinalResolution(zPosInStrawFrame));

  zpos += zposError;
  //  cout << "zpos in prod: " << zpos << endl;
}
 


ClassImp(PndSttHitProducerIdeal)
