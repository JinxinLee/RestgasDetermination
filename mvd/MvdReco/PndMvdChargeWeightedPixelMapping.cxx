#include "PndMvdChargeWeightedPixelMapping.h"
#include "PndSdsIdealChargeConversion.h"
#include "PndSdsTotChargeConversion.h"
#include "PndSdsPixelDigiPar.h"
#include "PndSdsTotDigiPar.h"

PndMvdChargeWeightedPixelMapping::PndMvdChargeWeightedPixelMapping(Int_t verbose)
:PndSdsChargeWeightedPixelMapping(){
	fVerbose = verbose;
	FairRun* ana = FairRun::Instance();
	FairRuntimeDb* rtdb=ana->GetRuntimeDb();
	PndSdsPixelDigiPar* fDigiPar = (PndSdsPixelDigiPar*)(rtdb->getContainer("MVDPixelDigiPar"));
	PndSdsTotDigiPar* fTotDigiPar = (PndSdsTotDigiPar*)(rtdb->getContainer("MVDPixelTotDigiPar"));
	if (fDigiPar->GetChargeConvMethod() == 0){
			if(fVerbose>0) Info("PndMvdChargeWeightedPixelMapping","ideal charge conversion");
			fChargeConverter = new PndSdsIdealChargeConversion(fDigiPar->GetNoise());
	}
	else if (fDigiPar->GetChargeConvMethod() == 1){
		if(fVerbose>0) Info("PndMvdChargeWeightedPixelMapping","use TOT charge conversion");
		fChargeConverter = new PndSdsTotChargeConversion(
			fTotDigiPar->GetChargingTime(),
			fTotDigiPar->GetConstCurrent(),
			fDigiPar->GetThreshold(),
			fTotDigiPar->GetClockFrequency(),
			fVerbose);
	}
	else {
		Fatal("PndMvdChargeWeightedPixelMapping", "conversion method not defined!");
	}

	flx = fDigiPar->GetXPitch();
	fly = fDigiPar->GetYPitch();
	fcols = fDigiPar->GetFECols();
	frows = fDigiPar->GetFERows();
}

PndMvdChargeWeightedPixelMapping::PndMvdChargeWeightedPixelMapping(PndGeoHandling* geo, Int_t verbose)
:PndSdsChargeWeightedPixelMapping(geo){
	fVerbose = verbose;
	FairRun* ana = FairRun::Instance();
	FairRuntimeDb* rtdb=ana->GetRuntimeDb();
	PndSdsPixelDigiPar* fDigiPar = (PndSdsPixelDigiPar*)(rtdb->getContainer("MVDPixelDigiPar"));
	PndSdsTotDigiPar* fTotDigiPar = (PndSdsTotDigiPar*)(rtdb->getContainer("MVDPixelTotDigiPar"));
	if (fDigiPar->GetChargeConvMethod() == 0){
			if(fVerbose>0) Info("PndMvdChargeWeightedPixelMapping","ideal charge conversion");
			fChargeConverter = new PndSdsIdealChargeConversion(fDigiPar->GetNoise());
	}
	else if (fDigiPar->GetChargeConvMethod() == 1){
		if(fVerbose>0) Info("PndMvdChargeWeightedPixelMapping","use TOT charge conversion");
		fChargeConverter = new PndSdsTotChargeConversion(
			fTotDigiPar->GetChargingTime(),
			fTotDigiPar->GetConstCurrent(),
			fDigiPar->GetThreshold(),
			fTotDigiPar->GetClockFrequency(),
			fVerbose);
	}
	else {
		Fatal("PndMvdChargeWeightedPixelMapping", "conversion method not defined!");
	}
	flx = fDigiPar->GetXPitch();
	fly = fDigiPar->GetYPitch();
	fcols = fDigiPar->GetFECols();
	frows = fDigiPar->GetFERows();
}
