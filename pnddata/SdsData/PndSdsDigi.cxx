#include "PndSdsDigi.h"

PndSdsDigi::PndSdsDigi(){
  fCharge = -1;
}

PndSdsDigi::PndSdsDigi(std::vector<Int_t> index, Int_t detID, Int_t sensorID, Int_t fe, Double_t charge, fDetectorType mcPointType, Int_t timestamp):
fDetID(detID), fSensorID(sensorID), fFE(fe), fCharge(charge), fTimestamp(timestamp)
{ 
  SetMCPointType(mcPointType);
	AddIndex(index);
}

PndSdsDigi::PndSdsDigi(Int_t index, Int_t detID, Int_t sensorID, Int_t fe, Double_t charge, fDetectorType mcPointType, Int_t timestamp):
fDetID(detID), fSensorID(sensorID), fFE(fe), fCharge(charge), fTimestamp(timestamp)
{
	SetMCPointType(mcPointType);
	AddIndex(index);
}

ClassImp(PndSdsDigi);
