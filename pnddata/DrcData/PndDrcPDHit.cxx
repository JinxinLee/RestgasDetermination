// -------------------------------------------------------------------------
// -----                       PndDrcPDHit source file                   -----
// -----               Created 12/11/07  by Annalisa Cecchi            -----
// -----                                                               -----
// -----                                                               -----
// -------------------------------------------------------------------------
#include <iostream>

using std::cout;
using std::endl;

#include "PndDrcPDHit.h"

// -----   Default constructor   -------------------------------------------
PndDrcPDHit::PndDrcPDHit()
  :FairHit(),
   fTime(-999.),
   fTimeThreshold(-999.)
{  
}
// -------------------------------------------------------------------------


// -----   Standard constructor   ------------------------------------------

PndDrcPDHit::PndDrcPDHit(Int_t detID, 
			 Int_t sensorId, 
			 TVector3& pos, 
			 TVector3& dpos,
			 Double_t time,
			 Double_t timeThreshold,
			 Int_t index)
  :FairHit(detID, pos, dpos, index),fSensorId (sensorId), 
   fTime (time),fTimeThreshold (timeThreshold){ 
}

// -----   Destructor   ----------------------------------------------------
PndDrcPDHit::~PndDrcPDHit(){}
// -------------------------------------------------------------------------


// -----   Public method Print   -------------------------------------------
void PndDrcPDHit::Print(const Option_t* opt) const {
  cout << "DIRC Photon hit: Time = " << fTime << endl;
}
// -------------------------------------------------------------------------
void PndDrcPDHit::SetBarID(Int_t BarID){
  fBarID = BarID;
}
void PndDrcPDHit::SetBoxID(Int_t BoxID){
  fBoxID = BoxID;
}
void PndDrcPDHit::SetTrackID(Int_t TrackID){
  fTrackID = TrackID;
}
void PndDrcPDHit::SetTrackIniVertex(TVector3 TrackIniVertex){
  fTrackIniVertex = TrackIniVertex;
}
void PndDrcPDHit::SetMotherID(Int_t MrID){
  fMrID = MrID;
}
void PndDrcPDHit::SetPdgCode(Int_t Pdg){
  fPdg = Pdg;
}
void PndDrcPDHit::SetTrackMom(TVector3 TrackMom){
  fTrackMom = TrackMom;
}
void PndDrcPDHit::SetMotherIDPho(Int_t MrIDPho){
  fMrIDPho = MrIDPho;
}
void PndDrcPDHit::SetTimeAtBar(Double_t TimeAtBar){
  fTimeAtBar = TimeAtBar;
}
void PndDrcPDHit::SetEvtTim(Double_t EvtTim){
  fEvtTim = EvtTim;
}
void PndDrcPDHit::SetEventTim(Double_t EventTim){
  fEventTim = EventTim;
}
void PndDrcPDHit::SetPileUp(Double_t pileup){
  fPileUp = pileup;
}
void PndDrcPDHit::SetEventNo(Int_t EventNo){
  fEventNo = EventNo;
}

ClassImp(PndDrcPDHit)
