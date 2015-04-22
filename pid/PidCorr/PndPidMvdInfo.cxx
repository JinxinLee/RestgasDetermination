#include "PndDetectorList.h"
#include "PndPidCorrelator.h"
#include "PndPidCandidate.h"
#include "PndTrack.h"
#include "PndTrackID.h"
#include "PndSdsHit.h"

#include "FairTrackParH.h"
#include "FairMCApplication.h"
#include "FairRunAna.h"
#include "FairRootManager.h"
#include "FairRuntimeDb.h"

#include "TObjArray.h"
#include "TVector3.h"
#include "TGeoMatrix.h"
#include "TGeoBBox.h"
#include "TGeoManager.h"

#include <cmath>

#include "PndPidCorrelator.h"
//_________________________________________________________________
Bool_t PndPidCorrelator::GetMvdInfo(PndTrack* track, PndPidCandidate* pidCand) 
{
  Float_t mvdELoss = 0.; // total energy lost in MVD;
  Float_t mvdPath = 0.;  // total thickness crossed in MVD
  Int_t mvdCounts = 0;
  Float_t SensorThickness=0;
  PndTrackCand trackCand = track->GetTrackCand();
  FairTrackParP  par = track->GetParamFirst();
  Int_t ierr = 0;
  FairTrackParH *helix = new FairTrackParH(&par, ierr); // This will be used for propagation
  
  for (Int_t ii=0; ii<trackCand.GetNHits(); ii++)
    {
      PndSdsHit *mvdHit = NULL;
      PndTrackCandHit candHit = trackCand.GetSortedHit(ii);
      if (fMixMode==kFALSE)
        if ( (candHit.GetDetId()!=FairRootManager::Instance()->GetBranchId("MVDHitsPixel")) && (candHit.GetDetId()!=FairRootManager::Instance()->GetBranchId("MVDHitsStrip")) ) continue;
      if (fMixMode==kTRUE)
        if ( (candHit.GetDetId()!=FairRootManager::Instance()->GetBranchId("MVDHitsPixelMix")) && (candHit.GetDetId()!=FairRootManager::Instance()->GetBranchId("MVDHitsStripMix")) ) continue;         

      if ( (candHit.GetDetId()==FairRootManager::Instance()->GetBranchId("MVDHitsPixel") && !fMixMode) ||
           (candHit.GetDetId()==FairRootManager::Instance()->GetBranchId("MVDHitsPixelMix") && fMixMode) )
        mvdHit = (PndSdsHit*)fMvdHitsPixel->At(candHit.GetHitId());
      if ( (candHit.GetDetId()==FairRootManager::Instance()->GetBranchId("MVDHitsStrip") && !fMixMode) ||
           (candHit.GetDetId()==FairRootManager::Instance()->GetBranchId("MVDHitsStripMix") && fMixMode))
        mvdHit = (PndSdsHit*)fMvdHitsStrip->At(candHit.GetHitId());
      if (mvdHit==0) continue;
      TVector3 mvdPos;
      mvdHit->Position(mvdPos);
      mvdCounts++;
      if (fFast) continue;
      TVector3 SensorDim=fGeoH->GetSensorDimensionsShortId(mvdHit->GetSensorID());//sensor dimension
      SensorThickness = SensorDim.Z();

      TGeoHMatrix *matrix = fGeoH->GetMatrixPath(fGeoH->GetPath(mvdHit->GetSensorID()));
      const Double_t *rotM = matrix->GetRotationMatrix();
      TVector3 zaxis(rotM[2], rotM[5], rotM[8]); // Z axis in the detector frame
      TVector3 momentum(0., 0., 0.);
      
      Double_t cos = 0.;
      if (ii==0) // If the MVD hit is the first, no need of propagation
	{ 
	  cos = TMath::Cos(helix->GetMomentum().Angle(zaxis)); 
	} 
      else
	{
	  if (fGeanePro) 
	    {
	      fGeanePropagator->SetPoint(mvdPos);
	      fGeanePropagator->PropagateToPCA(1, 1);
	      FairTrackParH *fRes= new FairTrackParH();
	      Bool_t rc =  fGeanePropagator->Propagate(helix, fRes, fPidHyp*pidCand->GetCharge()); // First propagation at module
	      if (rc)
		{
		  cos = TMath::Cos(fRes->GetMomentum().Angle(zaxis)); 
		  helix = fRes; //updates of helix params to propagate from the pervious hit to the next one
		}
	      else
		{
		  cos = 0.;
		}
	    }
	} // end of "if (ii==0) else"
      
      Float_t thickness = 0.; //projection of the momentum on the zaxis of the detector frame
      
      if (fabs(cos)<0.000001)
	{
	  std::cout << "-W- PndPidCorrelator::GetMvdInfo: Track perpendicular to MVD strip/pixel! Not added to MVD eloss" << std::endl;     
	}
      else
	{
	  thickness = SensorThickness*2./fabs(cos);
	  mvdELoss += mvdHit->GetEloss();
	  mvdPath += thickness;
	  fMvdHitCount++;
	}
      if (fVerbose>1) std::cout << mvdHit->GetSensorID() << "\t" << mvdHit->GetEloss() << "\t" << thickness << std::endl;
    }
  
  if (mvdPath>0.) pidCand->SetMvdDEDX(mvdELoss/mvdPath);
  pidCand->SetMvdHits(mvdCounts);
 
  return kTRUE;
}

ClassImp(PndPidCorrelator)
