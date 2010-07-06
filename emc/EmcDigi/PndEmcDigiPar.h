#pragma once
#ifndef PNDEMCDIGIPAR_H
#define PNDEMCDIGIPAR_H

#include "FairParGenericSet.h"
#include "FairParamList.h"

class PndEmcDigiPar : public FairParGenericSet
{
	public:
		Double_t GetEnergyHitThreshold() {return fEnergyHitThreshold;};
		Double_t GetDetectedPhotonsPerMeV() {return fDetectedPhotonsPerMeV;};
		Double_t GetSensitiveAreaAPD() {return fSensitiveAreaAPD;};
		Double_t GetSensitiveAreaVPT() {return fSensitiveAreaVPT;};
		Double_t GetQuantumEfficiencyAPD() {return fQuantumEfficiencyAPD;};
		Double_t GetQuantumEfficiencyVPT() {return fQuantumEfficiencyVPT;};
		Double_t GetExcessNoiseFactorAPD() {return fExcessNoiseFactorAPD;};
		Double_t GetExcessNoiseFactorVPT() {return fExcessNoiseFactorVPT;};
		Double_t GetIncoherent_elec_noise_width_GeV_APD() {return fIncoherent_elec_noise_width_GeV_APD;};
		Double_t GetIncoherent_elec_noise_width_GeV_VPT() {return fIncoherent_elec_noise_width_GeV_VPT;};
		Double_t GetEnergyRange() {return fEnergyRange;};
		Double_t GetEnergyRangeBW() {return fEnergyRangeBW;};
		Double_t GetFirstSamplePhase() {return fFirstSamplePhase;};
		Double_t GetASIC_Shaping_int_time() {return fASIC_Shaping_int_time;};
		Double_t GetCrystal_time_constant() {return fCrystal_time_constant;};
		Double_t GetSampleRate() {return fSampleRate;};
		Double_t GetEnergyDigiThreshold() {return fEnergyDigiThreshold;};
		Int_t GetNBits() {return fNBits;};
		Int_t GetNumber_of_samples_in_waveform() {return fNumber_of_samples_in_waveform;};
		Int_t GetUse_shaped_noise() {return fUse_shaped_noise;};
		Int_t GetUse_photon_statistic() {return fUse_photon_statistic;};
		Int_t GetUseDigiEffectiveSmearing() {return fUseDigiEffectiveSmearing;};
		Int_t GetNoiseAllChannels() {return fNoiseAllChannels;};
		Int_t GetUse_nonuniformity() {return fUse_nonuniformity;};
		
		PndEmcDigiPar (const char* name="PndEmcDigiPar",
			    const char* title="Emc digi parameter",
			    const char* context="TestDefaultContext");
		~PndEmcDigiPar(void){};
		
		// Empty method, WHY is this Called in constructor(FIXME)
		void clear(void){};
		
		void putParams(FairParamList* list);
		Bool_t getParams(FairParamList* list);
	
	private :
		Double_t fEnergyHitThreshold;
		Double_t fDetectedPhotonsPerMeV;
		Double_t fSensitiveAreaAPD; // mm^2
		Double_t fSensitiveAreaVPT; // mm^2
		Double_t fQuantumEfficiencyAPD;
		Double_t fQuantumEfficiencyVPT;
		Double_t fExcessNoiseFactorAPD;
		Double_t fExcessNoiseFactorVPT;
		Double_t fIncoherent_elec_noise_width_GeV_APD;
		Double_t fIncoherent_elec_noise_width_GeV_VPT;
		Double_t fEnergyRange;
		Double_t fEnergyRangeBW;
		Double_t fFirstSamplePhase;
		Double_t fASIC_Shaping_int_time;
		Double_t fCrystal_time_constant;
		Double_t fSampleRate;
		Double_t fEnergyDigiThreshold;
		Int_t fNBits;
		Int_t fNumber_of_samples_in_waveform;
		Int_t fUse_shaped_noise;
		Int_t fUse_photon_statistic;
		Int_t fUseDigiEffectiveSmearing;
		Int_t fNoiseAllChannels;
		Int_t fUse_nonuniformity;

		ClassDef(PndEmcDigiPar,7);
};

#endif
