#include "PndMQMvdChargeWeightedPixelMapping.h"
#include "PndSdsIdealChargeConversion.h"
#include "PndSdsTotChargeConversion.h"
#include "FairLogger.h"

PndMQMvdChargeWeightedPixelMapping::PndMQMvdChargeWeightedPixelMapping(Int_t verbose)
:PndMQSdsChargeWeightedPixelMapping(){
	fVerbose = verbose;
	FairRun* ana = FairRun::Instance();
	FairRuntimeDb* rtdb=ana->GetRuntimeDb();
	fDigiPar = (PndSdsPixelDigiPar*)(rtdb->getContainer("MVDPixelDigiPar"));
	fTotDigiPar = (PndSdsTotDigiPar*)(rtdb->getContainer("MVDPixelTotDigiPar"));

	SetChargeConverter();

}

PndMQMvdChargeWeightedPixelMapping::PndMQMvdChargeWeightedPixelMapping(PndGeoHandling* geo, Int_t verbose)
:PndMQSdsChargeWeightedPixelMapping(geo){
	fVerbose = verbose;
	FairRun* ana = FairRun::Instance();
	FairRuntimeDb* rtdb=ana->GetRuntimeDb();
	fDigiPar = (PndSdsPixelDigiPar*)(rtdb->getContainer("MVDPixelDigiPar"));
	fTotDigiPar = (PndSdsTotDigiPar*)(rtdb->getContainer("MVDPixelTotDigiPar"));

	SetChargeConverter();

}


PndMQMvdChargeWeightedPixelMapping::PndMQMvdChargeWeightedPixelMapping(PndGeoHandling* geo, PndSdsPixelDigiPar* digiPar, PndSdsTotDigiPar* totPar)
:PndMQSdsChargeWeightedPixelMapping(geo), fDigiPar(digiPar), fTotDigiPar(totPar)
{
	SetChargeConverter();
}

void PndMQMvdChargeWeightedPixelMapping::SetChargeConverter()
{
	LOG(INFO) << "ChargeConversionMethod: " << fDigiPar->GetChargeConvMethod() << " " << fDigiPar->GetThreshold() << FairLogger::endl;
    if (fDigiPar->GetChargeConvMethod() == 0){
			if(fVerbose>0) Info("PndMQMvdChargeWeightedPixelMapping","ideal charge conversion");
			fChargeConverter = new PndSdsIdealChargeConversion(fDigiPar->GetNoise());
	}
	else if (fDigiPar->GetChargeConvMethod() == 1){
		if(fVerbose>0) Info("PndMQMvdChargeWeightedPixelMapping","use TOT charge conversion");
		fChargeConverter = new PndSdsTotChargeConversion(
			fTotDigiPar->GetChargingTime(),
			fTotDigiPar->GetConstCurrent(),
			fDigiPar->GetThreshold(),
			fTotDigiPar->GetClockFrequency(),
			fVerbose);
	}
	else {
		Fatal("PndMQMvdChargeWeightedPixelMapping", "conversion method not defined!");
	}
	flx = fDigiPar->GetXPitch();
	fly = fDigiPar->GetYPitch();
	fcols = fDigiPar->GetFECols();
	frows = fDigiPar->GetFERows();
	LOG(INFO) << "ChargeConversionMethod: " << fDigiPar->GetChargeConvMethod() << " " << fChargeConverter << FairLogger::endl;
}
