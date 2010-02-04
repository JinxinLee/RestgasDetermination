#include "PndDetectorList.h"
#include "PndPidCorrelator.h"
#include "PndPidCandidate.h"
#include "PndTrack.h"
#include "PndTrackID.h"
#include "PndMvdHit.h"

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
void PndPidCorrelator::GetMvdInfo(PndTrack* track, PndPidCandidate* pidCand) 
{
  Float_t mvdELoss = 0.; // total energy lost in MVD;
  Float_t mvdPath = 0.;  // total thickness crossed in MVD
  Int_t mvdCounts = 0;
  PndTrackCand trackCand = track->GetTrackCand();
  for (Int_t ii=0; ii<trackCand.GetNHits(); ii++)
    {
      PndMvdHit *mvdHit = NULL;
      PndTrackCandHit candHit = trackCand.GetSortedHit(ii);
      if ( (candHit.GetDetId()!=kMVDHitsPixel) && (candHit.GetDetId()!=kMVDHitsStrip) ) continue;
      
      if (candHit.GetDetId()==kMVDHitsPixel) mvdHit = (PndMvdHit*)fMvdHitsPixel->At(candHit.GetHitId());
      if (candHit.GetDetId()==kMVDHitsStrip) mvdHit = (PndMvdHit*)fMvdHitsStrip->At(candHit.GetHitId());
      TVector3 mvdPos;
      mvdHit->Position(mvdPos);
      mvdCounts++;
      TGeoNode *mvdNode = (TGeoNode*)gGeoManager->FindNode(mvdHit->GetX(), mvdHit->GetY(), mvdHit->GetZ());
      TGeoVolume *mvdVol = (TGeoVolume*)mvdNode->GetVolume();
      TGeoBBox* actBox = (TGeoBBox*)(mvdVol->GetShape()); // volume of the MVD strip/pixel
      TGeoMatrix* mvdGeoRot = (TGeoMatrix*)mvdNode->GetMatrix();
      const Double_t *rotM = mvdGeoRot->GetRotationMatrix();
      TVector3 zaxis(rotM[2], rotM[5], rotM[8]); // Z axis in the detector frame
      TVector3 momentum(0., 0., 0.);
      FairTrackParP par = track->GetParamLast();
      Int_t ierr = 0;
      FairTrackParH *helix = new FairTrackParH(&par, ierr);
      Double_t cos = 0.;
      if (fGeanePro) 
    	{
	  FairGeanePro *fProMvd = new FairGeanePro();
	  fProMvd->SetPoint(mvdPos);
	  fProMvd->PropagateToPCA(1, -1);
	  FairTrackParH *fRes= new FairTrackParH();
          Bool_t rc =  fProMvd->Propagate(helix, fRes, -13*pidCand->GetCharge()); // First propagation at module
          if (rc)
	    {
	      cos = TMath::Cos(fRes->GetMomentum().Angle(zaxis)); 
	    }
	  else
	    {
	      cos = 0.;
	    }
	}
      Float_t thickness = 0.;
      
      if (fabs(cos)<0.000001)
	{
	  std::cout << "-W- PndPidCorrelator::GetMvdInfo: Track perpendicular to MVD strip/pixel! Not added to MVD eloss" << std::endl;     
	}
      else
	{
	  thickness = actBox->GetDZ()*2./fabs(cos);
	  mvdELoss += mvdHit->GetEloss();
	  mvdPath += thickness;
	  fMvdHitCount++;
	}
      if (fVerbose) std::cout << mvdHit->GetDetName() << "\t" << mvdHit->GetEloss() << "\t" << thickness << std::endl;
    }
  
  if (mvdPath>0.) pidCand->SetMvdDEDX(mvdELoss/mvdPath);
  pidCand->SetMvdHits(mvdCounts);
}

ClassImp(PndPidCorrelator)
