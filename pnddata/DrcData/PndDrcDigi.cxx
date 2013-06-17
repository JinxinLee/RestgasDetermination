#include "PndDrcDigi.h"

PndDrcDigi::PndDrcDigi() : FairTimeStamp()
{
  
}

PndDrcDigi::PndDrcDigi(std::vector<Int_t> index, Int_t sensorID, Double_t charge, Double_t time, Int_t CSflag, Double_t timeStamp):
FairTimeStamp(timeStamp), fSensorID(sensorID), fTime(time), fCSflag(CSflag), fCharge(charge)
{
	AddIndex(index);	
}

PndDrcDigi::PndDrcDigi(Int_t index, Int_t sensorID, Double_t charge, Double_t time, Int_t CSflag, Double_t timestamp):
FairTimeStamp(timestamp), fSensorID(sensorID), fTime(time), fCSflag(CSflag), fCharge(charge)
{
	AddIndex(index);
}


ClassImp(PndDrcDigi);
