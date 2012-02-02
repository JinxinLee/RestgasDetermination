/////////////////////////////////////////////////////////////
// PndEmcRecoPar
//
// Container class for Reconstruction parameters
//
/////////////////////////////////////////////////////////////

using namespace std;
#include "PndEmcRecoPar.h"


ClassImp(PndEmcRecoPar)

PndEmcRecoPar::PndEmcRecoPar(const char* name,const char* title,
			     const char* context)
    : FairParGenericSet(name,title,context) 
{
	clear();
}

void PndEmcRecoPar::putParams(FairParamList* list)
{
	if(!list) return;
	list->add("EnergyThresholdBarrel",fEnergyThresholdBarrel);
	list->add("EnergyThresholdFWD",fEnergyThresholdFWD);
	list->add("EnergyThresholdBWD",fEnergyThresholdBWD);
	list->add("EnergyThresholdShashlyk",fEnergyThresholdShashlyk);
	list->add("EmcClusterPosMethod",fEmcClusterPosMethod);	
        list->add("OffsetParmA",fOffsetParmA);
	list->add("OffsetParmB",fOffsetParmB);
	list->add("OffsetParmC",fOffsetParmC);
	list->add("EmcDigiPositionDepthPWO",fEmcDigiPositionDepthPWO);
	list->add("EmcDigiPositionDepthShashlyk",fEmcDigiPositionDepthShashlyk);
	
	// parameters related to bump splitting
	list->add("MaxECut",fMaxECut);
	list->add("NeighbourECut",fNeighbourECut);
	list->add("CutSlope",fCutSlope);
	list->add("CutOffset",fCutOffset);
	list->add("ERatioCorr",fERatioCorr);
	list->add("TheNeighbourLevel",fTheNeighbourLevel);
	list->add("MoliereRadius",fMoliereRadius);
	list->add("MoliereRadiusShashlyk",fMoliereRadiusShashlyk);
	list->add("ExponentialConstant",fExponentialConstant);
	list->add("MaxIterations",fMaxIterations);
	list->add("CentroidShift",fCentroidShift);
	list->add("MaxBumps",fMaxBumps);
	list->add("MinDigiEnergy",fMinDigiEnergy);
}
 
Bool_t PndEmcRecoPar::getParams(FairParamList* list)
{
	if (!list) return kFALSE;
	if (!list->fill("EnergyThresholdBarrel",&fEnergyThresholdBarrel)) return kFALSE;
	if (!list->fill("EnergyThresholdFWD",&fEnergyThresholdFWD)) return kFALSE;
	if (!list->fill("EnergyThresholdBWD",&fEnergyThresholdBWD)) return kFALSE;
	if (!list->fill("EnergyThresholdShashlyk",&fEnergyThresholdShashlyk)) return kFALSE;
	if (!list->fill("EmcClusterPosMethod",fEmcClusterPosMethod,10)) return kFALSE;
	if (!list->fill("OffsetParmA",&fOffsetParmA)) return kFALSE;
	if (!list->fill("OffsetParmB",&fOffsetParmB)) return kFALSE;
	if (!list->fill("OffsetParmC",&fOffsetParmC)) return kFALSE;
	if (!list->fill("EmcDigiPositionDepthPWO",&fEmcDigiPositionDepthPWO)) return kFALSE;
	if (!list->fill("EmcDigiPositionDepthShashlyk",&fEmcDigiPositionDepthShashlyk)) return kFALSE;
	
	// parameters related to bump splitting
	if (!list->fill("MaxECut",&fMaxECut)) return kFALSE;
	if (!list->fill("NeighbourECut",&fNeighbourECut)) return kFALSE;
	if (!list->fill("CutSlope",&fCutSlope)) return kFALSE;
	if (!list->fill("CutOffset",&fCutOffset)) return kFALSE;
	if (!list->fill("ERatioCorr",&fERatioCorr)) return kFALSE;
	if (!list->fill("TheNeighbourLevel",&fTheNeighbourLevel)) return kFALSE;
	if (!list->fill("MoliereRadius",&fMoliereRadius)) return kFALSE;
	if (!list->fill("MoliereRadiusShashlyk",&fMoliereRadiusShashlyk)) return kFALSE;
	if (!list->fill("ExponentialConstant",&fExponentialConstant)) return kFALSE;
	if (!list->fill("MaxIterations",&fMaxIterations)) return kFALSE;
	if (!list->fill("CentroidShift",&fCentroidShift)) return kFALSE;
	if (!list->fill("MaxBumps",&fMaxBumps)) return kFALSE;
	if (!list->fill("MinDigiEnergy",&fMinDigiEnergy)) return kFALSE;

	return kTRUE;
}

