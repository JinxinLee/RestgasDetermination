#include "PndMvdDigiStrip.h"
#include "PndMvdDigi.h"

PndMvdDigiStrip::PndMvdDigiStrip(){
}

PndMvdDigiStrip::PndMvdDigiStrip(std::vector<Int_t> index, Int_t detID,
  TString detName, Int_t fe, Int_t chan, Double_t charge, Int_t timestamp)
: PndMvdDigi(index,detID,detName,fe,charge)
{
// 	fIndex 	= index;
// 	fDetID  = detID;
// 	fDetName = detName;
// 	fFE = fe;
	fChannel = chan;
// 	fCharge = charge;
// 	fMCID = -1;
	fTimestamp = timestamp;
}

// const void PndMvdDigiStrip::Print(){
//      std::cout << *((PndMvdStrip*)this) << std::endl;
// }

// Bool_t const HasNeighbour(const PndMvdDigiStrip& d2)
// {
//   return true; // Dummy so far
// }

Bool_t PndMvdDigiStrip::operator==(const PndMvdDigiStrip& d2) const
{
return (
          fDetID==d2.GetDetID()
          && fDetName==d2.GetDetName()
          && fFE==d2.GetFE()
          && fChannel==d2.GetChannel()
          && fCharge==d2.GetCharge()
//           && fMCID==d2.GetMCID()
          && fTimestamp==d2.GetTimestamp()
        );
}

ClassImp(PndMvdDigiStrip);
