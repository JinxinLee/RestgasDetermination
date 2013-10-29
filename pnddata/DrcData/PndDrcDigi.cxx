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


ClassImp(PndDrcDigi);
