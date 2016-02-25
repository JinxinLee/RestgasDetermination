#pragma once
#ifndef PNDEMCRECOPAR_H
#define PNDEMCRECOPAR_H

#include "FairParGenericSet.h"
#include "FairParamList.h"

/**
 * @brief Parameter set for Emc Reco
 * @ingroup PndEmc
 */
class PndEmcRecoPar : public FairParGenericSet
{
	public :
		Double_t GetEnergyThresholdBarrel() {return fEnergyThresholdBarrel;};
		Double_t GetEnergyThresholdFWD() {return fEnergyThresholdFWD;};
		Double_t GetEnergyThresholdBWD() {return fEnergyThresholdBWD;};
		Double_t GetEnergyThresholdShashlyk() {return fEnergyThresholdShashlyk;};
		Double_t GetClusterActiveTime() {return fClusterActiveTime;};
		Text_t* GetEmcClusterPosMethod() {return fEmcClusterPosMethod;};
		Double_t GetOffsetParmA() {return fOffsetParmA;};
		Double_t GetOffsetParmB() {return fOffsetParmB;};
		Double_t GetOffsetParmC() {return fOffsetParmC;};
		Double_t GetEmcDigiPositionDepthPWO() {return fEmcDigiPositionDepthPWO;};
		Double_t GetEmcDigiPositionDepthShashlyk() {return fEmcDigiPositionDepthShashlyk;};
		Double_t GetMaxECut() {return fMaxECut;};
		Double_t GetNeighbourECut() {return fNeighbourECut;};
		Double_t GetCutSlope() {return fCutSlope;};
		Double_t GetCutOffset() {return fCutOffset;};
		Double_t GetERatioCorr() {return fERatioCorr;};
		Int_t GetTheNeighbourLevel() {return fTheNeighbourLevel;};
		Double_t GetMoliereRadius() {return fMoliereRadius;};
		Double_t GetMoliereRadiusShashlyk() {return fMoliereRadiusShashlyk;};
		Double_t GetExponentialConstant() {return fExponentialConstant;};
		Int_t GetMaxIterations() {return fMaxIterations;};
		Double_t GetCentroidShift() {return fCentroidShift;};
		Int_t GetMaxBumps() {return fMaxBumps;};
		Double_t GetMinDigiEnergy() {return fMinDigiEnergy;};
		

		PndEmcRecoPar (const char* name="PndEmcRecoPar",
			    const char* title="Emc reconstruction parameter",
			    const char* context="TestDefaultContext");
		~PndEmcRecoPar(void){};
		void clear(void){};
		void putParams(FairParamList* list);
		Bool_t getParams(FairParamList* list);
	
	private :
		Double_t fEnergyThresholdBarrel;   // Single crystal energy threshold for barrel EMC
		Double_t fEnergyThresholdFWD; // Single crystal energy threshold for forward endcup EMC
		Double_t fEnergyThresholdBWD; // Single crystal energy threshold for backward endcup EMC
		Double_t fEnergyThresholdShashlyk; // Single crystal energy threshold for shashlyk calorimeter
		Double_t fClusterActiveTime;	//time to keep cluster open in timebased clustering
		
		// Method to determine digi position
		Text_t fEmcClusterPosMethod[10]; // lilo, linear
		
		// The following 3 parameters are parameters for PndEmcClusterLiloPos to determine cluster position
		Double_t fOffsetParmA;
		Double_t fOffsetParmB;
		Double_t fOffsetParmC;

		Double_t fEmcDigiPositionDepthPWO;
		Double_t fEmcDigiPositionDepthShashlyk;
		
		// the following parameters are connected with bump splitting
		// parameters for PndEmc2DLocMaxFinder:
		Double_t fMaxECut;
		Double_t fNeighbourECut;
		Double_t fCutSlope;
		Double_t fCutOffset;
		Double_t fERatioCorr;
		Int_t fTheNeighbourLevel;
		// parameters for PndEmcExpClusterSplitter:
		Double_t fMoliereRadius;
		Double_t fMoliereRadiusShashlyk;
		Double_t fExponentialConstant;
		Int_t fMaxIterations;
		Double_t fCentroidShift;
		Int_t fMaxBumps;
		Double_t fMinDigiEnergy;

	
		ClassDef(PndEmcRecoPar, 2);
};

#endif
