#include "PndMvdChargeWeightedPixelMapping.h"
#include "PndSdsIdealChargeConversion.h"
#include "PndSdsTotChargeConversion.h"
#include "FairLogger.h"

PndMvdChargeWeightedPixelMapping::PndMvdChargeWeightedPixelMapping(Int_t verbose)
:PndSdsChargeWeightedPixelMapping(),
	fDigiPar(NULL),
	fTotDigiPar(NULL)
{
	fVerbose = verbose;
	FairRun* ana = FairRun::Instance();
	FairRuntimeDb* rtdb=ana->GetRuntimeDb();
	fDigiPar = (PndSdsPixelDigiPar*)(rtdb->getContainer("MVDPixelDigiPar"));
	fTotDigiPar = (PndSdsTotDigiPar*)(rtdb->getContainer("MVDPixelTotDigiPar"));
	SetChargeConverter();
}

PndMvdChargeWeightedPixelMapping::PndMvdChargeWeightedPixelMapping(PndGeoHandling* geo, Int_t verbose)
:PndSdsChargeWeightedPixelMapping(geo),
	fDigiPar(NULL),
	fTotDigiPar(NULL)
{
	fVerbose = verbose;
	FairRun* ana = FairRun::Instance();
	FairRuntimeDb* rtdb=ana->GetRuntimeDb();
	fDigiPar = (PndSdsPixelDigiPar*)(rtdb->getContainer("MVDPixelDigiPar"));
	fTotDigiPar = (PndSdsTotDigiPar*)(rtdb->getContainer("MVDPixelTotDigiPar"));
	SetChargeConverter();
}


PndMvdChargeWeightedPixelMapping::PndMvdChargeWeightedPixelMapping(PndGeoHandling* geo, PndSdsPixelDigiPar* digiPar, PndSdsTotDigiPar* totPar)
:PndSdsChargeWeightedPixelMapping(geo), 
  fDigiPar(digiPar),
  fTotDigiPar(totPar)
{
	SetChargeConverter();
}

void PndMvdChargeWeightedPixelMapping::SetChargeConverter()
{
	LOG(INFO) << "ChargeConversionMethod: " << fDigiPar->GetChargeConvMethod() << " " << fDigiPar->GetThreshold() << FairLogger::endl;
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
	LOG(INFO) << "ChargeConversionMethod: " << fDigiPar->GetChargeConvMethod() << " " << fChargeConverter << FairLogger::endl;
}
