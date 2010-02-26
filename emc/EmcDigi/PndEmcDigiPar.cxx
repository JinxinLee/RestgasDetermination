/////////////////////////////////////////////////////////////
// PndEmcDigiPar
//
// Container class for Digitisation parameters
// class is inherited from FairParGenericSet
//
/////////////////////////////////////////////////////////////

#include "PndEmcDigiPar.h"
#include <iostream>

ClassImp(PndEmcDigiPar)

PndEmcDigiPar::PndEmcDigiPar(const char* name,const char* title,
			     const char* context)
    : FairParGenericSet(name,title,context) 
{
	clear();//WHY?
}

void PndEmcDigiPar::putParams(FairParamList* list)
{
	if(!list) return;
	list->add("EnergyHitThreshold",fEnergyHitThreshold);
	list->add("DetectedPhotonsPerMeV",fDetectedPhotonsPerMeV);
	list->add("EnergyRange",fEnergyRange);
	list->add("EnergyRangeBW",fEnergyRangeBW);	
	list->add("ExcessNoiseFactor",fExcessNoiseFactor);
	list->add("FirstSamplePhase",fFirstSamplePhase);
	list->add("Shaping_diff_time",fShaping_diff_time);
	list->add("Shaping_int_time",fShaping_int_time);
	list->add("Crystal_time_constant",fCrystal_time_constant);
	list->add("Incoherent_elec_noise_width_GeV",fIncoherent_elec_noise_width_GeV);
	list->add("SampleRate",fSampleRate);
	list->add("EnergyDigiThreshold",fEnergyDigiThreshold);
	list->add("NBits",fNBits);
	list->add("Number_of_samples_in_waveform",fNumber_of_samples_in_waveform);
	list->add("Use_shaped_noise",fUse_shaped_noise);
	list->add("Use_photon_statistic",fUse_photon_statistic);
	list->add("UseDigiEffectiveSmearing",fUseDigiEffectiveSmearing);
	list->add("NoiseAllChannels",fNoiseAllChannels);
}

Bool_t PndEmcDigiPar::getParams(FairParamList* list)
{
	if (!list) return kFALSE;
	if (!list->fill("EnergyHitThreshold",&fEnergyHitThreshold)) return kFALSE;
	if (!list->fill("DetectedPhotonsPerMeV",&fDetectedPhotonsPerMeV)) return kFALSE;
	if (!list->fill("EnergyRange",&fEnergyRange)) return kFALSE;
	if (!list->fill("EnergyRangeBW",&fEnergyRangeBW)) return kFALSE;	
	if (!list->fill("ExcessNoiseFactor",&fExcessNoiseFactor)) return kFALSE;
	if (!list->fill("FirstSamplePhase",&fFirstSamplePhase)) return kFALSE;
	if (!list->fill("Shaping_diff_time",&fShaping_diff_time)) return kFALSE;
	if (!list->fill("Shaping_int_time",&fShaping_int_time)) return kFALSE;
	if (!list->fill("Crystal_time_constant",&fCrystal_time_constant)) return kFALSE;
	if (!list->fill("Incoherent_elec_noise_width_GeV",&fIncoherent_elec_noise_width_GeV)) return kFALSE;
	if (!list->fill("SampleRate",&fSampleRate)) return kFALSE;
	if (!list->fill("EnergyDigiThreshold",&fEnergyDigiThreshold)) return kFALSE;
	if (!list->fill("NBits",&fNBits)) return kFALSE;
	if (!list->fill("Number_of_samples_in_waveform",&fNumber_of_samples_in_waveform)) return kFALSE;
	if (!list->fill("Use_shaped_noise",&fUse_shaped_noise)) return kFALSE;
	if (!list->fill("Use_photon_statistic",&fUse_photon_statistic)) return kFALSE;
	if (!list->fill("UseDigiEffectiveSmearing",&fUseDigiEffectiveSmearing)) return kFALSE;
	if (!list->fill("NoiseAllChannels",&fNoiseAllChannels)) return kFALSE;
	
	return kTRUE;
}

