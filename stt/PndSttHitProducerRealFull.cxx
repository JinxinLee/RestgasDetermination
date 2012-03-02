/////////////////////////////////////////////////////////////
// PndSttHitProducerRealFull
//
// Class for digitalization for STT1
//
// authors: Pablo Genova - Pavia University
//          Lia Lavezzi  - Pavia University
//
/////////////////////////////////////////////////////////////

#include "PndSttHitProducerRealFull.h"

#include "PndSttHit.h"
#include "PndSttHitInfo.h"
#include "PndSttPoint.h"
#include "PndSttSingleStraw.h"
#include "PndSttMapCreator.h"
#include "PndSttTube.h"

#include "PndSttHitWriteoutBuffer.h"

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
PndSttHitProducerRealFull::PndSttHitProducerRealFull() :
  FairTask("Ideal STT Hit Producer") {
  fPersistence = kTRUE;
}
// -------------------------------------------------------------------------



// -----   Destructor   ----------------------------------------------------
PndSttHitProducerRealFull::~PndSttHitProducerRealFull() { }
// -------------------------------------------------------------------------



// -----   Public method Init   --------------------------------------------
InitStatus PndSttHitProducerRealFull::Init() {
  
  // Get RootManager
  FairRootManager* ioman = FairRootManager::Instance();
  if ( ! ioman ) {
    cout << "-E- PndSttHitProducerRealFull::Init: "
	 << "RootManager not instantiated!" << endl;
    return kFATAL;
  }
  
  // Get input array
  fPointArray = (TClonesArray*) ioman->GetObject("STTPoint");
  if ( ! fPointArray ) {
    cout << "-W- PndSttHitProducerRealFull::Init: "
	 << "No STTPoint array!" << endl;
    return kERROR;
  }

  // Create and register output array
//  fHitArray = new TClonesArray("PndSttHit");
//  ioman->Register("STTHit","STT",fHitArray, fPersistence);

  fDataBuffer = new PndSttHitWriteoutBuffer("STTHit", "STT", fPersistence);
  fDataBuffer = (PndSttHitWriteoutBuffer*)ioman->RegisterWriteoutBuffer("STTHit", fDataBuffer);
  fDataBuffer->ActivateBuffering(fTimeOrderedDigi);
  
 // Create and register output array
  fHitInfoArray = new TClonesArray("PndSttHitInfo");
  ioman->Register("STTHitInfo", "STT", fHitInfoArray, fPersistence);
  
  // CHECK added 
  PndSttMapCreator *mapper = new PndSttMapCreator(fSttParameters);
  fTubeArray = mapper->FillTubeArray();
  
  cout << "-I- PndSttHitProducerRealFull: Intialization successfull" << endl;
  
  return kSUCCESS;

}
// -------------------------------------------------------------------------

// CHECK added 
void PndSttHitProducerRealFull::SetParContainers() {
  FairRuntimeDb* rtdb = FairRunAna::Instance()->GetRuntimeDb();
  fSttParameters = (PndGeoSttPar*) rtdb->getContainer("PndGeoSttPar");
}

// -----   Public method Exec   --------------------------------------------
void PndSttHitProducerRealFull::Exec(Option_t* opt) {

  //  Int_t evtn=0;	
  //  PndSttPoint *ptemp =(PndSttPoint*) fPointArray->At(0);
  //  if(ptemp !=NULL) {
  //    evtn=ptemp->GetEventID();
  //    if(evtn%50==0)cout << "Event Number "<<evtn<<endl;
  //  }
  
  // Reset output array
//  if ( ! fHitArray ) Fatal("Exec", "No HitArray");
//
//  fHitArray->Delete();
  fHitInfoArray->Delete();

  Int_t detID = 0;    // detectorID
  TVector3 pos, dpos; // position and error vectors

  // Declare some variables
  PndSttPoint* point  = NULL;

  Double_t EventTime = FairRootManager::Instance()->GetEventTime();
   
  // Loop over SttPoints
  Int_t nPoints = fPointArray->GetEntriesFast();
  for (Int_t iPoint = 0; iPoint < nPoints; iPoint++) {
    point  = (PndSttPoint*) fPointArray->At(iPoint);
    if (point == NULL) continue;

    detID = point->GetDetectorID();
  
    // tubeID  CHECK added
    Int_t tubeID = point->GetTubeID();
    PndSttTube *tube = (PndSttTube*) fTubeArray->At(tubeID);
    
    double InOut[6];
    memset(InOut, 0, sizeof(InOut));

    InOut[0] = point->GetXInLocal();
    InOut[1] = point->GetYInLocal();
    InOut[2] = point->GetZInLocal();
    InOut[3] = point->GetXOutLocal();
    InOut[4] = point->GetYOutLocal();
    InOut[5] = point->GetZOutLocal();
    
    // single straw tube simulation -----------------------
    PndSttSingleStraw stt;
    
    //setting the single straw tube simulation constants
    // 3 options currently available:
    // TConst(tube radius (cm), gas pressure (bar), Ar%, CO2%)
    // stt.TConst(0.4, 1, 0.9, 0.1); 
    // stt.TConst(0.5, 1, 0.9, 0.1);
     stt.TConst(0.5, 2, 0.8, 0.2);
    
    // wire positioning   
    stt.PutWireXYZ(0.,  0., -75., 0., 0., 75.);

    // get particle momentum
    TVector3 momentum(point->GetPxOut(),point->GetPyOut(),point->GetPzOut()); // GeV/c
      
    Double_t GeV=1.;
    // position in cm (already in cm); momentum in GeV (already in GeV); mass in GeV (already in GeV)
      
    // drift time calculation
    Double_t pulset = stt.PartToTime(point->GetMass()/GeV, momentum.Mag()/GeV, InOut);
//    std::cout << "pulset: " << pulset << " EventTime: " << EventTime;
    pulset += EventTime;
//    std::cout << " Sum: " << pulset << std::endl;
    // simulated radius (cm)
    double radius = stt.TimnsToDiscm(pulset);
    if(radius < 0.) radius = 0.; // CHECK
      
    // true radius (cm)
    double true_rad = stt.TrueDist(InOut);

    // dE calculation
    double depCharge = stt.PartToADC();
      
    // dE/dx calculation
    double dedx = -999;
    
    Double_t halflength = tube->GetHalfLength(); // CHECK added


    // stt2: detID, pos, dpos, index come from --------------
    // stt2 (FairHit):
    Double_t closestDistanceError = 0.0150; // per adesso (stessa che in Ideal: 
                                            // radialResolution = 0.0150)

    TVector3 position  = tube->GetPosition(); // CHECK added

    // ----------------
    // stt2, ma cancellati in stt1 (controlla: in stt2 la posizione dell' hit non 
    // corrisponde al centro del tubo (xcentro, ycentro, 35.), ma per il Real deve
    // essere cosi' ??perche' in stt2 non e' cosi'??
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
    //----- end stt2 ------------------------------------------


    // create hit
    AddHit(detID, tubeID, iPoint, pos, dpos, pulset, radius, closestDistanceError, depCharge);

    AddHitInfo(0, 0, point->GetTrackID(), iPoint, 0, kFALSE);

  }// Loop over MCPoints
  
  // Event summary
  cout << "-I- PndSttHitProducerRealFull: " << nPoints << " SttPoints, "
       << nPoints << " Hits created." << endl;
  
}
// -------------------------------------------------------------------------
void PndSttHitProducerRealFull::FoldZPosWithResolution(Double_t &zpos, Double_t &zposError, 
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
PndSttHit* PndSttHitProducerRealFull::AddHit(Int_t detID, Int_t tubeID, Int_t iPoint, TVector3& pos, TVector3& dpos, Double_t p, Double_t rsim, Double_t closestDistanceError, Double_t depcharge)
{
  // see PndSttHit for hit description

  PndSttHit *hitnew =  new PndSttHit(detID, tubeID, iPoint, pos, dpos, p, rsim, closestDistanceError, depcharge);
  fDataBuffer->FillNewData(hitnew, p);
  return hitnew;

}
// ----

// -----   Private method AddHitInfo   --------------------------------------------
PndSttHitInfo* PndSttHitProducerRealFull::AddHitInfo(Int_t fileNumber, Int_t eventNumber, Int_t trackID, Int_t pointID, Int_t nMerged, Bool_t isFake){
  // see PndSttHitInfo for hit description
  TClonesArray& clref = *fHitInfoArray;
  Int_t size = clref.GetEntriesFast();
  return new(clref[size])  PndSttHitInfo(fileNumber, eventNumber, trackID, pointID, nMerged, isFake);
}






ClassImp(PndSttHitProducerRealFull)
