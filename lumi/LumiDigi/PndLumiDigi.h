#ifndef PNDLUMIDIGI_H_
#define PNDLUMIDIGI_H_

#include "PndLumiStrip.h"
#include <cmath>
#include "TMath.h"
#include "TVector3.h"
#include "FairHit.h"

using std::cout;
using std::endl;

class PndLumiDigi : public FairHit
{
public:

	PndLumiDigi();

	PndLumiDigi(Int_t detID, TVector3 pos, TVector3 dpos, Int_t index,
			Int_t nrPlan, Int_t nrSensor, Int_t multi, PndLumiStrip strip,
			Double_t dQ_r, Double_t dQ_l, Double_t inStripId, Double_t outStripId,
			Int_t stripright,	Double_t eLoss, TString detname);

	virtual ~PndLumiDigi();

	//Methods
	Int_t GetMultiplicity() const {return fMultiplicity;}
	Int_t GetSensorID() const {return fSensorID;}
	Int_t GetPlanID() const {return fPlanID;}
	PndLumiStrip GetStrip() const {return fStrip;}

	Double_t GetChargeRatioRight() const {return fdQ_r;}
	Double_t GetChargeRatioLeft() const {return fdQ_l;}
	TVector3 GetPosition() const {return fPosition;}
	Double_t GetEntryStripId() const {return fEntryStripId;}
	Double_t GetExitStripId() const {return fExitStripId;}
	Int_t GetRightID() const {return fRightId;}
	Double_t GetEnergyLoss()  const {return fEnergyLoss;}
	TString GetDetName() const {return fDetName;}

	void Print(const Option_t* opt) const;

private:

	Int_t fPlanID, fSensorID ,
	fMultiplicity, fIndex, fRightId;

	Double_t fdQ_r, fdQ_l, fEnergyLoss;
	Double_t fEntryStripId,fExitStripId;

	TVector3 fPosition;

	PndLumiStrip fStrip;

	std::vector<PndLumiStrip> fVStrip;

	TString fDetName;

	ClassDef(PndLumiDigi,1);
};

#endif /*PNDLUMIDIGI_H_*/


