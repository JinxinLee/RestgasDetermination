/////////////////////////////////////////////////////////////
// PndSttHitProducerRealFast
//
// Class for digitalization for STT1
//
// authors: Pablo Genova - Pavia University
//          Lia Lavezzi  - Pavia University
//
/////////////////////////////////////////////////////////////

#include "PndSttHitProducerRealFast.h"

#include "PndSttHit.h"
#include "PndSttHitInfo.h"
#include "PndSttPoint.h"
#include "PndSttSingleStraw.h"

#include "CbmRootManager.h"

#include "TGeoManager.h"
#include "TClonesArray.h"
#include "TVector3.h"
#include "TRandom.h"
#include <iostream>
#include <cmath>

using std::cout;
using std::endl;
using std::sqrt;

// -----   Default constructor   -------------------------------------------
PndSttHitProducerRealFast::PndSttHitProducerRealFast() :
  CbmTask("Ideal STT Hit Producer") { }
// -------------------------------------------------------------------------



// -----   Destructor   ----------------------------------------------------
PndSttHitProducerRealFast::~PndSttHitProducerRealFast() { }
// -------------------------------------------------------------------------



// -----   Public method Init   --------------------------------------------
InitStatus PndSttHitProducerRealFast::Init() {
  fevtn=0;
 
 // Get RootManager
  CbmRootManager* ioman = CbmRootManager::Instance();
  if ( ! ioman ) {
    cout << "-E- PndSttHitProducerRealFast-wintz::Init: "
	 << "RootManager not instantiated!" << endl;
    return kFATAL;
  }
  
  // Get input array
  fPointArray = (TClonesArray*) ioman->GetObject("STTPoint");
  if ( ! fPointArray ) {
    cout << "-W- PndSttHitProducerRealFast::Init: "
	 << "No STTPoint array!" << endl;
    return kERROR;
  }

  // Create and register output array
  fHitArray = new TClonesArray("PndSttHit");
  ioman->Register("STTHit","STT",fHitArray,kTRUE);
  
 // Create and register output array
  fHitInfoArray = new TClonesArray("PndSttHitInfo");
  ioman->Register("STTHitInfo", "STT", fHitInfoArray, kTRUE);

  // Geometry loading
  TFile *tstfile=ioman->GetInFile();
  
  TGeoManager *geoMan = (TGeoManager*) tstfile->Get("CBMGeom");
  fVolumeArray = geoMan->GetListOfVolumes();

  fVolumeArray = gGeoManager->GetListOfVolumes();
  
  //cout << "-I- PndSttHitProducerRealFast: Intialization successfull" << endl;
  
  return kSUCCESS;

}
// -------------------------------------------------------------------------



// -----   Public method Exec   --------------------------------------------
void PndSttHitProducerRealFast::Exec(Option_t* opt) {

 
  if(fevtn%50==0) cout << "Event Number "<<fevtn<<endl;
  fevtn++;
  
  // Reset output array
  if ( ! fHitArray ) Fatal("Exec", "No HitArray");
  
  fHitArray->Clear();
  fHitInfoArray->Clear();

  Int_t detID = 0;    // detectorID
  TVector3 pos, dpos; // position and error vectors

  // Declare some variables
  PndSttPoint* point  = NULL;
   
 
  // Loop over SttPoints
  Int_t nPoints = fPointArray->GetEntriesFast();

  //  cout << "------------ " << nPoints << endl;

  for (Int_t iPoint = 0; iPoint < nPoints; iPoint++) {
    point  = (PndSttPoint*) fPointArray->At(iPoint);
    if (point == NULL) continue;

    detID = point->GetDetectorID();

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
    //stt.TConst(0.5, 1, 0.9, 0.1);//1 bar
    stt.TConst(0.5, 2, 0.8, 0.2);  //2 bar
    
    // wire positioning   
    stt.PutWireXYZ(0.,  0., -75., 0., 0., 75.);

    // get particle momentum
    TVector3 momentum(point->GetPxOut(),point->GetPyOut(),point->GetPzOut()); // GeV/c
      
    Double_t GeV=1.;
    // position in cm (already in cm); momentum in GeV (already in GeV); mass in GeV (already in GeV)
      
    // drift time calculation

    Double_t pulset =-1;
    //pulset = stt.PartToTime(point->GetMass()/GeV, momentum.Mag()/GeV, InOut);
     
    // constant initialization
    stt.TInit(point->GetMass()/GeV, momentum.Mag()/GeV, InOut);
     
    // true radius (cm)
    Double_t true_rad = stt.TrueDist(InOut);
   
    // simulated radius (cm)
    //Double_t radius = stt.TimnsToDiscm(pulset);
    //if(radius < 0.) radius = 0.; // CHECK
    //if(radius <0. ||radius==0.) radius =-999;
    
    // fast simulation 
    Double_t radius= stt.FastRec(true_rad,1) ; //,0) standard curve ,1) Juelich exp curve
                                               //Juelich is at 2 bar pressure
    // dE calculation
    //  double depCharge = stt.PartToADC();
      
     // dE calculation ------- check
    // charge calculation
    Double_t depcharge = stt.FastPartToADC(); // CHECK   arbitrary units!
    // dE/dx calculation
    TVector3 diff3(InOut[0] - InOut[3], InOut[1] - InOut[4], InOut[2] - InOut[5]);
    double distance = diff3.Mag(); //
    Double_t dedx = 999;
    if (distance != 0)  dedx = depcharge/(1000000 * distance);  // in arbitrary units
  
    Double_t halflength = point->GetTubeHalfLength(); 
    
    // stt2: detID, pos, dpos, index come from --------------
    // stt2 (CbmHit):
    Double_t closestDistanceError = GetError(radius);//calculates the error according                                                      to Juelich experimental curves
    //cout<<"radius "<<radius<<" error "<<closestDistanceError<<endl;                    
    //closestDistanceError = 0.0150; //150 microns check this point!                             
    //closestDistanceError =TMath::Sqrt(2.)*radius/TMath::Sqrt(12);
    TVector3 position(point->GetX(), point->GetY(), point->GetZ());

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

    // wire direction from stt2 -------------------------------
    TVector3 wireDirection(point->GetXWireDirection(), point->GetYWireDirection(), point->GetZWireDirection());
    // = 0, 0, 1 if only axias tubes
    // --------------------------------------------------------
    //    cout << "r: " << radius << " err: " << closestDistanceError << endl;
    //cout<<" radius "<<radius<<endl;
    // create hit
    AddHit(detID, pos, dpos, iPoint, point->GetTrackID(), pulset, radius, true_rad, closestDistanceError, wireDirection, halflength, depcharge, dedx);

    AddHitInfo(0, 0, point->GetTrackID(), iPoint, 0, kFALSE);

  }// Loop over MCPoints


  // Event summary
  //cout << "-I- PndSttHitProducerRealFast: " << nPoints << " SttPoints, "
  //     << nPoints << " Hits created." << endl;
  
}
// -------------------------------------------------------------------------
void PndSttHitProducerRealFast::FoldZPosWithResolution(Double_t &zpos, Double_t &zposError, 
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
PndSttHit* PndSttHitProducerRealFast::AddHit(Int_t detID, TVector3& pos, TVector3& dpos, Int_t iPoint, Int_t trackID, Double_t p, Double_t rsim, Double_t rtrue, Double_t closestDistanceError, TVector3 wireDirection, Double_t halflength, Double_t depcharge, Double_t dedx){
  // see PndSttHit for hit description
  TClonesArray& clref = *fHitArray;
  Int_t size = clref.GetEntriesFast();
  //cout << "-I- PndSttHitProducerRealFast: Adding Hit: track"<<trackID<<" event: "<<eventID<<" pulse = " << p << ", rsim = " << rsim 
  //     << ", rtrue = " << rtrue <<" name "<<nam<<"center.X "<<center.X()<< endl;
 
  PndSttHit *hitnew = new(clref[size]) PndSttHit(detID, pos, dpos, iPoint, trackID, p, rsim, rtrue, closestDistanceError, wireDirection);
  hitnew->SetDepCharge(depcharge);       // CHECK
  hitnew->SetEnergyLoss(depcharge/1e6);  // eloss in arbitrary units CHECK
  hitnew->SetdEdx(dedx);                 // CHECK
  hitnew->SetTubeHalfLength(halflength); // CHECK
  return hitnew;

  // return new(clref[size]) PndSttHit(detID, pos, dpos, iPoint, trackID, p, rsim, rtrue, closestDistanceError, wireDirection);
}
// ----

// -----   Private method AddHitInfo   --------------------------------------------
PndSttHitInfo* PndSttHitProducerRealFast::AddHitInfo(Int_t fileNumber, Int_t eventNumber, Int_t trackID, Int_t pointID, Int_t nMerged, Bool_t isFake){
  // see PndSttHitInfo for hit description
  TClonesArray& clref = *fHitInfoArray;
  Int_t size = clref.GetEntriesFast();
  return new(clref[size])  PndSttHitInfo(fileNumber, eventNumber, trackID, pointID, nMerged, isFake);
}


Double_t PndSttHitProducerRealFast::GetError(Double_t TrueDcm) {

  // data from julich 
  Double_t resmic=-1;
  if(TrueDcm < 0.48){
    resmic =    20. +1.48048e+02
      -3.35951e+02*TrueDcm 
      -1.87575e+03*pow(TrueDcm,2)  
      +1.92910e+04*pow(TrueDcm,3)   
      -6.90036e+04*pow(TrueDcm,4)   
      +1.07960e+05*pow(TrueDcm,5) 
      -5.90064e+04*pow(TrueDcm,6) ;  
  }
  else resmic=65.;
  
  return resmic*0.0001;
}

// void PndSttHitProducerRealFast::WriteHistograms(){
//   TFile* file = CbmRootManager::Instance()->GetOutFile();
//   file->cd();
//   file->mkdir("PndSttHitProducerRealFast");
//   file->cd("PndSttHitProducerRealFast");
  
//   dedxvsp->Write();
//   delete dedxvsp;
// }


Double_t PndSttHitProducerRealFast::TruncatedMean(Double_t vec[], Double_t perc, Int_t totalnum){

  Int_t i;
  // sorting
  Double_t a;

  for(Int_t j=1; j < totalnum; j++){
    a = vec[j]; 
    i = j-1;
    while(i >= 0 && vec[i] > a){
      vec[i+1] = vec[i];
      i--;
    }
    vec[i+1] = a;
  }

 cout << "ordinati: "<< endl; 
  for(Int_t j=1; j < totalnum; j++) cout << " " << vec[j] << endl;
  cout << endl;
 

  //truncated mean
  Double_t sum = 0;
  Int_t endnum = ceil(totalnum * perc);
  for(Int_t m = 0; m < endnum; m++) sum += vec[m];
 cout <<" totale " << totalnum << " endnum " <<  endnum <<endl;
  
  cout << "somma " << sum << " media " << sum/(Double_t) endnum << endl;


  return sum/(Double_t) endnum;
 }


ClassImp(PndSttHitProducerRealFast)
