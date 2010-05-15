#include "PndSdsDigiStrip.h"
#include "PndSdsDigi.h"

PndSdsDigiStrip::PndSdsDigiStrip(){
}

PndSdsDigiStrip::PndSdsDigiStrip(std::vector<Int_t> index, Int_t detID, 
 	  Int_t sensorID, Int_t fe, Int_t chan, Double_t charge, fDetectorType mcPointType, Int_t timestamp)
: PndSdsDigi(index,detID,sensorID,fe,charge, mcPointType)
{
	fChannel = chan;
	fTimestamp = timestamp;
}

PndSdsDigiStrip::PndSdsDigiStrip(Int_t index, Int_t detID,
  Int_t sensorID, Int_t fe, Int_t chan, Double_t charge, fDetectorType mcPointType, Int_t timestamp)
: PndSdsDigi(index,detID,sensorID,fe,charge, mcPointType)
{
	fChannel = chan;
	fTimestamp = timestamp;
}

// const void PndSdsDigiStrip::Print(){
//      std::cout << *((PndSdsStrip*)this) << std::endl;
// }

// Bool_t const HasNeighbour(const PndSdsDigiStrip& d2)
// {
//   return true; // Dummy so far
// }

Bool_t PndSdsDigiStrip::operator==(const PndSdsDigiStrip& d2) const
{
return (
          fDetID==d2.GetDetID()
          && fSensorID==d2.GetSensorID()
          && fFE==d2.GetFE()
          && fChannel==d2.GetChannel()
          && fCharge==d2.GetCharge()
//           && fMCID==d2.GetMCID()
          && fTimestamp==d2.GetTimestamp()
        );
}

ClassImp(PndSdsDigiStrip);
