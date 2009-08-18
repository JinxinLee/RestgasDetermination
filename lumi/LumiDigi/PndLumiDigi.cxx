#include "PndLumiDigi.h"

PndLumiDigi::PndLumiDigi()
: FairHit()
{
}

PndLumiDigi::PndLumiDigi(Int_t detID, TVector3 pos, TVector3 dpos,
		Int_t index, Int_t nrPlan, Int_t nrSensor, Int_t multi, PndLumiStrip strip,
		Double_t dQ_r, Double_t dQ_l, Double_t inStripId, Double_t outStripId,
		Int_t stripright,	Double_t eLoss, TString detname)
: FairHit(detID, pos, dpos, index)
{
	fPlanID = nrPlan;
	fSensorID = nrSensor;
	fPosition = pos;
	fMultiplicity = multi;
	fStrip = strip;
	fIndex = index;
	fdQ_r = dQ_r;
	fdQ_l = dQ_l;
	fEntryStripId = inStripId;
	fExitStripId = outStripId;
	fRightId = stripright;
	fEnergyLoss = eLoss;
	fDetName = detname;
}

PndLumiDigi::~PndLumiDigi()
{
}
void PndLumiDigi::Print(const Option_t* opt) const
{
    std::cout << " ******* LUMI DIGITIZATION **************************************************************** " << std::endl
    <<" * Detector hit	: "<<fDetName<<std::endl
	      << " * Plan ID	: " << fPlanID << std::endl
	      << " * Sensor ID	: " << fSensorID << std::endl
	      << " * Multiplicity	: " << fMultiplicity << std::endl
	      << " * Entry Position	: (" << fPosition.X() << ", " << fPosition.Y() << ", " << fPosition.Z() << ") [cm]" << std::endl
	      << " * Strip Involved	: " << fStrip << std::endl
	      << " ***************************************************************************************** "<< std::endl;

}

ClassImp(PndLumiDigi)

