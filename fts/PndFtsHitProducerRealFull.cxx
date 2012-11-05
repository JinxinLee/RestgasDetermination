/////////////////////////////////////////////////////////////
// PndFtsHitProducerRealFull
//
// Class for digitalization for FTS
//
// authors: Pablo Genova - Pavia University
//          Lia Lavezzi  - Pavia University
//          
// modified for FTS by Isabella Garzia
/////////////////////////////////////////////////////////////

#include "PndFtsHitProducerRealFull.h"

#include "PndFtsHit.h"
#include "PndFtsHitInfo.h"
#include "PndFtsPoint.h"
#include "PndFtsSingleStraw.h"
#include "PndFtsMapCreator.h"
#include "PndFtsTube.h"

#include "FairRootManager.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"

#include "TGeoManager.h"
#include "TVector3.h"
#include "TRandom.h"
#include "TClonesArray.h"

#include <iostream>

using std::cout;
using std::endl;

// -----   Default constructor   -------------------------------------------
PndFtsHitProducerRealFull::PndFtsHitProducerRealFull() :
  FairTask("Real FTS Hit Producer"), fPointArray(new TClonesArray),  fHitArray(new TClonesArray),
  fHitInfoArray(new TClonesArray), fFtsParameters(new PndGeoFtsPar()),
  fPersistence(kTRUE)
{
}
// -------------------------------------------------------------------------



// -----   Destructor   ----------------------------------------------------
PndFtsHitProducerRealFull::~PndFtsHitProducerRealFull() { }
// -------------------------------------------------------------------------



// -----   Public method Init   --------------------------------------------
InitStatus PndFtsHitProducerRealFull::Init() {
  
  // Get RootManager
  FairRootManager* ioman = FairRootManager::Instance();
  if ( ! ioman ) {
    cout << "-E- PndFtsHitProducerRealFull::Init: "
	 << "RootManager not instantiated!" << endl;
    return kFATAL;
  }
  
  // Get input array
  fPointArray = (TClonesArray*) ioman->GetObject("FTSPoint");
  if ( ! fPointArray ) {
    cout << "-W- PndFtsHitProducerRealFull::Init: "
	 << "No FTSPoint array!" << endl;
    return kERROR;
  }

  // Create and register output array
  fHitArray = new TClonesArray("PndFtsHit");
  ioman->Register("FTSHit","FTS",fHitArray, fPersistence);
  
 // Create and register output array
  fHitInfoArray = new TClonesArray("PndFtsHitInfo");
  ioman->Register("FTSHitInfo", "FTS", fHitInfoArray, fPersistence);
  
  // CHECK added 
  PndFtsMapCreator *mapper = new PndFtsMapCreator(fFtsParameters);
  fTubeArray = mapper->FillTubeArray();

  cout << "-I- PndFtsHitProducerRealFull: Intialization successfull" << endl;
  
  return kSUCCESS;

}
// -------------------------------------------------------------------------

// CHECK added 
void PndFtsHitProducerRealFull::SetParContainers() {
  FairRuntimeDb* rtdb = FairRunAna::Instance()->GetRuntimeDb();
  fFtsParameters = (PndGeoFtsPar*) rtdb->getContainer("PndGeoFtsPar");
}

// -----   Public method Exec   --------------------------------------------
void PndFtsHitProducerRealFull::Exec(Option_t* opt) {

  //  Int_t evtn=0;	
  //  PndFtsPoint *ptemp =(PndFtsPoint*) fPointArray->At(0);
  //  if(ptemp !=NULL) {
  //    evtn=ptemp->GetEventID();
  //    if(evtn%50==0)cout << "Event Number "<<evtn<<endl;
  //  }
  
  // Reset output array
  if ( ! fHitArray ) Fatal("Exec", "No HitArray");
  
  fHitArray->Clear();
  fHitInfoArray->Clear();

  Int_t detID = 0;    // detectorID
  TVector3 pos, dpos; // position and error vectors

  // Declare some variables
  PndFtsPoint* point  = NULL;
   
  // Loop over FtsPoints
  Int_t nPoints = fPointArray->GetEntriesFast();
  for (Int_t iPoint = 0; iPoint < nPoints; iPoint++) {
    point  = (PndFtsPoint*) fPointArray->At(iPoint);
    if (point == NULL) continue;

    detID = point->GetDetectorID();
  
    // tubeID  CHECK added
    Int_t tubeID = point->GetTubeID();
    Int_t chamberID = point->GetChamberID();    
    PndFtsTube *tube = (PndFtsTube*) fTubeArray->At(tubeID);

    double InOut[6];
    memset(InOut, 0, sizeof(InOut));

    InOut[0] = point->GetXInLocal();
    InOut[1] = point->GetYInLocal();
    InOut[2] = point->GetZInLocal();
    InOut[3] = point->GetXOutLocal();
    InOut[4] = point->GetYOutLocal();
    InOut[5] = point->GetZOutLocal();
    
    // single straw tube simulation -----------------------
    PndFtsSingleStraw fts;
    
    //setting the single straw tube simulation constants
    // 3 options currently available:
    // TConst(tube radius (cm), gas pressure (bar), Ar%, CO2%)
    // fts.TConst(0.4, 1, 0.9, 0.1); 
    // fts.TConst(0.5, 1, 0.9, 0.1);
     fts.TConst(0.5, 2, 0.8, 0.2);
    
    // wire positioning   
    fts.PutWireXYZ(0.,  0., -75., 0., 0., 75.);

    // get particle momentum
    TVector3 momentum(point->GetPxOut(),point->GetPyOut(),point->GetPzOut()); // GeV/c
      
    Double_t GeV=1.;
    // position in cm (already in cm); momentum in GeV (already in GeV); mass in GeV (already in GeV)
      
    // drift time calculation
    Double_t pulset = fts.PartToTime(point->GetMass()/GeV, momentum.Mag()/GeV, InOut);
        
    // simulated radius (cm)
    double radius = fts.TimnsToDiscm(pulset);
    if(radius < 0.) radius = 0.; // CHECK
      
    // true radius (cm)
    double true_rad = fts.TrueDist(InOut);

    // dE calculation
    double depCharge = fts.PartToADC();
      
    // dE/dx calculation
    double dedx = -999;
    
    // fts: detID, pos, dpos, index come from --------------
    // fts (FairHit):
    Double_t closestDistanceError = 0.0150; // per adesso (stessa che in Ideal: 
                                            // radialResolution = 0.0150)

    TVector3 position = tube->GetPosition();

    // ----------------
    // TVector3 posInLocal(point->GetXInLocal(), point->GetYInLocal(), point->GetZInLocal());
    // TVector3 posOutLocal(point->GetXOutLocal(), point->GetYOutLocal(), point->GetZOutLocal());
    // Double_t zpos = position.Z() + ((posOutLocal.Z() + posInLocal.Z()) / 2.);
    // Double_t zposError;
    // FoldZPosWithResolution(zpos, zposError, posInLocal, posOutLocal);
    //    pos.SetXYZ(position.X(), position.Y(), zpos); // <--- stt2
    // ----------------

    pos.SetXYZ(position.X(), position.Y(), position.Z()); // <--- stt1

    //    dpos.SetXYZ(innerStrawDiameter / 2., innerStrawDiameter / 2., GetLongitudinalResolution(position.Z()));
    dpos.SetXYZ(0.5, 0.5, 3.); // per adesso (stessi che in Ideal:
                               // innerStrawDiameter/2 = 0.5,
                               // longitudinalResolution = 3.)


    // create hit
    AddHit(detID, tubeID, chamberID, iPoint, pos, dpos, pulset, radius, closestDistanceError, depCharge);

    AddHitInfo(0, 0, point->GetTrackID(), iPoint, 0, kFALSE);

  }// Loop over MCPoints
  
  // Event summary
  cout << "-I- PndFtsHitProducerRealFull: " << nPoints << " FtsPoints, "
       << nPoints << " Hits created." << endl;
  
}
// -------------------------------------------------------------------------
void PndFtsHitProducerRealFull::FoldZPosWithResolution(Double_t &zpos, Double_t &zposError, 
						    TVector3 localInPos, TVector3 localOutPos)
{
  Double_t
    zPosInStrawFrame = (localOutPos.Z() - localInPos.Z()) / 2.;
 
  //  zposError = gRandom->Gaus(0., GetLongitudinalResolution(zPosInStrawFrame));
  zposError = gRandom->Gaus(0., 3.); // per adesso (stesso che in Ideal: 
                                     // longitudinalResolution = 3.)

  zpos += zposError;
}



// -----   Private method AddHit   --------------------------------------------
PndFtsHit* PndFtsHitProducerRealFull::AddHit(Int_t detID, Int_t tubeID, Int_t chamberID, Int_t iPoint, TVector3& pos, TVector3& dpos, Double_t p, Double_t rsim, Double_t closestDistanceError, Double_t depcharge)
{
  // see PndSttHit for hit description
  TClonesArray& clref = *fHitArray;
  Int_t size = clref.GetEntriesFast();

  PndFtsHit *hitnew =  new(clref[size]) PndFtsHit(detID, tubeID, chamberID, iPoint, pos, dpos, p, rsim, closestDistanceError, depcharge);
  return hitnew;

}
// ----

// -----   Private method AddHitInfo   --------------------------------------------
PndFtsHitInfo* PndFtsHitProducerRealFull::AddHitInfo(Int_t fileNumber, Int_t eventNumber, Int_t trackID, Int_t pointID, Int_t nMerged, Bool_t isFake){
  // see PndSttHitInfo for hit description
  TClonesArray& clref = *fHitInfoArray;
  Int_t size = clref.GetEntriesFast();
  return new(clref[size])  PndFtsHitInfo(fileNumber, eventNumber, trackID, pointID, nMerged, isFake);
}






ClassImp(PndFtsHitProducerRealFull)
