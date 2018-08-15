#include "PndEmcFWEndcapDigiPar.h"
#include <iostream>

ClassImp(PndEmcFWEndcapDigiPar)

PndEmcFWEndcapDigiPar::PndEmcFWEndcapDigiPar(const char* name, const char* title, const char* context) :
	FairParGenericSet(name, title, context), fNBits(0), fPulseshapeTau(0.), fPulseshapeN(0.), fEnergyRangeHigh(0.), fEnergyRangeLow(0.), fNoiseWidthHigh(0.), fNoiseWidthLow(0.), fCalibHigh(0.), fCalibLow(0.) {
	clear();
}

void PndEmcFWEndcapDigiPar::putParams(FairParamList* list) {
	if(!list) return;

	list->add("TimeBeforeHit", fTimeBeforeHit);
	list->add("TimeAfterHit", fTimeAfterHit);
	list->add("WfCutOffEnergy", fWfCutOffEnergy);
	list->add("SampleRate", fSampleRate);

	list->add("PsaParLow", fPsaParLow);
	list->add("PsaParHigh", fPsaParHigh);
	list->add("PsaTypeLow", fPsaTypeLow);
	list->add("PsaTypeHigh", fPsaTypeHigh);
	list->add("RValueParaLow", fRValueParaLow);
	list->add("RValueParaHigh", fRValueParaHigh);
	list->add("SignalOverflowHigh", fSignalOverflowHigh);

	list->add("Use_photon_statistic",fUse_photon_statistic);
	list->add("QuantumEfficiencyAPD",fQuantumEfficiencyAPD);
	list->add("ExcessNoiseFactorAPD",fExcessNoiseFactorAPD);
	list->add("DetectedPhotonsPerMeV",fDetectedPhotonsPerMeV);
	list->add("SensitiveAreaAPD",fSensitiveAreaAPD);
	list->add("NBits", fNBits);
	list->add("PulseshapeTau", fPulseshapeTau);
	list->add("PulseshapeN", fPulseshapeN);
	list->add("EnergyRangeHigh", fEnergyRangeHigh);
	list->add("EnergyRangeLow", fEnergyRangeLow);
	list->add("NoiseWidthLow", fNoiseWidthLow);
	list->add("NoiseWidthHigh", fNoiseWidthHigh);
	list->add("EnergyDigiThreshold", fEnergyDigiThreshold);
	list->add("CalibHigh", fCalibHigh);
	list->add("CalibLow", fCalibLow);
}


Bool_t PndEmcFWEndcapDigiPar::getParams(FairParamList* list) {

	if(!list) return kFALSE;
	if (!list->fill("TimeBeforeHit", &fTimeBeforeHit)) return kFALSE;
	if (!list->fill("TimeAfterHit", &fTimeAfterHit)) return kFALSE;
	if (!list->fill("WfCutOffEnergy", &fWfCutOffEnergy)) return kFALSE;
	if (!list->fill("SampleRate", &fSampleRate)) return kFALSE;

	Text_t buf[512];
	if(!list->fill("PsaParLow", &fPsaParLow)) return kFALSE;
	if(!list->fill("PsaParHigh", &fPsaParHigh)) return kFALSE;
	if(!list->fill("PsaTypeLow", buf, 512)) return kFALSE;
	fPsaTypeLow = buf;
	if(!list->fill("PsaTypeHigh", buf, 512)) return kFALSE;
	fPsaTypeHigh = buf;
	if(!list->fill("RValueParaLow", buf, 512)) return kFALSE;
	fRValueParaLow = buf;
	if(!list->fill("RValueParaHigh", buf, 512)) return kFALSE;
	fRValueParaHigh = buf;
	if(!list->fill("SignalOverflowHigh", &fSignalOverflowHigh)) return kFALSE;

	/*other paras */
	if (!list->fill("Use_photon_statistic",&fUse_photon_statistic)) return kFALSE;
	if (!list->fill("QuantumEfficiencyAPD",&fQuantumEfficiencyAPD)) return kFALSE;
	if (!list->fill("ExcessNoiseFactorAPD",&fExcessNoiseFactorAPD)) return kFALSE;
	if (!list->fill("SensitiveAreaAPD",&fSensitiveAreaAPD)) return kFALSE;
	if (!list->fill("DetectedPhotonsPerMeV",&fDetectedPhotonsPerMeV)) return kFALSE;
	if (!list->fill("NBits", &fNBits)) return kFALSE;
	if (!list->fill("PulseshapeTau", &fPulseshapeTau)) return kFALSE;
	if (!list->fill("PulseshapeN", &fPulseshapeN)) return kFALSE;
	if (!list->fill("EnergyRangeHigh", &fEnergyRangeHigh)) return kFALSE;
	if (!list->fill("EnergyRangeLow", &fEnergyRangeLow)) return kFALSE;
	if (!list->fill("NoiseWidthLow", &fNoiseWidthLow)) return kFALSE;
	if (!list->fill("NoiseWidthHigh", &fNoiseWidthHigh)) return kFALSE;
	if(!list->fill("EnergyDigiThreshold", &fEnergyDigiThreshold)) return kFALSE;
	if(!list->fill("CalibHigh", &fCalibHigh)) return kFALSE;
	if(!list->fill("CalibLow", &fCalibLow)) return kFALSE;

	return kTRUE;
}

