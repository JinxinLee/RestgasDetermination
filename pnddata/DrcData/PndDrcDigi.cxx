#include "PndDrcDigi.h"

PndDrcDigi::PndDrcDigi() : FairTimeStamp()
{
  
}

PndDrcDigi::PndDrcDigi(std::vector<Int_t> index, Int_t detectorId, Int_t sensorId, Double_t charge, Double_t time, Int_t CSflag, Double_t timeStamp):
FairTimeStamp(timeStamp), fDetectorId(detectorId), fSensorId(sensorId), fTime(time), fCSflag(CSflag), fCharge(charge)
{
	AddIndex(index);	
}

PndDrcDigi::PndDrcDigi(Int_t index, Int_t detectorId, Int_t sensorId, Double_t charge, Double_t time, Int_t CSflag, Double_t timestamp):
FairTimeStamp(timestamp), fDetectorId(detectorId), fSensorId(sensorId), fTime(time), fCSflag(CSflag), fCharge(charge)
{
	AddIndex(index);
}
void PndDrcDigi::SetBarID(Int_t BarID){
  fBarID = BarID;
}
void PndDrcDigi::SetBoxID(Int_t BoxID){
  fBoxID = BoxID;
}
void PndDrcDigi::SetTrackID(Int_t TrackID){
  fTrackID = TrackID;
}
void PndDrcDigi::SetTrackIniVertex(TVector3 TrackIniVertex){
  fTrackIniVertex = TrackIniVertex;
}
void PndDrcDigi::SetMotherID(Int_t MrID){
  fMrID = MrID;
}
void PndDrcDigi::SetPdgCode(Int_t Pdg){
  fPdg = Pdg;
}
void PndDrcDigi::SetTrackMom(TVector3 TrackMom){
  fTrackMom = TrackMom;
}
void PndDrcDigi::SetMotherIDPho(Int_t MrIDPho){
  fMrIDPho = MrIDPho;
}
void PndDrcDigi::SetTimeAtBar(Double_t TimeAtBar){
  fTimeAtBar = TimeAtBar;
}
void PndDrcDigi::SetEvtTim(Double_t EvtTim){
  fEvtTim = EvtTim;
}
void PndDrcDigi::SetEventTim(Double_t EventTim){
  fEventTim = EventTim;
}
void PndDrcDigi::SetPileUp(Double_t pileup){
  fPileUp = pileup;
}
void PndDrcDigi::SetEventNo(Double_t EventNo){
  fEventNo = EventNo;
}


ClassImp(PndDrcDigi);
