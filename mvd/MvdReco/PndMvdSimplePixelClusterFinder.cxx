// A simple clusterfinder for pixel detectors
#include "PndMvdSimplePixelClusterFinder.h"
#include "PndSdsPixelDigiPar.h"
#include "PndSdsIdealChargeConversion.h"
#include "PndSdsTotDigiPar.h"
#include "PndSdsTotChargeConversion.h"

PndMvdSimplePixelClusterFinder::PndMvdSimplePixelClusterFinder(Int_t verbose):PndSdsSimplePixelClusterFinder(){
	fVerbose = verbose;
	FairRun* ana = FairRun::Instance();
	FairRuntimeDb* rtdb=ana->GetRuntimeDb();
	PndSdsPixelDigiPar* fDigiPar = (PndSdsPixelDigiPar*)(rtdb->getContainer("MVDPixelDigiPar"));
	PndSdsTotDigiPar* fTotDigiPar = (PndSdsTotDigiPar*)(rtdb->getContainer("MVDPixelTotDigiPar"));
	if (fDigiPar->GetChargeConvMethod() == 0){
		if(fVerbose>0) std::cout<<"Info in <PndMvdSimplePixelClusterFinder>: ideal charge conversion"<<std::endl;
		fChargeConverter = new PndSdsIdealChargeConversion(fDigiPar->GetNoise());
		}
	else if (fDigiPar->GetChargeConvMethod() == 1){
		if(fVerbose>0) std::cout<<"Info in <PndMvdSimplePixelClusterFinder>: TOT charge conversion"<<std::endl;
		fChargeConverter = new PndSdsTotChargeConversion(
		fTotDigiPar->GetChargingTime(),
		fTotDigiPar->GetConstCurrent(),
		fDigiPar->GetThreshold(),
		fTotDigiPar->GetClockFrequency(),
		fVerbose);
	}
	else{
		std::cout<<"Error in <PndMvdSimplePixelClusterFinder>: charge conversion method not defined, use ideal converter..."<<std::endl;
		fChargeConverter = new PndSdsIdealChargeConversion(fDigiPar->GetNoise());
	}
	fcols = fDigiPar->GetFECols();
	frows = fDigiPar->GetFERows();
	fradius = fDigiPar->GetClustRadius();
};

